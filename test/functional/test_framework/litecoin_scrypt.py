#!/usr/bin/env python3
# Copyright (c) 2026 The Feathercoin Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.
"""Legacy scrypt-compatible PoW helper used pre-NeoScrypt fork."""

from ._neoscrypt_native import get_pow_hash


def getPoWHash(header_80_bytes):
    # Legacy profile used before the NeoScrypt fork in Feathercoin consensus.
    return get_pow_hash(header_80_bytes, profile=0x3)
