// Copyright (c) 2013-2019 Feathercoin developers
// Copyright (c) 2011-2013 PPCoin developers
// Copyright (c) 2013 Primecoin developers
// Distributed under conditional MIT/X11 open source software license
// see the accompanying file COPYING
#ifndef BITCOIN_CHECKPOINTSYNC_H
#define  BITCOIN_CHECKPOINTSYNC_H

#include <serialize.h>
#include <uint256.h>

#include <string>
#include <vector>

class CNode;
class CBlockIndex;
class CSyncCheckpoint;
class uint256;

extern uint256 hashSyncCheckpoint;
extern CSyncCheckpoint checkpointMessage;

bool WriteSyncCheckpoint(const uint256& hashCheckpoint);
bool AcceptPendingSyncCheckpoint();
uint256 AutoSelectSyncCheckpoint();
bool CheckSyncCheckpoint(const CBlockIndex* pindexNew);
bool ResetSyncCheckpoint();
void AskForPendingSyncCheckpoint(CNode* pfrom);
bool CheckCheckpointPubKey();
bool SetCheckpointPrivKey(std::string strPrivKey);
bool SendSyncCheckpoint(uint256 hashCheckpoint);

// Synchronized checkpoint (introduced first in ppcoin)
class CUnsignedSyncCheckpoint
{
public:
    int nVersion;
    uint256 hashCheckpoint;      // checkpoint block

    ADD_SERIALIZE_METHODS
    template <typename Stream, typename Operation>
    inline void SerializationOp(Stream& s, Operation ser_action) {
        READWRITE(this->nVersion);
        READWRITE(hashCheckpoint);
    }

    void SetNull();
    std::string ToString() const;
    void print() const;
};

class CSyncCheckpoint : public CUnsignedSyncCheckpoint
{
public:
    static std::string strMasterPrivKey;
    std::vector<unsigned char> vchMsg;
    std::vector<unsigned char> vchSig;

    CSyncCheckpoint();

    ADD_SERIALIZE_METHODS
    
    template <typename Stream, typename Operation>
    inline void SerializationOp(Stream& s, Operation ser_action) {
        READWRITE(vchMsg);
        READWRITE(vchSig);
    }

    void SetNull();
    bool IsNull() const;
    uint256 GetHash() const;
    bool RelayTo(CNode* pfrom) const;
    bool CheckSignature();
    bool ProcessSyncCheckpoint(CNode* pfrom = nullptr);
};

#endif
