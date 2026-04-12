// Copyright (c) 2013-2019 Feathercoin developers
// Copyright (c) 2011-2013 PPCoin developers
// Copyright (c) 2013 Primecoin developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_CHECKPOINTSYNC_H
#define BITCOIN_CHECKPOINTSYNC_H

#include <serialize.h>
#include <sync.h>
#include <uint256.h>

#include <string>
#include <vector>

class CBlockIndex;
class CConnman;
class CNode;

static constexpr int DEFAULT_AUTOCHECKPOINT{5};
static constexpr bool DEFAULT_CHECKPOINT_SYNC_ENABLED{true};

extern uint256 hashSyncCheckpoint;
extern RecursiveMutex cs_hashSyncCheckpoint;
extern CConnman* g_connman;
extern bool fSyncCheckpointsEnabled;

class CSyncCheckpoint;
extern CSyncCheckpoint checkpointMessage;

bool WriteSyncCheckpoint(const uint256& hashCheckpoint);
bool AcceptPendingSyncCheckpoint();
uint256 AutoSelectSyncCheckpoint();
bool CheckSyncCheckpoint(const uint256& hashBlock, int nHeight, const CBlockIndex* pindexPrev = nullptr);
bool CheckCheckpointPubKey();
bool SetCheckpointPrivKey(const std::string& strPrivKey);
bool SendSyncCheckpoint(const uint256& hashCheckpoint);
void NotifySyncCheckpointIBDExit();
bool MaybeReconcileSyncCheckpoint();

class CUnsignedSyncCheckpoint
{
public:
    int nVersion;
    uint256 hashCheckpoint;

    SERIALIZE_METHODS(CUnsignedSyncCheckpoint, obj) { READWRITE(obj.nVersion, obj.hashCheckpoint); }

    void SetNull();
    std::string ToString() const;
};

class CSyncCheckpoint : public CUnsignedSyncCheckpoint
{
public:
    static std::string strMasterPrivKey;
    std::vector<unsigned char> vchMsg;
    std::vector<unsigned char> vchSig;

    CSyncCheckpoint();

    SERIALIZE_METHODS(CSyncCheckpoint, obj) { READWRITE(obj.vchMsg, obj.vchSig); }

    void SetNull();
    bool IsNull() const;
    uint256 GetHash() const;
    void RelayTo(CNode* pfrom) const;
    bool CheckSignature();
    bool ProcessSyncCheckpoint();
};

#endif // BITCOIN_CHECKPOINTSYNC_H
