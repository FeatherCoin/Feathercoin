#!/usr/bin/env python3
# Copyright (c) 2026 The Feathercoin Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.
"""Native NeoScrypt bridge for functional tests."""

from ctypes import CDLL, POINTER, c_ubyte, c_uint
from pathlib import Path
import os
import subprocess
import threading


_LOCK = threading.Lock()
_LIB = None


def _shared_object_path():
    module_dir = Path(__file__).resolve().parent
    build_dir = module_dir / ".native"
    build_dir.mkdir(exist_ok=True)
    return build_dir / "libneoscrypt.so"


def _repo_root():
    # .../test/functional/test_framework -> repo root is parent[3]
    return Path(__file__).resolve().parents[3]


def _build_shared_object(so_path):
    cc = os.environ.get("CC", "cc")
    root = _repo_root()
    source = root / "src" / "crypto" / "neoscrypt.c"
    include = root / "src"
    cmd = [
        cc,
        "-shared",
        "-fPIC",
        "-O2",
        "-std=c99",
        f"-I{include}",
        str(source),
        "-o",
        str(so_path),
    ]
    subprocess.check_call(cmd)


def _get_lib():
    global _LIB
    with _LOCK:
        if _LIB is not None:
            return _LIB
        so_path = _shared_object_path()
        if not so_path.exists():
            _build_shared_object(so_path)
        lib = CDLL(str(so_path))
        lib.neoscrypt.argtypes = [POINTER(c_ubyte), POINTER(c_ubyte), c_uint]
        lib.neoscrypt.restype = None
        _LIB = lib
        return _LIB


def get_pow_hash(header_80_bytes, profile):
    if len(header_80_bytes) != 80:
        raise ValueError("NeoScrypt input must be exactly 80 bytes")
    lib = _get_lib()
    in_buf = (c_ubyte * 80).from_buffer_copy(header_80_bytes)
    out_buf = (c_ubyte * 32)()
    lib.neoscrypt(in_buf, out_buf, c_uint(profile))
    return bytes(out_buf)
