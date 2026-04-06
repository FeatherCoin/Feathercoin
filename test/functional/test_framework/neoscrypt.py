#!/usr/bin/env python3
# Copyright (c) 2026 The Feathercoin Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.
"""NeoScrypt hash helper for functional tests."""

from ._neoscrypt_native import get_pow_hash


def getPoWHash(header_80_bytes):
    # NeoScrypt profile used post-fork in Feathercoin consensus.
    return get_pow_hash(header_80_bytes, profile=0x0)
