// Copyright (c) 2013-2019 Feathercoin developers
// Copyright (c) 2011-2013 PPCoin developers
// Copyright (c) 2013 Primecoin developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <checkpointsync.h>

#include <chain.h>
#include <chainparams.h>
#include <consensus/validation.h>
#include <hash.h>
#include <key.h>
#include <key_io.h>
#include <logging.h>
#include <net.h>
#include <netmessagemaker.h>
#include <txdb.h>
#include <util/strencodings.h>
#include <util/system.h>
#include <validation.h>

#include <atomic>

std::string CSyncCheckpoint::strMasterPrivKey;
uint256 hashSyncCheckpoint;
static uint256 hashPendingCheckpoint;
CSyncCheckpoint checkpointMessage;
static CSyncCheckpoint checkpointMessagePending;
RecursiveMutex cs_hashSyncCheckpoint;
CConnman* g_connman{nullptr};
bool fSyncCheckpointsEnabled = DEFAULT_CHECKPOINT_SYNC_ENABLED;
static std::atomic_bool g_reconcile_sync_checkpoint_after_ibd{false};

static bool HasUsableCheckpointChain(const CBlockIndex* pindex)
{
    return pindex && (::ChainActive().Contains(pindex) ||
        (pindex->IsValid(BLOCK_VALID_TRANSACTIONS) && pindex->HaveTxsDownloaded()));
}

static bool ValidateSyncCheckpoint(const uint256& hashCheckpoint)
{
    CBlockIndex* pindexSyncCheckpoint = nullptr;
    CBlockIndex* pindexCheckpointRecv = nullptr;

    {
        LOCK2(cs_main, cs_hashSyncCheckpoint);
        pindexSyncCheckpoint = LookupBlockIndex(hashSyncCheckpoint);
        pindexCheckpointRecv = LookupBlockIndex(hashCheckpoint);
        if (!pindexSyncCheckpoint) {
            return error("%s: missing current sync-checkpoint %s", __func__, hashSyncCheckpoint.ToString());
        }
        if (!pindexCheckpointRecv) {
            return error("%s: missing received sync-checkpoint %s", __func__, hashCheckpoint.ToString());
        }
    }

    if (pindexCheckpointRecv->nHeight <= pindexSyncCheckpoint->nHeight) {
        CBlockIndex* pindex = pindexSyncCheckpoint;
        while (pindex->nHeight > pindexCheckpointRecv->nHeight) {
            if (!(pindex = pindex->pprev)) {
                return error("%s: pprev1 null - block index structure failure", __func__);
            }
        }
        if (pindex->GetBlockHash() != hashCheckpoint) {
            LOCK(cs_hashSyncCheckpoint);
            return error("%s: new sync-checkpoint %s conflicts with current %s", __func__, hashCheckpoint.ToString(), hashSyncCheckpoint.ToString());
        }
        return false;
    }

    CBlockIndex* pindex = pindexCheckpointRecv;
    while (pindex->nHeight > pindexSyncCheckpoint->nHeight) {
        if (!(pindex = pindex->pprev)) {
            return error("%s: pprev2 null - block index structure failure", __func__);
        }
    }

    {
        LOCK(cs_hashSyncCheckpoint);
        if (pindex->GetBlockHash() != hashSyncCheckpoint) {
            return error("%s: new sync-checkpoint %s is not a descendant of current %s", __func__, hashCheckpoint.ToString(), hashSyncCheckpoint.ToString());
        }
    }

    return true;
}

bool WriteSyncCheckpoint(const uint256& hashCheckpoint)
{
    if (!pblocktree->WriteSyncCheckpoint(hashCheckpoint)) {
        return error("%s: failed to write sync checkpoint %s", __func__, hashCheckpoint.ToString());
    }

    hashSyncCheckpoint = hashCheckpoint;
    return true;
}

bool AcceptPendingSyncCheckpoint()
{
    {
        LOCK2(cs_main, cs_hashSyncCheckpoint);
        const bool havePendingCheckpoint = !hashPendingCheckpoint.IsNull() && LookupBlockIndex(hashPendingCheckpoint);
        if (!havePendingCheckpoint) {
            return false;
        }
    }

    uint256 hashPendingCheckpointTmp;
    {
        LOCK(cs_hashSyncCheckpoint);
        hashPendingCheckpointTmp = hashPendingCheckpoint;
    }

    if (!ValidateSyncCheckpoint(hashPendingCheckpointTmp)) {
        LOCK(cs_hashSyncCheckpoint);
        hashPendingCheckpoint.SetNull();
        checkpointMessagePending.SetNull();
        return false;
    }

    {
        LOCK2(cs_main, cs_hashSyncCheckpoint);
        const CBlockIndex* pending = LookupBlockIndex(hashPendingCheckpoint);
        if (!pending || !::ChainActive().Contains(pending)) {
            return false;
        }
    }

    {
        LOCK(cs_hashSyncCheckpoint);
        if (!WriteSyncCheckpoint(hashPendingCheckpoint)) {
            return error("%s: failed to write sync checkpoint %s", __func__, hashPendingCheckpoint.ToString());
        }

        hashPendingCheckpoint.SetNull();
        checkpointMessage = checkpointMessagePending;
        checkpointMessagePending.SetNull();

        if (g_connman && !checkpointMessage.IsNull()) {
            g_connman->ForEachNode([](CNode* pnode) {
                if (pnode->supportACPMessages) {
                    checkpointMessage.RelayTo(pnode);
                }
            });
        }
    }

    return true;
}

uint256 AutoSelectSyncCheckpoint()
{
    LOCK(cs_main);
    const CBlockIndex* pindex = ::ChainActive().Tip();
    if (!pindex) {
        return uint256();
    }
    while (pindex->pprev && pindex->nHeight + gArgs.GetArg("-checkpointdepth", DEFAULT_AUTOCHECKPOINT) > ::ChainActive().Tip()->nHeight) {
        pindex = pindex->pprev;
    }
    return pindex->GetBlockHash();
}

bool CheckSyncCheckpoint(const uint256& hashBlock, int nHeight, const CBlockIndex* pindexPrev)
{
    if (nHeight == 0) {
        return true;
    }

    if (!fSyncCheckpointsEnabled) {
        return true;
    }

    LOCK2(cs_main, cs_hashSyncCheckpoint);
    if (hashSyncCheckpoint.IsNull()) {
        return true;
    }

    const CBlockIndex* pindexSync = LookupBlockIndex(hashSyncCheckpoint);
    assert(pindexSync);

    if (nHeight > pindexSync->nHeight) {
        const CBlockIndex* pindex = pindexPrev ? pindexPrev : ::ChainActive().Tip();
        while (pindex && pindex->nHeight > pindexSync->nHeight) {
            pindex = pindex->pprev;
        }
        if (!pindex) {
            return error("%s: pprev null - block index structure failure", __func__);
        }
        if (pindex->nHeight < pindexSync->nHeight) {
            return error("%s: rewound below sync-checkpoint height", __func__);
        }
        if (pindex->GetBlockHash() != hashSyncCheckpoint) {
            return error("%s: mismatched block hash at sync height %d", __func__, pindex->nHeight);
        }
    }

    if (nHeight == pindexSync->nHeight && hashBlock != hashSyncCheckpoint) {
        return error("%s: same height with sync-checkpoint", __func__);
    }

    return true;
}

void NotifySyncCheckpointIBDExit()
{
    g_reconcile_sync_checkpoint_after_ibd = true;
}

static bool ReconcileSyncCheckpointHash(const uint256& hashCheckpoint, bool& switched)
{
    switched = false;

    CBlockIndex* index = nullptr;
    CBlockIndex* bad_fork = nullptr;
    {
        LOCK2(cs_main, cs_hashSyncCheckpoint);
        index = LookupBlockIndex(hashCheckpoint);
        if (!HasUsableCheckpointChain(index)) {
            return false;
        }
        if (!::ChainActive().Contains(index)) {
            const CBlockIndex* ancestor = LastCommonAncestor(index, ::ChainActive().Tip());
            if (ancestor) {
                bad_fork = ::ChainActive().Next(ancestor);
            }
        } else {
            return true;
        }
    }

    if (bad_fork && index && index->GetAncestor(bad_fork->nHeight) != bad_fork) {
        BlockValidationState state;
        InvalidateBlock(state, Params(), bad_fork);
        if (state.IsValid() && !::ChainstateActive().ActivateBestChain(state, Params(), nullptr)) {
            return error("%s: failed to activate best chain for sync-checkpoint %s (%s)", __func__, hashCheckpoint.ToString(), state.ToString());
        }
        if (!state.IsValid()) {
            return error("%s: failed to switch to sync-checkpoint %s (%s)", __func__, hashCheckpoint.ToString(), state.ToString());
        }
    }

    {
        LOCK2(cs_main, cs_hashSyncCheckpoint);
        index = LookupBlockIndex(hashCheckpoint);
        if (!index || !::ChainActive().Contains(index)) {
            return false;
        }
    }

    switched = true;
    return true;
}

bool MaybeReconcileSyncCheckpoint()
{
    if (!g_reconcile_sync_checkpoint_after_ibd.exchange(false)) {
        return true;
    }

    if (!fSyncCheckpointsEnabled) {
        return true;
    }

    if (::ChainstateActive().IsInitialBlockDownload()) {
        g_reconcile_sync_checkpoint_after_ibd = true;
        return true;
    }

    LogPrintf("ACP synchronized checkpoint enforcement enabled after IBD.\n");

    uint256 pendingHash;
    CSyncCheckpoint pendingMessage;
    bool havePending = false;
    bool pendingWasActive = false;
    {
        LOCK2(cs_main, cs_hashSyncCheckpoint);
        if (!hashPendingCheckpoint.IsNull() && !checkpointMessagePending.IsNull()) {
            pendingHash = hashPendingCheckpoint;
            pendingMessage = checkpointMessagePending;
            havePending = true;
            const CBlockIndex* pendingIndex = LookupBlockIndex(pendingHash);
            pendingWasActive = pendingIndex && ::ChainActive().Contains(pendingIndex);
        }
    }

    if (havePending && pendingMessage.ProcessSyncCheckpoint()) {
        bool pendingIsActive = false;
        {
            LOCK2(cs_main, cs_hashSyncCheckpoint);
            const CBlockIndex* pendingIndex = LookupBlockIndex(pendingHash);
            pendingIsActive = pendingIndex && ::ChainActive().Contains(pendingIndex);
        }
        if (!pendingWasActive && pendingIsActive) {
            LogPrintf("ACP switched to synchronized checkpoint chain %s after IBD.\n", pendingHash.ToString());
        }
        return true;
    }

    uint256 currentSyncCheckpoint;
    {
        LOCK(cs_hashSyncCheckpoint);
        currentSyncCheckpoint = hashSyncCheckpoint;
    }

    bool switched = false;
    if (!currentSyncCheckpoint.IsNull() && ReconcileSyncCheckpointHash(currentSyncCheckpoint, switched)) {
        if (switched) {
            LogPrintf("ACP switched to synchronized checkpoint chain %s after IBD.\n", currentSyncCheckpoint.ToString());
        } else if (havePending) {
            LogPrintf("ACP enforcement active after IBD, but pending sync-checkpoint %s is not on a usable chain yet.\n", pendingHash.ToString());
        }
        return true;
    }

    if (havePending) {
        LogPrintf("ACP enforcement active after IBD, but pending sync-checkpoint %s is not on a usable chain yet.\n", pendingHash.ToString());
    } else {
        LogPrintf("ACP enforcement active after IBD, but no usable synchronized checkpoint branch is available yet.\n");
    }

    return true;
}

static bool ResetSyncCheckpoint()
{
    LOCK(cs_hashSyncCheckpoint);
    if (!WriteSyncCheckpoint(Params().GetConsensus().hashGenesisBlock)) {
        return error("%s: failed to reset sync checkpoint to genesis", __func__);
    }
    return true;
}

bool CheckCheckpointPubKey()
{
    const std::string strMasterPubKey = Params().GetConsensus().checkpointPubKey;
    if (strMasterPubKey.empty()) {
        LOCK(cs_hashSyncCheckpoint);
        hashSyncCheckpoint.SetNull();
        checkpointMessage.SetNull();
        return true;
    }

    std::string strPubKey;
    if (!pblocktree->ReadCheckpointPubKey(strPubKey) || strPubKey != strMasterPubKey) {
        if (!ResetSyncCheckpoint()) {
            return error("%s: failed to reset sync-checkpoint", __func__);
        }
        if (!pblocktree->WriteCheckpointPubKey(strMasterPubKey)) {
            return error("%s: failed to write checkpoint master key", __func__);
        }
    }

    return true;
}

bool SetCheckpointPrivKey(const std::string& strPrivKey)
{
    CKey key = DecodeSecret(strPrivKey);
    if (!key.IsValid()) {
        return false;
    }

    CSyncCheckpoint checkpoint;
    checkpoint.hashCheckpoint = Params().GetConsensus().hashGenesisBlock;
    CDataStream sMsg(SER_NETWORK, PROTOCOL_VERSION);
    sMsg << static_cast<CUnsignedSyncCheckpoint>(checkpoint);
    checkpoint.vchMsg.assign(sMsg.begin(), sMsg.end());

    if (!key.Sign(Hash(MakeSpan(checkpoint.vchMsg)), checkpoint.vchSig)) {
        return false;
    }

    const std::string strMasterPubKey = Params().GetConsensus().checkpointPubKey;
    CPubKey pubkey(ParseHex(strMasterPubKey));
    if (!pubkey.IsValid()) {
        return false;
    }
    if (!pubkey.Verify(Hash(MakeSpan(checkpoint.vchMsg)), checkpoint.vchSig)) {
        return false;
    }

    CSyncCheckpoint::strMasterPrivKey = strPrivKey;
    return true;
}

bool SendSyncCheckpoint(const uint256& hashCheckpoint)
{
    if (hashCheckpoint.IsNull()) {
        return true;
    }

    CSyncCheckpoint checkpoint;
    checkpoint.hashCheckpoint = hashCheckpoint;
    CDataStream sMsg(SER_NETWORK, PROTOCOL_VERSION);
    sMsg << static_cast<CUnsignedSyncCheckpoint>(checkpoint);
    checkpoint.vchMsg.assign(sMsg.begin(), sMsg.end());

    if (CSyncCheckpoint::strMasterPrivKey.empty()) {
        return error("%s: checkpoint master key unavailable", __func__);
    }

    CKey key = DecodeSecret(CSyncCheckpoint::strMasterPrivKey);
    if (!key.IsValid()) {
        return error("%s: checkpoint master key invalid", __func__);
    }

    if (!key.Sign(Hash(MakeSpan(checkpoint.vchMsg)), checkpoint.vchSig)) {
        return error("%s: unable to sign checkpoint", __func__);
    }

    if (!checkpoint.ProcessSyncCheckpoint()) {
        return error("%s: failed to process checkpoint", __func__);
    }

    if (g_connman && g_connman->GetNodeCount(CConnman::CONNECTIONS_ALL) > 0) {
        g_connman->ForEachNode([checkpoint](CNode* pnode) {
            checkpoint.RelayTo(pnode);
        });
    }

    return true;
}

void CUnsignedSyncCheckpoint::SetNull()
{
    nVersion = 1;
    hashCheckpoint.SetNull();
}

std::string CUnsignedSyncCheckpoint::ToString() const
{
    return strprintf("CSyncCheckpoint(\n    nVersion=%d\n    hashCheckpoint=%s\n)\n", nVersion, hashCheckpoint.ToString());
}

CSyncCheckpoint::CSyncCheckpoint()
{
    SetNull();
}

void CSyncCheckpoint::SetNull()
{
    CUnsignedSyncCheckpoint::SetNull();
    vchMsg.clear();
    vchSig.clear();
}

bool CSyncCheckpoint::IsNull() const
{
    return hashCheckpoint.IsNull();
}

uint256 CSyncCheckpoint::GetHash() const
{
    return Hash(MakeSpan(vchMsg));
}

void CSyncCheckpoint::RelayTo(CNode* pfrom) const
{
    if (!g_connman || !pfrom->supportACPMessages) {
        return;
    }

    bool should_relay{false};
    {
        LOCK(pfrom->cs_inventory);
        if (pfrom->hashCheckpointKnown != hashCheckpoint) {
            pfrom->hashCheckpointKnown = hashCheckpoint;
            should_relay = true;
        }
    }
    if (should_relay) {
        g_connman->PushMessage(pfrom, CNetMsgMaker(pfrom->GetCommonVersion()).Make(NetMsgType::CHECKPOINT, *this));
    }
}

bool CSyncCheckpoint::CheckSignature()
{
    const std::string strMasterPubKey = Params().GetConsensus().checkpointPubKey;
    CPubKey key(ParseHex(strMasterPubKey));
    if (!key.IsValid()) {
        return error("%s: checkpoint public key invalid", __func__);
    }
    if (!key.Verify(Hash(MakeSpan(vchMsg)), vchSig)) {
        return error("%s: verify signature failed", __func__);
    }

    CDataStream sMsg(vchMsg, SER_NETWORK, PROTOCOL_VERSION);
    sMsg >> *static_cast<CUnsignedSyncCheckpoint*>(this);
    return true;
}

bool CSyncCheckpoint::ProcessSyncCheckpoint()
{
    if (!CheckSignature()) {
        return false;
    }

    {
        LOCK2(cs_main, cs_hashSyncCheckpoint);
        const CBlockIndex* index = LookupBlockIndex(hashCheckpoint);
        if (!HasUsableCheckpointChain(index)) {
            const bool already_pending = hashPendingCheckpoint == hashCheckpoint;
            hashPendingCheckpoint = hashCheckpoint;
            checkpointMessagePending = *this;
            if (!already_pending) {
                LogPrintf("%s: pending for sync-checkpoint %s\n", __func__, hashCheckpoint.ToString());
            }
            return false;
        }
    }

    if (!ValidateSyncCheckpoint(hashCheckpoint)) {
        return false;
    }

    CBlockIndex* bad_fork = nullptr;
    CBlockIndex* index = nullptr;
    {
        LOCK2(cs_main, cs_hashSyncCheckpoint);
        index = LookupBlockIndex(hashCheckpoint);
        if (index && !::ChainActive().Contains(index)) {
            const CBlockIndex* ancestor = LastCommonAncestor(index, ::ChainActive().Tip());
            if (ancestor) {
                bad_fork = ::ChainActive().Next(ancestor);
            }
        }
    }

    if (bad_fork && index && index->GetAncestor(bad_fork->nHeight) != bad_fork) {
        BlockValidationState state;
        InvalidateBlock(state, Params(), bad_fork);
        if (state.IsValid()) {
            ActivateBestChain(state, Params());
        }
    }

    {
        LOCK2(cs_main, cs_hashSyncCheckpoint);
        index = LookupBlockIndex(hashCheckpoint);
        if (!index || !::ChainActive().Contains(index)) {
            const bool already_pending = hashPendingCheckpoint == hashCheckpoint;
            hashPendingCheckpoint = hashCheckpoint;
            checkpointMessagePending = *this;
            if (!already_pending) {
                LogPrintf("%s: waiting for sync-checkpoint chain %s to activate\n", __func__, hashCheckpoint.ToString());
            }
            return false;
        }

        if (!WriteSyncCheckpoint(hashCheckpoint)) {
            return error("%s: failed to write sync checkpoint %s", __func__, hashCheckpoint.ToString());
        }
        checkpointMessage = *this;
        hashPendingCheckpoint.SetNull();
        checkpointMessagePending.SetNull();
    }

    return true;
}
