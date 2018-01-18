// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2013 The Bitcoin developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_OPENKEY_H
#define BITCOIN_OPENKEY_H

#include "support/allocators.h"
#include "hash.h"
#include "serialize.h"
#include "uint256.h"

#include <stdexcept>
#include <vector>

// secp256k1:
// const unsigned int PRIVATE_KEY_SIZE = 279;
// const unsigned int PUBLIC_KEY_SIZE  = 65;
// const unsigned int SIGNATURE_SIZE   = 72;
//
// see www.keylength.com
// script supports up to 75 for single byte push

/** A reference to a COpenKey: the Hash160 of its serialized public key */
class COpenKeyID : public uint160
{
public:
    COpenKeyID() : uint160() { }
    COpenKeyID(const uint160 &in) : uint160(in) { }
};

/** An encapsulated public key. */
class COpenPubKey {
private:
    // Just store the serialized data.
    // Its length can very cheaply be computed from the first byte.
    unsigned char vch[65];

    // Compute the length of a pubkey with a given first byte.
    unsigned int static GetLen(unsigned char chHeader) {
        if (chHeader == 2 || chHeader == 3)
            return 33;
        if (chHeader == 4 || chHeader == 6 || chHeader == 7)
            return 65;
        return 0;
    }

    // Set this key data to be invalid
    void Invalidate() {
        vch[0] = 0xFF;
    }

public:
    // Construct an invalid public key.
    COpenPubKey() {
        Invalidate();
    }

    // Initialize a public key using begin/end iterators to byte data.
    template<typename T>
    void Set(const T pbegin, const T pend) {
        int len = pend == pbegin ? 0 : GetLen(pbegin[0]);
        if (len && len == (pend-pbegin))
            memcpy(vch, (unsigned char*)&pbegin[0], len);
        else
            Invalidate();
    }

    // Construct a public key using begin/end iterators to byte data.
    template<typename T>
    COpenPubKey(const T pbegin, const T pend) {
        Set(pbegin, pend);
    }

    // Construct a public key from a byte vector.
    COpenPubKey(const std::vector<unsigned char> &vch) {
        Set(vch.begin(), vch.end());
    }

    // Simple read-only vector-like interface to the pubkey data.
    unsigned int size() const { return GetLen(vch[0]); }
    const unsigned char *begin() const { return vch; }
    const unsigned char *end() const { return vch+size(); }
    const unsigned char &operator[](unsigned int pos) const { return vch[pos]; }

    // Comparator implementation.
    friend bool operator==(const COpenPubKey &a, const COpenPubKey &b) {
        return a.vch[0] == b.vch[0] &&
               memcmp(a.vch, b.vch, a.size()) == 0;
    }
    friend bool operator!=(const COpenPubKey &a, const COpenPubKey &b) {
        return !(a == b);
    }
    friend bool operator<(const COpenPubKey &a, const COpenPubKey &b) {
        return a.vch[0] < b.vch[0] ||
               (a.vch[0] == b.vch[0] && memcmp(a.vch, b.vch, a.size()) < 0);
    }

    // Implement serialization, as if this was a byte vector.
    unsigned int GetSerializeSize(int nType, int nVersion) const {
        return size() + 1;
    }
    template<typename Stream> void Serialize(Stream &s, int nType, int nVersion) const {
        unsigned int len = size();
        ::WriteCompactSize(s, len);
        s.write((char*)vch, len);
    }
    template<typename Stream> void Unserialize(Stream &s, int nType, int nVersion) {
        unsigned int len = ::ReadCompactSize(s);
        if (len <= 65) {
            s.read((char*)vch, len);
        } else {
            // invalid pubkey, skip available data
            char dummy;
            while (len--)
                s.read(&dummy, 1);
            Invalidate();
        }
    }

    // Get the KeyID of this public key (hash of its serialization)
    COpenKeyID GetID() const {
        return COpenKeyID(Hash160(vch, vch+size()));
    }

    // Get the 256-bit hash of this public key.
    uint256 GetHash() const {
        return Hash(vch, vch+size());
    }

    // Check syntactic correctness.
    //
    // Note that this is consensus critical as CheckSig() calls it!
    bool IsValid() const {
        return size() > 0;
    }

    // fully validate whether this is a valid public key (more expensive than IsValid())
    bool IsFullyValid() const;

    // Check whether this is a compressed public key.
    bool IsCompressed() const {
        return size() == 33;
    }

    // Verify a DER signature (~72 bytes).
    // If this public key is not fully valid, the return value will be false.
    bool Verify(const uint256 &hash, const std::vector<unsigned char>& vchSig) const;

    // Verify a compact signature (~65 bytes).
    // See COpenKey::SignCompact.
    bool VerifyCompact(const uint256 &hash, const std::vector<unsigned char>& vchSig) const;

    // Recover a public key from a compact signature.
    bool RecoverCompact(const uint256 &hash, const std::vector<unsigned char>& vchSig);

    // Turn this public key into an uncompressed public key.
    bool Decompress();

    // Derive BIP32 child pubkey.
    bool Derive(COpenPubKey& pubkeyChild, unsigned char ccChild[32], unsigned int nChild, const unsigned char cc[32]) const;
};


// secure_open_allocator is defined in allocators.h
// COpenPrivKey is a serialized private key, with all parameters included (279 bytes)
typedef std::vector<unsigned char, secure_open_allocator<unsigned char> > COpenPrivKey;

/** An encapsulated private key. */
class COpenKey {
private:
    // Whether this private key is valid. We check for correctness when modifying the key
    // data, so fValid should always correspond to the actual state.
    bool fValid;

    // Whether the public key corresponding to this private key is (to be) compressed.
    bool fCompressed;

    // The actual byte data
    unsigned char vch[32];

    // Check whether the 32-byte array pointed to be vch is valid keydata.
    bool static Check(const unsigned char *vch);
public:

    // Construct an invalid private key.
    COpenKey() : fValid(false) {
        LockOpenObject(vch);
    }

    // Copy constructor. This is necessary because of memlocking.
    COpenKey(const COpenKey &secret) : fValid(secret.fValid), fCompressed(secret.fCompressed) {
        LockOpenObject(vch);
        memcpy(vch, secret.vch, sizeof(vch));
    }

    // Destructor (again necessary because of memlocking).
    ~COpenKey() {
        UnlockOpenObject(vch);
    }

    friend bool operator==(const COpenKey &a, const COpenKey &b) {
        return a.fCompressed == b.fCompressed && a.size() == b.size() &&
               memcmp(&a.vch[0], &b.vch[0], a.size()) == 0;
    }

    // Initialize using begin and end iterators to byte data.
    template<typename T>
    void Set(const T pbegin, const T pend, bool fCompressedIn) {
        if (pend - pbegin != 32) {
            fValid = false;
            return;
        }
        if (Check(&pbegin[0])) {
            memcpy(vch, (unsigned char*)&pbegin[0], 32);
            fValid = true;
            fCompressed = fCompressedIn;
        } else {
            fValid = false;
        }
    }

    // Simple read-only vector-like interface.
    unsigned int size() const { return (fValid ? 32 : 0); }
    const unsigned char *begin() const { return vch; }
    const unsigned char *end() const { return vch + size(); }

    // Check whether this private key is valid.
    bool IsValid() const { return fValid; }

    // Check whether the public key corresponding to this private key is (to be) compressed.
    bool IsCompressed() const { return fCompressed; }

    // Initialize from a COpenPrivKey (serialized OpenSSL private key data).
    bool SetPrivKey(const COpenPrivKey &vchPrivKey, bool fCompressed);

    // Generate a new private key using a cryptographic PRNG.
    void MakeNewKey(bool fCompressed);

    // Convert the private key to a COpenPrivKey (serialized OpenSSL private key data).
    // This is expensive.
    COpenPrivKey GetPrivKey() const;

    // Compute the public key from a private key.
    // This is expensive.
    COpenPubKey GetPubKey() const;

    // Create a DER-serialized signature.
    bool Sign(const uint256 &hash, std::vector<unsigned char>& vchSig) const;

    // Create a compact signature (65 bytes), which allows reconstructing the used public key.
    // The format is one header byte, followed by two times 32 bytes for the serialized r and s values.
    // The header byte: 0x1B = first key with even y, 0x1C = first key with odd y,
    //                  0x1D = second key with even y, 0x1E = second key with odd y,
    //                  add 0x04 for compressed keys.
    bool SignCompact(const uint256 &hash, std::vector<unsigned char>& vchSig) const;

    // Derive BIP32 child key.
    bool Derive(COpenKey& keyChild, unsigned char ccChild[32], unsigned int nChild, const unsigned char cc[32]) const;

    // Load private key and check that public key matches.
    bool Load(COpenPrivKey &privkey, COpenPubKey &vchPubKey, bool fSkipCheck);
};

/** Check that required EC support is available at runtime */
bool ECC_OpenSanityCheck(void);

#endif