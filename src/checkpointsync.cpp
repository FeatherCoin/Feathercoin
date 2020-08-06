// Copyright (c) 2013-2019 Feathercoin developers
// Copyright (c) 2011-2013 PPCoin developers
// Copyright (c) 2013 Primecoin developers
// Distributed under conditional MIT/X11 software license,
// see the accompanying file COPYING
//
// The synchronized checkpoint system is first developed by Sunny King for
// ppcoin network in 2012, giving cryptocurrency developers a tool to gain
// additional network protection against 51% attack.
//
// Primecoin also adopts this security mechanism, and the enforcement of
// checkpoints is explicitly granted by user, thus granting only temporary
// consensual central control to developer at the threats of 51% attack.
//
// Concepts
//
// In the network there can be a privileged node known as 'checkpoint master'.
// This node can send out checkpoint messages signed by the checkpoint master
// key. Each checkpoint is a block hash, representing a block on the blockchain
// that the network should reach consensus on.
//
// Besides verifying signatures of checkpoint messages, each node also verifies
// the consistency of the checkpoints. If a conflicting checkpoint is received,
// it means either the checkpoint master key is compromised, or there is an
// operator mistake. In this situation the node would discard the conflicting
// checkpoint message and display a warning message. This precaution controls
// the damage to network caused by operator mistake or compromised key.
//
// Operations
//
// Any node can be turned into checkpoint master by setting the 'checkpointkey'
// configuration parameter with the private key of the checkpoint master key.
// Operator should exercise caution such that at any moment there is at most
// one node operating as checkpoint master. When switching master node, the
// recommended procedure is to shutdown the master node and restart as
// regular node, note down the current checkpoint by 'getcheckpoint', then
// compare to the checkpoint at the new node to be upgraded to master node.
// When the checkpoint on both nodes match then it is safe to switch the new
// node to checkpoint master.
//
// The configuration parameter 'checkpointdepth' specifies how many blocks
// should the checkpoints lag behind the latest block in auto checkpoint mode.
// A depth of 5 is the minimum auto checkpoint policy and offers the greatest
// protection against 51% attack.
//

#include <checkpointsync.h>

#include <base58.h>
#include <chainparams.h>
#include <consensus/validation.h>
#include <main.h>
#include <txdb.h>

// Synchronized checkpoint (centrally broadcasted)
std::string CSyncCheckpoint::strMasterPrivKey;
uint256 hashSyncCheckpoint;
static uint256 hashPendingCheckpoint;
CSyncCheckpoint checkpointMessage;
static CSyncCheckpoint checkpointMessagePending;
CCriticalSection cs_hashSyncCheckpoint;


// Only descendant of current sync-checkpoint is allowed
bool ValidateSyncCheckpoint(uint256 hashCheckpoint)
{
    CBlockIndex* pindexSyncCheckpoint = nullptr;
    CBlockIndex* pindexCheckpointRecv = nullptr;

    {
        LOCK2(cs_main, cs_hashSyncCheckpoint);
        if (!mapBlockIndex.count(hashSyncCheckpoint))
            return error("%s: block index missing for current sync-checkpoint %s", __func__, hashSyncCheckpoint.ToString());
        if (!mapBlockIndex.count(hashCheckpoint))
            return error("%s: block index missing for received sync-checkpoint %s", __func__, hashCheckpoint.ToString());

        pindexSyncCheckpoint = mapBlockIndex[hashSyncCheckpoint];
        pindexCheckpointRecv = mapBlockIndex[hashCheckpoint];
    }

    if (pindexCheckpointRecv->nHeight <= pindexSyncCheckpoint->nHeight)
    {
        // Received an older checkpoint, trace back from current checkpoint
        // to the same height of the received checkpoint to verify
        // that current checkpoint should be a descendant block
        CBlockIndex* pindex = pindexSyncCheckpoint;
        while (pindex->nHeight > pindexCheckpointRecv->nHeight)
            if (!(pindex = pindex->pprev))
                return error("%s: pprev1 null - block index structure failure", __func__);
        if (pindex->GetBlockHash() != hashCheckpoint)
        {
            LOCK(cs_hashSyncCheckpoint);
            return error("%s: new sync-checkpoint %s is conflicting with current sync-checkpoint %s", __func__, hashCheckpoint.ToString(), hashSyncCheckpoint.ToString());
        }
        return false; // ignore older checkpoint
    }

    // Received checkpoint should be a descendant block of the current
    // checkpoint. Trace back to the same height of current checkpoint
    // to verify.
    CBlockIndex* pindex = pindexCheckpointRecv;
    while (pindex->nHeight > pindexSyncCheckpoint->nHeight)
        if (!(pindex = pindex->pprev))
            return error("%s: pprev2 null - block index structure failure", __func__);

    {
        LOCK(cs_hashSyncCheckpoint);
        if (pindex->GetBlockHash() != hashSyncCheckpoint)
        {
            return error("%s: new sync-checkpoint %s is not a descendant of current sync-checkpoint %s", __func__, hashCheckpoint.ToString(), hashSyncCheckpoint.ToString());
        }
    }

    return true;
}

bool WriteSyncCheckpoint(const uint256& hashCheckpoint)
{
    if (!pblocktree->WriteSyncCheckpoint(hashCheckpoint))
        return error("%s: failed to write to txdb sync checkpoint %s", __func__, hashCheckpoint.ToString());

    hashSyncCheckpoint = hashCheckpoint;
    return true;
}

bool AcceptPendingSyncCheckpoint()
{
    {
        LOCK2(cs_main, cs_hashSyncCheckpoint);
        bool havePendingCheckpoint = hashPendingCheckpoint != uint256() && mapBlockIndex.count(hashPendingCheckpoint);
        if (!havePendingCheckpoint)
            return false;
    }

    uint256 hashPendingCheckpointTmp;
    {
        LOCK(cs_hashSyncCheckpoint);
        hashPendingCheckpointTmp = hashPendingCheckpoint;
    }

    if (!ValidateSyncCheckpoint(hashPendingCheckpointTmp))
    {
        LOCK(cs_hashSyncCheckpoint);
        hashPendingCheckpoint = uint256();
        checkpointMessagePending.SetNull();
        return false;
    }

    {
        LOCK2(cs_main, cs_hashSyncCheckpoint);
        if (!chainActive.Contains(mapBlockIndex[hashPendingCheckpoint]))
            return false;
    }

    {
        LOCK(cs_hashSyncCheckpoint);
        if (!WriteSyncCheckpoint(hashPendingCheckpoint)) {
            return error("%s: failed to write sync checkpoint %s", __func__, hashPendingCheckpoint.ToString());
        }

        hashPendingCheckpoint = uint256();
        checkpointMessage = checkpointMessagePending;
        checkpointMessagePending.SetNull();

        // Relay the checkpoint
        if (!checkpointMessage.IsNull())
        {
            for (auto* pnode : vNodes) {
                if (pnode->supportACPMessages)
                    checkpointMessage.RelayTo(pnode);
            }
        }
    }

    return true;
}

// Automatically select a suitable sync-checkpoint
uint256 AutoSelectSyncCheckpoint()
{
    LOCK(cs_main);
    // Search backward for a block with specified depth policy
    const CBlockIndex *pindex = chainActive.Tip();
    while (pindex->pprev && pindex->nHeight + GetArg("-checkpointdepth", DEFAULT_AUTOCHECKPOINT) > chainActive.Tip()->nHeight)
        pindex = pindex->pprev;
    return pindex->GetBlockHash();
}

// Check against synchronized checkpoint
bool CheckSyncCheckpoint(const uint256 hashBlock, const int nHeight)
{
    // Genesis block
    if (nHeight == 0) {
        return true;
    }

    CBlockIndex* tip = nullptr;
    {
        LOCK(cs_main);
        tip = chainActive.Tip();
    }

    {
        LOCK(cs_hashSyncCheckpoint);

        // Checkpoint on default
        if (hashSyncCheckpoint == uint256()) {
            return true;
        }
    }

    CBlockIndex* pindexSyncTemp = nullptr;
    {
        LOCK2(cs_main, cs_hashSyncCheckpoint);
        // sync-checkpoint should always be accepted block
        assert(mapBlockIndex.count(hashSyncCheckpoint));
        pindexSyncTemp = mapBlockIndex[hashSyncCheckpoint];
    }

    const CBlockIndex* pindexSync = pindexSyncTemp;

    {
        LOCK(cs_hashSyncCheckpoint);
        if (nHeight > pindexSync->nHeight)
        {
            // Trace back to same height as sync-checkpoint
            const CBlockIndex* pindex = tip;
            while (pindex->nHeight > pindexSync->nHeight)
                if (!(pindex = pindex->pprev))
                    return error("%s: pprev null - block index structure failure", __func__);
            if (pindex->nHeight < pindexSync->nHeight || pindex->GetBlockHash() != hashSyncCheckpoint)
                return false; // only descendant of sync-checkpoint can pass check
        }
        if (nHeight == pindexSync->nHeight && hashBlock != hashSyncCheckpoint)
            return error("%s: Same height with sync-checkpoint", __func__);
    }

    {
        LOCK2(cs_main, cs_hashSyncCheckpoint);
        if (nHeight < pindexSync->nHeight && !mapBlockIndex.count(hashBlock))
            return error("%s: Lower height than sync-checkpoint", __func__);
    }
    return true;
}

// Reset synchronized checkpoint to the genesis block
bool ResetSyncCheckpoint()
{
    LOCK(cs_hashSyncCheckpoint);

    if (!WriteSyncCheckpoint(Params().GetConsensus().hashGenesisBlock))
        return error("%s: failed to reset sync checkpoint to genesis block", __func__);

    return true;
}

// Verify sync checkpoint master pubkey and reset sync checkpoint if changed
bool CheckCheckpointPubKey()
{
    std::string strPubKey = "";
    std::string strMasterPubKey = Params().GetConsensus().checkpointPubKey;

    if (!pblocktree->ReadCheckpointPubKey(strPubKey) || strPubKey != strMasterPubKey)
    {
        // write checkpoint master key to db
        if (!ResetSyncCheckpoint())
            return error("%s: failed to reset sync-checkpoint", __func__);
        if (!pblocktree->WriteCheckpointPubKey(strMasterPubKey))
            return error("%s: failed to write new checkpoint master key to db", __func__);
    }

    return true;
}

void CUnsignedSyncCheckpoint::SetNull()
{
    nVersion = 1;
    hashCheckpoint = uint256();
}

std::string CUnsignedSyncCheckpoint::ToString() const
{
    return strprintf(
            "CSyncCheckpoint(\n"
            "    nVersion       = %d\n"
            "    hashCheckpoint = %s\n"
            ")\n",
        nVersion,
        hashCheckpoint.ToString());
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
    return (hashCheckpoint == uint256());
}

uint256 CSyncCheckpoint::GetHash() const
{
    return Hash(this->vchMsg.begin(), this->vchMsg.end());
}

void CSyncCheckpoint::RelayTo(CNode* pfrom) const
{
    if (pfrom->hashCheckpointKnown != hashCheckpoint && pfrom->supportACPMessages)
    {
        pfrom->hashCheckpointKnown = hashCheckpoint;
        pfrom->PushMessage(NetMsgType::CHECKPOINT, *this);
    }
}

// Verify signature of sync-checkpoint message
bool CSyncCheckpoint::CheckSignature()
{
    std::string strMasterPubKey = Params().GetConsensus().checkpointPubKey;
    CPubKey key(ParseHex(strMasterPubKey));
    if (!key.Verify(Hash(vchMsg.begin(), vchMsg.end()), vchSig))
        return error("%s: verify signature failed", __func__);

    // Now unserialize the data
    CDataStream sMsg(vchMsg, SER_NETWORK, PROTOCOL_VERSION);
    sMsg >> *static_cast<CUnsignedSyncCheckpoint*>(this);
    return true;
}

// Process synchronized checkpoint
bool CSyncCheckpoint::ProcessSyncCheckpoint()
{
    if (!CheckSignature())
        return false;

    {
        LOCK2(cs_main, cs_hashSyncCheckpoint);

        if (!mapBlockIndex.count(hashCheckpoint)) {
            // We haven't received the checkpoint chain, keep the checkpoint as pending
            hashPendingCheckpoint = hashCheckpoint;
            checkpointMessagePending = *this;
            LogPrintf("%s: pending for sync-checkpoint %s\n", __func__, hashCheckpoint.ToString());

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

        // Check if we're on a fork
        index = mapBlockIndex[hashCheckpoint];
        if (!chainActive.Contains(index)) {
            auto ancestor = LastCommonAncestor(index, chainActive.Tip());
            bad_fork = chainActive.Next(ancestor);
        }
    }

    if (bad_fork && index && index->GetAncestor(bad_fork->nHeight) != bad_fork) {
        CValidationState state;
        InvalidateBlock(state, Params(), bad_fork);

        if (state.IsValid()) {
            ActivateBestChain(state, Params());
        }
    }

    {
        LOCK(cs_hashSyncCheckpoint);
        if (!WriteSyncCheckpoint(hashCheckpoint)) {
            return error("%s: failed to write sync checkpoint %s\n", __func__, hashCheckpoint.ToString());
        }

        checkpointMessage = *this;
        hashPendingCheckpoint = uint256();
        checkpointMessagePending.SetNull();
    }

    return true;
}
