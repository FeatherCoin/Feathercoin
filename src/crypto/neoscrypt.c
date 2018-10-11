/*
 * Copyright (c) 2009 Colin Percival, 2011 ArtForz
 * Copyright (c) 2012 Andrew Moon (floodyberry)
 * Copyright (c) 2012 Samuel Neves <sneves@dei.uc.pt>
 * Copyright (c) 2014-2016 John Doering <ghostlander@phoenixcoin.org>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */


#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "crypto/neoscrypt.h"

/* SHA-256 */

static const unsigned int sha256_constants[64] = {
    0x428A2F98, 0x71374491, 0xB5C0FBCF, 0xE9B5DBA5, 0x3956C25B, 0x59F111F1, 0x923F82A4, 0xAB1C5ED5,
    0xD807AA98, 0x12835B01, 0x243185BE, 0x550C7DC3, 0x72BE5D74, 0x80DEB1FE, 0x9BDC06A7, 0xC19BF174,
    0xE49B69C1, 0xEFBE4786, 0x0FC19DC6, 0x240CA1CC, 0x2DE92C6F, 0x4A7484AA, 0x5CB0A9DC, 0x76F988DA,
    0x983E5152, 0xA831C66D, 0xB00327C8, 0xBF597FC7, 0xC6E00BF3, 0xD5A79147, 0x06CA6351, 0x14292967,
    0x27B70A85, 0x2E1B2138, 0x4D2C6DFC, 0x53380D13, 0x650A7354, 0x766A0ABB, 0x81C2C92E, 0x92722C85,
    0xA2BFE8A1, 0xA81A664B, 0xC24B8B70, 0xC76C51A3, 0xD192E819, 0xD6990624, 0xF40E3585, 0x106AA070,
    0x19A4C116, 0x1E376C08, 0x2748774C, 0x34B0BCB5, 0x391C0CB3, 0x4ED8AA4A, 0x5B9CCA4F, 0x682E6FF3,
    0x748F82EE, 0x78A5636F, 0x84C87814, 0x8CC70208, 0x90BEFFFA, 0xA4506CEB, 0xBEF9A3F7, 0xC67178F2
};

#define Ch(x,y,z)  (z ^ (x & (y ^ z)))
#define Maj(x,y,z) (((x | y) & z) | (x & y))
#define S0(x)      (ROTR32(x,  2) ^ ROTR32(x, 13) ^ ROTR32(x, 22))
#define S1(x)      (ROTR32(x,  6) ^ ROTR32(x, 11) ^ ROTR32(x, 25))
#define G0(x)      (ROTR32(x,  7) ^ ROTR32(x, 18) ^ (x >>  3))
#define G1(x)      (ROTR32(x, 17) ^ ROTR32(x, 19) ^ (x >> 10))
#define W0(in,i)   (U8TO32_BE(&in[i * 4]))
#define W1(i)      (G1(w[i - 2]) + w[i - 7] + G0(w[i - 15]) + w[i - 16])
#define STEP(i) \
    t1 = S0(r[0]) + Maj(r[0], r[1], r[2]); \
    t0 = r[7] + S1(r[4]) + Ch(r[4], r[5], r[6]) + sha256_constants[i] + w[i]; \
    r[7] = r[6]; \
    r[6] = r[5]; \
    r[5] = r[4]; \
    r[4] = r[3] + t0; \
    r[3] = r[2]; \
    r[2] = r[1]; \
    r[1] = r[0]; \
    r[0] = t0 + t1;

typedef struct sha256_hash_state_t {
    unsigned int H[8];
    unsigned long long T;
    unsigned int leftover;
    unsigned char buffer[BLOCK_SIZE];
} sha256_hash_state;

static void sha256_blocks(sha256_hash_state *S, const unsigned char *in, unsigned int blocks) {
    unsigned int r[8], w[64], t0, t1, i;

    for(i = 0; i < 8; i++)
      r[i] = S->H[i];

    while(blocks--) {
        for(i =  0; i < 16; i++) {
            w[i] = W0(in, i);
        }
        for(i = 16; i < 64; i++) {
            w[i] = W1(i);
        }
        for(i =  0; i < 64; i++) {
            STEP(i);
        }
        for(i =  0; i <  8; i++) {
            r[i] += S->H[i];
            S->H[i] = r[i];
        }
        S->T += BLOCK_SIZE * 8;
        in += BLOCK_SIZE;
    }
}

static void neoscrypt_hash_init_sha256(sha256_hash_state *S) {
    S->H[0] = 0x6A09E667;
    S->H[1] = 0xBB67AE85;
    S->H[2] = 0x3C6EF372;
    S->H[3] = 0xA54FF53A;
    S->H[4] = 0x510E527F;
    S->H[5] = 0x9B05688C;
    S->H[6] = 0x1F83D9AB;
    S->H[7] = 0x5BE0CD19;
    S->T = 0;
    S->leftover = 0;
}

static void neoscrypt_hash_update_sha256(sha256_hash_state *S, const unsigned char *in, unsigned int inlen) {
    unsigned int blocks, want;

    /* handle the previous data */
    if(S->leftover) {
        want = (BLOCK_SIZE - S->leftover);
        want = (want < inlen) ? want : inlen;
        neoscrypt_copy(S->buffer + S->leftover, in, want);
        S->leftover += (unsigned int)want;
        if(S->leftover < BLOCK_SIZE)
          return;
        in += want;
        inlen -= want;
        sha256_blocks(S, S->buffer, 1);
    }

    /* handle the current data */
    blocks = (inlen & ~(BLOCK_SIZE - 1));
    S->leftover = (unsigned int)(inlen - blocks);
    if(blocks) {
        sha256_blocks(S, in, blocks / BLOCK_SIZE);
        in += blocks;
    }

    /* handle leftover data */
    if(S->leftover)
      neoscrypt_copy(S->buffer, in, S->leftover);
}

static void neoscrypt_hash_finish_sha256(sha256_hash_state *S, unsigned char *hash) {
    unsigned long long t = S->T + (S->leftover * 8);

    S->buffer[S->leftover] = 0x80;
    if(S->leftover <= 55) {
        neoscrypt_erase(S->buffer + S->leftover + 1, 55 - S->leftover);
    } else {
        neoscrypt_erase(S->buffer + S->leftover + 1, 63 - S->leftover);
        sha256_blocks(S, S->buffer, 1);
        neoscrypt_erase(S->buffer, 56);
    }

    U64TO8_BE(S->buffer + 56, t);
    sha256_blocks(S, S->buffer, 1);

    U32TO8_BE(&hash[ 0], S->H[0]);
    U32TO8_BE(&hash[ 4], S->H[1]);
    U32TO8_BE(&hash[ 8], S->H[2]);
    U32TO8_BE(&hash[12], S->H[3]);
    U32TO8_BE(&hash[16], S->H[4]);
    U32TO8_BE(&hash[20], S->H[5]);
    U32TO8_BE(&hash[24], S->H[6]);
    U32TO8_BE(&hash[28], S->H[7]);
}


/* HMAC for SHA-256 */

typedef struct sha256_hmac_state_t {
    sha256_hash_state inner, outer;
} sha256_hmac_state;

static inline void neoscrypt_hmac_init_sha256(sha256_hmac_state *st,
  const unsigned char *key, unsigned int keylen) {
    unsigned char pad[BLOCK_SIZE + DIGEST_SIZE];
    unsigned int *P = (unsigned int *) pad;
    unsigned int i;

    /* The pad initialisation for the inner loop */
    for(i = 0; i < (BLOCK_SIZE >> 2); i++)
      P[i] = 0x36363636;

    if(keylen <= BLOCK_SIZE) {
        /* XOR the key into the pad */
        neoscrypt_xor(pad, key, keylen);
    } else {
        /* Hash the key and XOR into the pad */
        sha256_hash_state st0;
        neoscrypt_hash_init_sha256(&st0);
        neoscrypt_hash_update_sha256(&st0, key, keylen);
        neoscrypt_hash_finish_sha256(&st0, &pad[BLOCK_SIZE]);
        neoscrypt_xor(&pad[0], &pad[BLOCK_SIZE], DIGEST_SIZE);
    }

    neoscrypt_hash_init_sha256(&st->inner);
    /* h(inner || pad) */
    neoscrypt_hash_update_sha256(&st->inner, pad, BLOCK_SIZE);

    /* The pad re-initialisation for the outer loop */
    for(i = 0; i < (BLOCK_SIZE >> 2); i++)
       P[i] ^= (0x36363636 ^ 0x5C5C5C5C);

    neoscrypt_hash_init_sha256(&st->outer);
    /* h(outer || pad) */
    neoscrypt_hash_update_sha256(&st->outer, pad, BLOCK_SIZE);
}

static inline void neoscrypt_hmac_update_sha256(sha256_hmac_state *st,
  const unsigned char *m, unsigned int mlen) {
    /* h(inner || m...) */
    neoscrypt_hash_update_sha256(&st->inner, m, mlen);
}

static inline void neoscrypt_hmac_finish_sha256(sha256_hmac_state *st,
  hash_digest mac) {
    /* h(inner || m) */
    hash_digest innerhash;
    neoscrypt_hash_finish_sha256(&st->inner, innerhash);

    /* h(outer || h(inner || m)) */
    neoscrypt_hash_update_sha256(&st->outer, innerhash, sizeof(innerhash));
    neoscrypt_hash_finish_sha256(&st->outer, mac);
}


/* PBKDF2 for SHA-256 */

void neoscrypt_pbkdf2_sha256(const unsigned char *password, unsigned int password_len,
  const unsigned char *salt, unsigned int salt_len, unsigned int N, unsigned char *output, unsigned int output_len) {
    sha256_hmac_state hmac_pw, hmac_pw_salt, work;
    hash_digest ti, u;
    unsigned char be[4];
    unsigned int i, j, k, blocks;

    /* bytes must be <= (0xffffffff - (DIGEST_SIZE - 1)), which they will always be under scrypt */

    /* hmac(password, ...) */
    neoscrypt_hmac_init_sha256(&hmac_pw, password, password_len);

    /* hmac(password, salt...) */
    hmac_pw_salt = hmac_pw;
    neoscrypt_hmac_update_sha256(&hmac_pw_salt, salt, salt_len);

    blocks = ((unsigned int)output_len + (DIGEST_SIZE - 1)) / DIGEST_SIZE;
    for(i = 1; i <= blocks; i++) {
        /* U1 = hmac(password, salt || be(i)) */
        U32TO8_BE(be, i);
        work = hmac_pw_salt;
        neoscrypt_hmac_update_sha256(&work, be, 4);
        neoscrypt_hmac_finish_sha256(&work, ti);
        neoscrypt_copy(u, ti, sizeof(u));

        /* T[i] = U1 ^ U2 ^ U3... */
        for(j = 0; j < N - 1; j++) {
            /* UX = hmac(password, U{X-1}) */
            work = hmac_pw;
            neoscrypt_hmac_update_sha256(&work, u, DIGEST_SIZE);
            neoscrypt_hmac_finish_sha256(&work, u);

            /* T[i] ^= UX */
            for(k = 0; k < sizeof(u); k++)
              ti[k] ^= u[k];
        }

        neoscrypt_copy(output, ti, (output_len > DIGEST_SIZE) ? DIGEST_SIZE : output_len);
        output += DIGEST_SIZE;
        output_len -= DIGEST_SIZE;
    }
}


/* NeoScrypt */

#ifdef USE_ASM

extern void neoscrypt_copy(void *dstp, const void *srcp, unsigned int len);
extern void neoscrypt_erase(void *dstp, unsigned int len);
extern void neoscrypt_xor(void *dstp, const void *srcp, unsigned int len);

#else

/* Salsa20, rounds must be a multiple of 2 */
static void neoscrypt_salsa(unsigned int *X, unsigned int rounds) {
    unsigned int x0, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11, x12, x13, x14, x15, t;

    x0 = X[0];   x1 = X[1];   x2 = X[2];   x3 = X[3];
    x4 = X[4];   x5 = X[5];   x6 = X[6];   x7 = X[7];
    x8 = X[8];   x9 = X[9];  x10 = X[10]; x11 = X[11];
   x12 = X[12]; x13 = X[13]; x14 = X[14]; x15 = X[15];

#define quarter(a, b, c, d) \
    t = a + d; t = ROTL32(t,  7); b ^= t; \
    t = b + a; t = ROTL32(t,  9); c ^= t; \
    t = c + b; t = ROTL32(t, 13); d ^= t; \
    t = d + c; t = ROTL32(t, 18); a ^= t;

    for(; rounds; rounds -= 2) {
        quarter( x0,  x4,  x8, x12);
        quarter( x5,  x9, x13,  x1);
        quarter(x10, x14,  x2,  x6);
        quarter(x15,  x3,  x7, x11);
        quarter( x0,  x1,  x2,  x3);
        quarter( x5,  x6,  x7,  x4);
        quarter(x10, x11,  x8,  x9);
        quarter(x15, x12, x13, x14);
    }

    X[0] += x0;   X[1] += x1;   X[2] += x2;   X[3] += x3;
    X[4] += x4;   X[5] += x5;   X[6] += x6;   X[7] += x7;
    X[8] += x8;   X[9] += x9;  X[10] += x10; X[11] += x11;
   X[12] += x12; X[13] += x13; X[14] += x14; X[15] += x15;

#undef quarter
}

/* ChaCha20, rounds must be a multiple of 2 */
static void neoscrypt_chacha(unsigned int *X, unsigned int rounds) {
    unsigned int x0, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11, x12, x13, x14, x15, t;

    x0 = X[0];   x1 = X[1];   x2 = X[2];   x3 = X[3];
    x4 = X[4];   x5 = X[5];   x6 = X[6];   x7 = X[7];
    x8 = X[8];   x9 = X[9];  x10 = X[10]; x11 = X[11];
   x12 = X[12]; x13 = X[13]; x14 = X[14]; x15 = X[15];

#define quarter(a,b,c,d) \
    a += b; t = d ^ a; d = ROTL32(t, 16); \
    c += d; t = b ^ c; b = ROTL32(t, 12); \
    a += b; t = d ^ a; d = ROTL32(t,  8); \
    c += d; t = b ^ c; b = ROTL32(t,  7);

    for(; rounds; rounds -= 2) {
        quarter( x0,  x4,  x8, x12);
        quarter( x1,  x5,  x9, x13);
        quarter( x2,  x6, x10, x14);
        quarter( x3,  x7, x11, x15);
        quarter( x0,  x5, x10, x15);
        quarter( x1,  x6, x11, x12);
        quarter( x2,  x7,  x8, x13);
        quarter( x3,  x4,  x9, x14);
    }

    X[0] += x0;   X[1] += x1;   X[2] += x2;   X[3] += x3;
    X[4] += x4;   X[5] += x5;   X[6] += x6;   X[7] += x7;
    X[8] += x8;   X[9] += x9;  X[10] += x10; X[11] += x11;
   X[12] += x12; X[13] += x13; X[14] += x14; X[15] += x15;

#undef quarter
}

/* Fast 32-bit / 64-bit memcpy();
 * len must be a multiple of 32 bytes */
static void neoscrypt_blkcpy(void *dstp, const void *srcp, unsigned int len) {
    size_t *dst = (size_t *) dstp;
    size_t *src = (size_t *) srcp;
    unsigned int i;

    for(i = 0; i < (len / sizeof(size_t)); i += 4) {
        dst[i]     = src[i];
        dst[i + 1] = src[i + 1];
        dst[i + 2] = src[i + 2];
        dst[i + 3] = src[i + 3];
    }
}

/* Fast 32-bit / 64-bit block swapper;
 * len must be a multiple of 32 bytes */
static void neoscrypt_blkswp(void *blkAp, void *blkBp, unsigned int len) {
    size_t *blkA = (size_t *) blkAp;
    size_t *blkB = (size_t *) blkBp;
    register size_t t0, t1, t2, t3;
    unsigned int i;

    for(i = 0; i < (len / sizeof(size_t)); i += 4) {
        t0          = blkA[i];
        t1          = blkA[i + 1];
        t2          = blkA[i + 2];
        t3          = blkA[i + 3];
        blkA[i]     = blkB[i];
        blkA[i + 1] = blkB[i + 1];
        blkA[i + 2] = blkB[i + 2];
        blkA[i + 3] = blkB[i + 3];
        blkB[i]     = t0;
        blkB[i + 1] = t1;
        blkB[i + 2] = t2;
        blkB[i + 3] = t3;
    }
}

/* Fast 32-bit / 64-bit block XOR engine;
 * len must be a multiple of 32 bytes */
static void neoscrypt_blkxor(void *dstp, const void *srcp, unsigned int len) {
    size_t *dst = (size_t *) dstp;
    size_t *src = (size_t *) srcp;
    unsigned int i;

    for(i = 0; i < (len / sizeof(size_t)); i += 4) {
        dst[i]     ^= src[i];
        dst[i + 1] ^= src[i + 1];
        dst[i + 2] ^= src[i + 2];
        dst[i + 3] ^= src[i + 3];
    }
}

/* 32-bit / 64-bit optimised memcpy() */
void neoscrypt_copy(void *dstp, const void *srcp, unsigned int len) {
    size_t *dst = (size_t *) dstp;
    size_t *src = (size_t *) srcp;
    unsigned int i, tail;

    for(i = 0; i < (len / sizeof(size_t)); i++)
      dst[i] = src[i];

    tail = len & (sizeof(size_t) - 1);
    if(tail) {
        unsigned char *dstb = (unsigned char *) dstp;
        unsigned char *srcb = (unsigned char *) srcp;

        for(i = len - tail; i < len; i++)
          dstb[i] = srcb[i];
    }
}

/* 32-bit / 64-bit optimised memory erase aka memset() to zero */
void neoscrypt_erase(void *dstp, unsigned int len) {
    const size_t null = 0;
    size_t *dst = (size_t *) dstp;
    unsigned int i, tail;

    for(i = 0; i < (len / sizeof(size_t)); i++)
      dst[i] = null;

    tail = len & (sizeof(size_t) - 1);
    if(tail) {
        unsigned char *dstb = (unsigned char *) dstp;

        for(i = len - tail; i < len; i++)
          dstb[i] = (unsigned char)null;
    }
}

/* 32-bit / 64-bit optimised XOR engine */
void neoscrypt_xor(void *dstp, const void *srcp, unsigned int len) {
    size_t *dst = (size_t *) dstp;
    size_t *src = (size_t *) srcp;
    unsigned int i, tail;

    for(i = 0; i < (len / sizeof(size_t)); i++)
      dst[i] ^= src[i];

    tail = len & (sizeof(size_t) - 1);
    if(tail) {
        unsigned char *dstb = (unsigned char *) dstp;
        unsigned char *srcb = (unsigned char *) srcp;

        for(i = len - tail; i < len; i++)
          dstb[i] ^= srcb[i];
    }
}

#endif /* USE_ASM */


/* BLAKE2s */

/* Parameter block of 32 bytes */
typedef struct blake2s_param_t {
    unsigned char digest_length;
    unsigned char key_length;
    unsigned char fanout;
    unsigned char depth;
    unsigned int  leaf_length;
    unsigned char node_offset[6];
    unsigned char node_depth;
    unsigned char inner_length;
    unsigned char salt[8];
    unsigned char personal[8];
} blake2s_param;

/* State block of 256 bytes */
typedef struct blake2s_state_t {
    unsigned int  h[8];
    unsigned int  t[2];
    unsigned int  f[2];
    unsigned char buf[2 * BLOCK_SIZE];
    unsigned int  buflen;
    unsigned int  padding[3];
    unsigned char tempbuf[BLOCK_SIZE];
} blake2s_state;

static const unsigned int blake2s_IV[8] = {
    0x6A09E667, 0xBB67AE85, 0x3C6EF372, 0xA54FF53A,
    0x510E527F, 0x9B05688C, 0x1F83D9AB, 0x5BE0CD19
};

#ifdef USE_ASM

extern void blake2s_compress(blake2s_state *S);

#else

/* Buffer mixer (compressor) */
static void blake2s_compress(blake2s_state *S) {
    unsigned int *v = (unsigned int *) S->tempbuf;
    unsigned int *m = (unsigned int *) S->buf;
    register unsigned int t0, t1, t2, t3;

    v[0]  = S->h[0];
    v[1]  = S->h[1];
    v[2]  = S->h[2];
    v[3]  = S->h[3];
    v[4]  = S->h[4];
    v[5]  = S->h[5];
    v[6]  = S->h[6];
    v[7]  = S->h[7];
    v[8]  = blake2s_IV[0];
    v[9]  = blake2s_IV[1];
    v[10] = blake2s_IV[2];
    v[11] = blake2s_IV[3];
    v[12] = S->t[0] ^ blake2s_IV[4];
    v[13] = S->t[1] ^ blake2s_IV[5];
    v[14] = S->f[0] ^ blake2s_IV[6];
    v[15] = S->f[1] ^ blake2s_IV[7];

/* Round 0 */
    t0 = v[0];
    t1 = v[4];
    t0 = t0 + t1 + m[0];
    t3 = v[12];
    t2 = v[8];
    t3 = ROTR32(t3 ^ t0, 16);
    t2 = t2 + t3;
    t1 = ROTR32(t1 ^ t2, 12);
    t0 = t0 + t1 + m[1];
    v[0] = t0;
    t3 = ROTR32(t3 ^ t0, 8);
    v[12] = t3;
    t2 = t2 + t3;
    v[8] = t2;
    t1 = ROTR32(t1 ^ t2, 7);
    v[4] = t1;

    t0 = v[1];
    t1 = v[5];
    t0 = t0 + t1 + m[2];
    t3 = v[13];
    t2 = v[9];
    t3 = ROTR32(t3 ^ t0, 16);
    t2 = t2 + t3;
    t1 = ROTR32(t1 ^ t2, 12);
    t0 = t0 + t1 + m[3];
    v[1] = t0;
    t3 = ROTR32(t3 ^ t0, 8);
    v[13] = t3;
    t2 = t2 + t3;
    v[9] = t2;
    t1 = ROTR32(t1 ^ t2, 7);
    v[5] = t1;

    t0 = v[2];
    t1 = v[6];
    t0 = t0 + t1 + m[4];
    t3 = v[14];
    t2 = v[10];
    t3 = ROTR32(t3 ^ t0, 16);
    t2 = t2 + t3;
    t1 = ROTR32(t1 ^ t2, 12);
    t0 = t0 + t1 + m[5];
    v[2] = t0;
    t3 = ROTR32(t3 ^ t0, 8);
    v[14] = t3;
    t2 = t2 + t3;
    v[10] = t2;
    t1 = ROTR32(t1 ^ t2, 7);
    v[6] = t1;

    t0 = v[3];
    t1 = v[7];
    t0 = t0 + t1 + m[6];
    t3 = v[15];
    t2 = v[11];
    t3 = ROTR32(t3 ^ t0, 16);
    t2 = t2 + t3;
    t1 = ROTR32(t1 ^ t2, 12);
    t0 = t0 + t1 + m[7];
    v[3] = t0;
    t3 = ROTR32(t3 ^ t0, 8);
    v[15] = t3;
    t2 = t2 + t3;
    v[11] = t2;
    t1 = ROTR32(t1 ^ t2, 7);
    v[7] = t1;

    t0 = v[0];
    t1 = v[5];
    t0 = t0 + t1 + m[8];
    t3 = v[15];
    t2 = v[10];
    t3 = ROTR32(t3 ^ t0, 16);
    t2 = t2 + t3;
    t1 = ROTR32(t1 ^ t2, 12);
    t0 = t0 + t1 + m[9];
    v[0] = t0;
    t3 = ROTR32(t3 ^ t0, 8);
    v[15] = t3;
    t2 = t2 + t3;
    v[10] = t2;
    t1 = ROTR32(t1 ^ t2, 7);
    v[5] = t1;

    t0 = v[1];
    t1 = v[6];
    t0 = t0 + t1 + m[10];
    t3 = v[12];
    t2 = v[11];
    t3 = ROTR32(t3 ^ t0, 16);
    t2 = t2 + t3;
    t1 = ROTR32(t1 ^ t2, 12);
    t0 = t0 + t1 + m[11];
    v[1] = t0;
    t3 = ROTR32(t3 ^ t0, 8);
    v[12] = t3;
    t2 = t2 + t3;
    v[11] = t2;
    t1 = ROTR32(t1 ^ t2, 7);
    v[6] = t1;

    t0 = v[2];
    t1 = v[7];
    t0 = t0 + t1 + m[12];
    t3 = v[13];
    t2 = v[8];
    t3 = ROTR32(t3 ^ t0, 16);
    t2 = t2 + t3;
    t1 = ROTR32(t1 ^ t2, 12);
    t0 = t0 + t1 + m[13];
    v[2] = t0;
    t3 = ROTR32(t3 ^ t0, 8);
    v[13] = t3;
    t2 = t2 + t3;
    v[8] = t2;
    t1 = ROTR32(t1 ^ t2, 7);
    v[7] = t1;

    t0 = v[3];
    t1 = v[4];
    t0 = t0 + t1 + m[14];
    t3 = v[14];
    t2 = v[9];
    t3 = ROTR32(t3 ^ t0, 16);
    t2 = t2 + t3;
    t1 = ROTR32(t1 ^ t2, 12);
    t0 = t0 + t1 + m[15];
    v[3] = t0;
    t3 = ROTR32(t3 ^ t0, 8);
    v[14] = t3;
    t2 = t2 + t3;
    v[9] = t2;
    t1 = ROTR32(t1 ^ t2, 7);
    v[4] = t1;

/* Round 1 */
    t0 = v[0];
    t1 = v[4];
    t0 = t0 + t1 + m[14];
    t3 = v[12];
    t2 = v[8];
    t3 = ROTR32(t3 ^ t0, 16);
    t2 = t2 + t3;
    t1 = ROTR32(t1 ^ t2, 12);
    t0 = t0 + t1 + m[10];
    v[0] = t0;
    t3 = ROTR32(t3 ^ t0, 8);
    v[12] = t3;
    t2 = t2 + t3;
    v[8] = t2;
    t1 = ROTR32(t1 ^ t2, 7);
    v[4] = t1;

    t0 = v[1];
    t1 = v[5];
    t0 = t0 + t1 + m[4];
    t3 = v[13];
    t2 = v[9];
    t3 = ROTR32(t3 ^ t0, 16);
    t2 = t2 + t3;
    t1 = ROTR32(t1 ^ t2, 12);
    t0 = t0 + t1 + m[8];
    v[1] = t0;
    t3 = ROTR32(t3 ^ t0, 8);
    v[13] = t3;
    t2 = t2 + t3;
    v[9] = t2;
    t1 = ROTR32(t1 ^ t2, 7);
    v[5] = t1;

    t0 = v[2];
    t1 = v[6];
    t0 = t0 + t1 + m[9];
    t3 = v[14];
    t2 = v[10];
    t3 = ROTR32(t3 ^ t0, 16);
    t2 = t2 + t3;
    t1 = ROTR32(t1 ^ t2, 12);
    t0 = t0 + t1 + m[15];
    v[2] = t0;
    t3 = ROTR32(t3 ^ t0, 8);
    v[14] = t3;
    t2 = t2 + t3;
    v[10] = t2;
    t1 = ROTR32(t1 ^ t2, 7);
    v[6] = t1;

    t0 = v[3];
    t1 = v[7];
    t0 = t0 + t1 + m[13];
    t3 = v[15];
    t2 = v[11];
    t3 = ROTR32(t3 ^ t0, 16);
    t2 = t2 + t3;
    t1 = ROTR32(t1 ^ t2, 12);
    t0 = t0 + t1 + m[6];
    v[3] = t0;
    t3 = ROTR32(t3 ^ t0, 8);
    v[15] = t3;
    t2 = t2 + t3;
    v[11] = t2;
    t1 = ROTR32(t1 ^ t2, 7);
    v[7] = t1;

    t0 = v[0];
    t1 = v[5];
    t0 = t0 + t1 + m[1];
    t3 = v[15];
    t2 = v[10];
    t3 = ROTR32(t3 ^ t0, 16);
    t2 = t2 + t3;
    t1 = ROTR32(t1 ^ t2, 12);
    t0 = t0 + t1 + m[12];
    v[0] = t0;
    t3 = ROTR32(t3 ^ t0, 8);
    v[15] = t3;
    t2 = t2 + t3;
    v[10] = t2;
    t1 = ROTR32(t1 ^ t2, 7);
    v[5] = t1;

    t0 = v[1];
    t1 = v[6];
    t0 = t0 + t1 + m[0];
    t3 = v[12];
    t2 = v[11];
    t3 = ROTR32(t3 ^ t0, 16);
    t2 = t2 + t3;
    t1 = ROTR32(t1 ^ t2, 12);
    t0 = t0 + t1 + m[2];
    v[1] = t0;
    t3 = ROTR32(t3 ^ t0, 8);
    v[12] = t3;
    t2 = t2 + t3;
    v[11] = t2;
    t1 = ROTR32(t1 ^ t2, 7);
    v[6] = t1;

    t0 = v[2];
    t1 = v[7];
    t0 = t0 + t1 + m[11];
    t3 = v[13];
    t2 = v[8];
    t3 = ROTR32(t3 ^ t0, 16);
    t2 = t2 + t3;
    t1 = ROTR32(t1 ^ t2, 12);
    t0 = t0 + t1 + m[7];
    v[2] = t0;
    t3 = ROTR32(t3 ^ t0, 8);
    v[13] = t3;
    t2 = t2 + t3;
    v[8] = t2;
    t1 = ROTR32(t1 ^ t2, 7);
    v[7] = t1;

    t0 = v[3];
    t1 = v[4];
    t0 = t0 + t1 + m[5];
    t3 = v[14];
    t2 = v[9];
    t3 = ROTR32(t3 ^ t0, 16);
    t2 = t2 + t3;
    t1 = ROTR32(t1 ^ t2, 12);
    t0 = t0 + t1 + m[3];
    v[3] = t0;
    t3 = ROTR32(t3 ^ t0, 8);
    v[14] = t3;
    t2 = t2 + t3;
    v[9] = t2;
    t1 = ROTR32(t1 ^ t2, 7);
    v[4] = t1;

/* Round 2 */
    t0 = v[0];
    t1 = v[4];
    t0 = t0 + t1 + m[11];
    t3 = v[12];
    t2 = v[8];
    t3 = ROTR32(t3 ^ t0, 16);
    t2 = t2 + t3;
    t1 = ROTR32(t1 ^ t2, 12);
    t0 = t0 + t1 + m[8];
    v[0] = t0;
    t3 = ROTR32(t3 ^ t0, 8);
    v[12] = t3;
    t2 = t2 + t3;
    v[8] = t2;
    t1 = ROTR32(t1 ^ t2, 7);
    v[4] = t1;

    t0 = v[1];
    t1 = v[5];
    t0 = t0 + t1 + m[12];
    t3 = v[13];
    t2 = v[9];
    t3 = ROTR32(t3 ^ t0, 16);
    t2 = t2 + t3;
    t1 = ROTR32(t1 ^ t2, 12);
    t0 = t0 + t1 + m[0];
    v[1] = t0;
    t3 = ROTR32(t3 ^ t0, 8);
    v[13] = t3;
    t2 = t2 + t3;
    v[9] = t2;
    t1 = ROTR32(t1 ^ t2, 7);
    v[5] = t1;

    t0 = v[2];
    t1 = v[6];
    t0 = t0 + t1 + m[5];
    t3 = v[14];
    t2 = v[10];
    t3 = ROTR32(t3 ^ t0, 16);
    t2 = t2 + t3;
    t1 = ROTR32(t1 ^ t2, 12);
    t0 = t0 + t1 + m[2];
    v[2] = t0;
    t3 = ROTR32(t3 ^ t0, 8);
    v[14] = t3;
    t2 = t2 + t3;
    v[10] = t2;
    t1 = ROTR32(t1 ^ t2, 7);
    v[6] = t1;

    t0 = v[3];
    t1 = v[7];
    t0 = t0 + t1 + m[15];
    t3 = v[15];
    t2 = v[11];
    t3 = ROTR32(t3 ^ t0, 16);
    t2 = t2 + t3;
    t1 = ROTR32(t1 ^ t2, 12);
    t0 = t0 + t1 + m[13];
    v[3] = t0;
    t3 = ROTR32(t3 ^ t0, 8);
    v[15] = t3;
    t2 = t2 + t3;
    v[11] = t2;
    t1 = ROTR32(t1 ^ t2, 7);
    v[7] = t1;

    t0 = v[0];
    t1 = v[5];
    t0 = t0 + t1 + m[10];
    t3 = v[15];
    t2 = v[10];
    t3 = ROTR32(t3 ^ t0, 16);
    t2 = t2 + t3;
    t1 = ROTR32(t1 ^ t2, 12);
    t0 = t0 + t1 + m[14];
    v[0] = t0;
    t3 = ROTR32(t3 ^ t0, 8);
    v[15] = t3;
    t2 = t2 + t3;
    v[10] = t2;
    t1 = ROTR32(t1 ^ t2, 7);
    v[5] = t1;

    t0 = v[1];
    t1 = v[6];
    t0 = t0 + t1 + m[3];
    t3 = v[12];
    t2 = v[11];
    t3 = ROTR32(t3 ^ t0, 16);
    t2 = t2 + t3;
    t1 = ROTR32(t1 ^ t2, 12);
    t0 = t0 + t1 + m[6];
    v[1] = t0;
    t3 = ROTR32(t3 ^ t0, 8);
    v[12] = t3;
    t2 = t2 + t3;
    v[11] = t2;
    t1 = ROTR32(t1 ^ t2, 7);
    v[6] = t1;

    t0 = v[2];
    t1 = v[7];
    t0 = t0 + t1 + m[7];
    t3 = v[13];
    t2 = v[8];
    t3 = ROTR32(t3 ^ t0, 16);
    t2 = t2 + t3;
    t1 = ROTR32(t1 ^ t2, 12);
    t0 = t0 + t1 + m[1];
    v[2] = t0;
    t3 = ROTR32(t3 ^ t0, 8);
    v[13] = t3;
    t2 = t2 + t3;
    v[8] = t2;
    t1 = ROTR32(t1 ^ t2, 7);
    v[7] = t1;

    t0 = v[3];
    t1 = v[4];
    t0 = t0 + t1 + m[9];
    t3 = v[14];
    t2 = v[9];
    t3 = ROTR32(t3 ^ t0, 16);
    t2 = t2 + t3;
    t1 = ROTR32(t1 ^ t2, 12);
    t0 = t0 + t1 + m[4];
    v[3] = t0;
    t3 = ROTR32(t3 ^ t0, 8);
    v[14] = t3;
    t2 = t2 + t3;
    v[9] = t2;
    t1 = ROTR32(t1 ^ t2, 7);
    v[4] = t1;

/* Round 3 */
    t0 = v[0];
    t1 = v[4];
    t0 = t0 + t1 + m[7];
    t3 = v[12];
    t2 = v[8];
    t3 = ROTR32(t3 ^ t0, 16);
    t2 = t2 + t3;
    t1 = ROTR32(t1 ^ t2, 12);
    t0 = t0 + t1 + m[9];
    v[0] = t0;
    t3 = ROTR32(t3 ^ t0, 8);
    v[12] = t3;
    t2 = t2 + t3;
    v[8] = t2;
    t1 = ROTR32(t1 ^ t2, 7);
    v[4] = t1;

    t0 = v[1];
    t1 = v[5];
    t0 = t0 + t1 + m[3];
    t3 = v[13];
    t2 = v[9];
    t3 = ROTR32(t3 ^ t0, 16);
    t2 = t2 + t3;
    t1 = ROTR32(t1 ^ t2, 12);
    t0 = t0 + t1 + m[1];
    v[1] = t0;
    t3 = ROTR32(t3 ^ t0, 8);
    v[13] = t3;
    t2 = t2 + t3;
    v[9] = t2;
    t1 = ROTR32(t1 ^ t2, 7);
    v[5] = t1;

    t0 = v[2];
    t1 = v[6];
    t0 = t0 + t1 + m[13];
    t3 = v[14];
    t2 = v[10];
    t3 = ROTR32(t3 ^ t0, 16);
    t2 = t2 + t3;
    t1 = ROTR32(t1 ^ t2, 12);
    t0 = t0 + t1 + m[12];
    v[2] = t0;
    t3 = ROTR32(t3 ^ t0, 8);
    v[14] = t3;
    t2 = t2 + t3;
    v[10] = t2;
    t1 = ROTR32(t1 ^ t2, 7);
    v[6] = t1;

    t0 = v[3];
    t1 = v[7];
    t0 = t0 + t1 + m[11];
    t3 = v[15];
    t2 = v[11];
    t3 = ROTR32(t3 ^ t0, 16);
    t2 = t2 + t3;
    t1 = ROTR32(t1 ^ t2, 12);
    t0 = t0 + t1 + m[14];
    v[3] = t0;
    t3 = ROTR32(t3 ^ t0, 8);
    v[15] = t3;
    t2 = t2 + t3;
    v[11] = t2;
    t1 = ROTR32(t1 ^ t2, 7);
    v[7] = t1;

    t0 = v[0];
    t1 = v[5];
    t0 = t0 + t1 + m[2];
    t3 = v[15];
    t2 = v[10];
    t3 = ROTR32(t3 ^ t0, 16);
    t2 = t2 + t3;
    t1 = ROTR32(t1 ^ t2, 12);
    t0 = t0 + t1 + m[6];
    v[0] = t0;
    t3 = ROTR32(t3 ^ t0, 8);
    v[15] = t3;
    t2 = t2 + t3;
    v[10] = t2;
    t1 = ROTR32(t1 ^ t2, 7);
    v[5] = t1;

    t0 = v[1];
    t1 = v[6];
    t0 = t0 + t1 + m[5];
    t3 = v[12];
    t2 = v[11];
    t3 = ROTR32(t3 ^ t0, 16);
    t2 = t2 + t3;
    t1 = ROTR32(t1 ^ t2, 12);
    t0 = t0 + t1 + m[10];
    v[1] = t0;
    t3 = ROTR32(t3 ^ t0, 8);
    v[12] = t3;
    t2 = t2 + t3;
    v[11] = t2;
    t1 = ROTR32(t1 ^ t2, 7);
    v[6] = t1;

    t0 = v[2];
    t1 = v[7];
    t0 = t0 + t1 + m[4];
    t3 = v[13];
    t2 = v[8];
    t3 = ROTR32(t3 ^ t0, 16);
    t2 = t2 + t3;
    t1 = ROTR32(t1 ^ t2, 12);
    t0 = t0 + t1 + m[0];
    v[2] = t0;
    t3 = ROTR32(t3 ^ t0, 8);
    v[13] = t3;
    t2 = t2 + t3;
    v[8] = t2;
    t1 = ROTR32(t1 ^ t2, 7);
    v[7] = t1;

    t0 = v[3];
    t1 = v[4];
    t0 = t0 + t1 + m[15];
    t3 = v[14];
    t2 = v[9];
    t3 = ROTR32(t3 ^ t0, 16);
    t2 = t2 + t3;
    t1 = ROTR32(t1 ^ t2, 12);
    t0 = t0 + t1 + m[8];
    v[3] = t0;
    t3 = ROTR32(t3 ^ t0, 8);
    v[14] = t3;
    t2 = t2 + t3;
    v[9] = t2;
    t1 = ROTR32(t1 ^ t2, 7);
    v[4] = t1;

/* Round 4 */
    t0 = v[0];
    t1 = v[4];
    t0 = t0 + t1 + m[9];
    t3 = v[12];
    t2 = v[8];
    t3 = ROTR32(t3 ^ t0, 16);
    t2 = t2 + t3;
    t1 = ROTR32(t1 ^ t2, 12);
    t0 = t0 + t1 + m[0];
    v[0] = t0;
    t3 = ROTR32(t3 ^ t0, 8);
    v[12] = t3;
    t2 = t2 + t3;
    v[8] = t2;
    t1 = ROTR32(t1 ^ t2, 7);
    v[4] = t1;

    t0 = v[1];
    t1 = v[5];
    t0 = t0 + t1 + m[5];
    t3 = v[13];
    t2 = v[9];
    t3 = ROTR32(t3 ^ t0, 16);
    t2 = t2 + t3;
    t1 = ROTR32(t1 ^ t2, 12);
    t0 = t0 + t1 + m[7];
    v[1] = t0;
    t3 = ROTR32(t3 ^ t0, 8);
    v[13] = t3;
    t2 = t2 + t3;
    v[9] = t2;
    t1 = ROTR32(t1 ^ t2, 7);
    v[5] = t1;

    t0 = v[2];
    t1 = v[6];
    t0 = t0 + t1 + m[2];
    t3 = v[14];
    t2 = v[10];
    t3 = ROTR32(t3 ^ t0, 16);
    t2 = t2 + t3;
    t1 = ROTR32(t1 ^ t2, 12);
    t0 = t0 + t1 + m[4];
    v[2] = t0;
    t3 = ROTR32(t3 ^ t0, 8);
    v[14] = t3;
    t2 = t2 + t3;
    v[10] = t2;
    t1 = ROTR32(t1 ^ t2, 7);
    v[6] = t1;

    t0 = v[3];
    t1 = v[7];
    t0 = t0 + t1 + m[10];
    t3 = v[15];
    t2 = v[11];
    t3 = ROTR32(t3 ^ t0, 16);
    t2 = t2 + t3;
    t1 = ROTR32(t1 ^ t2, 12);
    t0 = t0 + t1 + m[15];
    v[3] = t0;
    t3 = ROTR32(t3 ^ t0, 8);
    v[15] = t3;
    t2 = t2 + t3;
    v[11] = t2;
    t1 = ROTR32(t1 ^ t2, 7);
    v[7] = t1;

    t0 = v[0];
    t1 = v[5];
    t0 = t0 + t1 + m[14];
    t3 = v[15];
    t2 = v[10];
    t3 = ROTR32(t3 ^ t0, 16);
    t2 = t2 + t3;
    t1 = ROTR32(t1 ^ t2, 12);
    t0 = t0 + t1 + m[1];
    v[0] = t0;
    t3 = ROTR32(t3 ^ t0, 8);
    v[15] = t3;
    t2 = t2 + t3;
    v[10] = t2;
    t1 = ROTR32(t1 ^ t2, 7);
    v[5] = t1;

    t0 = v[1];
    t1 = v[6];
    t0 = t0 + t1 + m[11];
    t3 = v[12];
    t2 = v[11];
    t3 = ROTR32(t3 ^ t0, 16);
    t2 = t2 + t3;
    t1 = ROTR32(t1 ^ t2, 12);
    t0 = t0 + t1 + m[12];
    v[1] = t0;
    t3 = ROTR32(t3 ^ t0, 8);
    v[12] = t3;
    t2 = t2 + t3;
    v[11] = t2;
    t1 = ROTR32(t1 ^ t2, 7);
    v[6] = t1;

    t0 = v[2];
    t1 = v[7];
    t0 = t0 + t1 + m[6];
    t3 = v[13];
    t2 = v[8];
    t3 = ROTR32(t3 ^ t0, 16);
    t2 = t2 + t3;
    t1 = ROTR32(t1 ^ t2, 12);
    t0 = t0 + t1 + m[8];
    v[2] = t0;
    t3 = ROTR32(t3 ^ t0, 8);
    v[13] = t3;
    t2 = t2 + t3;
    v[8] = t2;
    t1 = ROTR32(t1 ^ t2, 7);
    v[7] = t1;

    t0 = v[3];
    t1 = v[4];
    t0 = t0 + t1 + m[3];
    t3 = v[14];
    t2 = v[9];
    t3 = ROTR32(t3 ^ t0, 16);
    t2 = t2 + t3;
    t1 = ROTR32(t1 ^ t2, 12);
    t0 = t0 + t1 + m[13];
    v[3] = t0;
    t3 = ROTR32(t3 ^ t0, 8);
    v[14] = t3;
    t2 = t2 + t3;
    v[9] = t2;
    t1 = ROTR32(t1 ^ t2, 7);
    v[4] = t1;

/* Round 5 */
    t0 = v[0];
    t1 = v[4];
    t0 = t0 + t1 + m[2];
    t3 = v[12];
    t2 = v[8];
    t3 = ROTR32(t3 ^ t0, 16);
    t2 = t2 + t3;
    t1 = ROTR32(t1 ^ t2, 12);
    t0 = t0 + t1 + m[12];
    v[0] = t0;
    t3 = ROTR32(t3 ^ t0, 8);
    v[12] = t3;
    t2 = t2 + t3;
    v[8] = t2;
    t1 = ROTR32(t1 ^ t2, 7);
    v[4] = t1;

    t0 = v[1];
    t1 = v[5];
    t0 = t0 + t1 + m[6];
    t3 = v[13];
    t2 = v[9];
    t3 = ROTR32(t3 ^ t0, 16);
    t2 = t2 + t3;
    t1 = ROTR32(t1 ^ t2, 12);
    t0 = t0 + t1 + m[10];
    v[1] = t0;
    t3 = ROTR32(t3 ^ t0, 8);
    v[13] = t3;
    t2 = t2 + t3;
    v[9] = t2;
    t1 = ROTR32(t1 ^ t2, 7);
    v[5] = t1;

    t0 = v[2];
    t1 = v[6];
    t0 = t0 + t1 + m[0];
    t3 = v[14];
    t2 = v[10];
    t3 = ROTR32(t3 ^ t0, 16);
    t2 = t2 + t3;
    t1 = ROTR32(t1 ^ t2, 12);
    t0 = t0 + t1 + m[11];
    v[2] = t0;
    t3 = ROTR32(t3 ^ t0, 8);
    v[14] = t3;
    t2 = t2 + t3;
    v[10] = t2;
    t1 = ROTR32(t1 ^ t2, 7);
    v[6] = t1;

    t0 = v[3];
    t1 = v[7];
    t0 = t0 + t1 + m[8];
    t3 = v[15];
    t2 = v[11];
    t3 = ROTR32(t3 ^ t0, 16);
    t2 = t2 + t3;
    t1 = ROTR32(t1 ^ t2, 12);
    t0 = t0 + t1 + m[3];
    v[3] = t0;
    t3 = ROTR32(t3 ^ t0, 8);
    v[15] = t3;
    t2 = t2 + t3;
    v[11] = t2;
    t1 = ROTR32(t1 ^ t2, 7);
    v[7] = t1;

    t0 = v[0];
    t1 = v[5];
    t0 = t0 + t1 + m[4];
    t3 = v[15];
    t2 = v[10];
    t3 = ROTR32(t3 ^ t0, 16);
    t2 = t2 + t3;
    t1 = ROTR32(t1 ^ t2, 12);
    t0 = t0 + t1 + m[13];
    v[0] = t0;
    t3 = ROTR32(t3 ^ t0, 8);
    v[15] = t3;
    t2 = t2 + t3;
    v[10] = t2;
    t1 = ROTR32(t1 ^ t2, 7);
    v[5] = t1;

    t0 = v[1];
    t1 = v[6];
    t0 = t0 + t1 + m[7];
    t3 = v[12];
    t2 = v[11];
    t3 = ROTR32(t3 ^ t0, 16);
    t2 = t2 + t3;
    t1 = ROTR32(t1 ^ t2, 12);
    t0 = t0 + t1 + m[5];
    v[1] = t0;
    t3 = ROTR32(t3 ^ t0, 8);
    v[12] = t3;
    t2 = t2 + t3;
    v[11] = t2;
    t1 = ROTR32(t1 ^ t2, 7);
    v[6] = t1;

    t0 = v[2];
    t1 = v[7];
    t0 = t0 + t1 + m[15];
    t3 = v[13];
    t2 = v[8];
    t3 = ROTR32(t3 ^ t0, 16);
    t2 = t2 + t3;
    t1 = ROTR32(t1 ^ t2, 12);
    t0 = t0 + t1 + m[14];
    v[2] = t0;
    t3 = ROTR32(t3 ^ t0, 8);
    v[13] = t3;
    t2 = t2 + t3;
    v[8] = t2;
    t1 = ROTR32(t1 ^ t2, 7);
    v[7] = t1;

    t0 = v[3];
    t1 = v[4];
    t0 = t0 + t1 + m[1];
    t3 = v[14];
    t2 = v[9];
    t3 = ROTR32(t3 ^ t0, 16);
    t2 = t2 + t3;
    t1 = ROTR32(t1 ^ t2, 12);
    t0 = t0 + t1 + m[9];
    v[3] = t0;
    t3 = ROTR32(t3 ^ t0, 8);
    v[14] = t3;
    t2 = t2 + t3;
    v[9] = t2;
    t1 = ROTR32(t1 ^ t2, 7);
    v[4] = t1;

/* Round 6 */
    t0 = v[0];
    t1 = v[4];
    t0 = t0 + t1 + m[12];
    t3 = v[12];
    t2 = v[8];
    t3 = ROTR32(t3 ^ t0, 16);
    t2 = t2 + t3;
    t1 = ROTR32(t1 ^ t2, 12);
    t0 = t0 + t1 + m[5];
    v[0] = t0;
    t3 = ROTR32(t3 ^ t0, 8);
    v[12] = t3;
    t2 = t2 + t3;
    v[8] = t2;
    t1 = ROTR32(t1 ^ t2, 7);
    v[4] = t1;

    t0 = v[1];
    t1 = v[5];
    t0 = t0 + t1 + m[1];
    t3 = v[13];
    t2 = v[9];
    t3 = ROTR32(t3 ^ t0, 16);
    t2 = t2 + t3;
    t1 = ROTR32(t1 ^ t2, 12);
    t0 = t0 + t1 + m[15];
    v[1] = t0;
    t3 = ROTR32(t3 ^ t0, 8);
    v[13] = t3;
    t2 = t2 + t3;
    v[9] = t2;
    t1 = ROTR32(t1 ^ t2, 7);
    v[5] = t1;

    t0 = v[2];
    t1 = v[6];
    t0 = t0 + t1 + m[14];
    t3 = v[14];
    t2 = v[10];
    t3 = ROTR32(t3 ^ t0, 16);
    t2 = t2 + t3;
    t1 = ROTR32(t1 ^ t2, 12);
    t0 = t0 + t1 + m[13];
    v[2] = t0;
    t3 = ROTR32(t3 ^ t0, 8);
    v[14] = t3;
    t2 = t2 + t3;
    v[10] = t2;
    t1 = ROTR32(t1 ^ t2, 7);
    v[6] = t1;

    t0 = v[3];
    t1 = v[7];
    t0 = t0 + t1 + m[4];
    t3 = v[15];
    t2 = v[11];
    t3 = ROTR32(t3 ^ t0, 16);
    t2 = t2 + t3;
    t1 = ROTR32(t1 ^ t2, 12);
    t0 = t0 + t1 + m[10];
    v[3] = t0;
    t3 = ROTR32(t3 ^ t0, 8);
    v[15] = t3;
    t2 = t2 + t3;
    v[11] = t2;
    t1 = ROTR32(t1 ^ t2, 7);
    v[7] = t1;

    t0 = v[0];
    t1 = v[5];
    t0 = t0 + t1 + m[0];
    t3 = v[15];
    t2 = v[10];
    t3 = ROTR32(t3 ^ t0, 16);
    t2 = t2 + t3;
    t1 = ROTR32(t1 ^ t2, 12);
    t0 = t0 + t1 + m[7];
    v[0] = t0;
    t3 = ROTR32(t3 ^ t0, 8);
    v[15] = t3;
    t2 = t2 + t3;
    v[10] = t2;
    t1 = ROTR32(t1 ^ t2, 7);
    v[5] = t1;

    t0 = v[1];
    t1 = v[6];
    t0 = t0 + t1 + m[6];
    t3 = v[12];
    t2 = v[11];
    t3 = ROTR32(t3 ^ t0, 16);
    t2 = t2 + t3;
    t1 = ROTR32(t1 ^ t2, 12);
    t0 = t0 + t1 + m[3];
    v[1] = t0;
    t3 = ROTR32(t3 ^ t0, 8);
    v[12] = t3;
    t2 = t2 + t3;
    v[11] = t2;
    t1 = ROTR32(t1 ^ t2, 7);
    v[6] = t1;

    t0 = v[2];
    t1 = v[7];
    t0 = t0 + t1 + m[9];
    t3 = v[13];
    t2 = v[8];
    t3 = ROTR32(t3 ^ t0, 16);
    t2 = t2 + t3;
    t1 = ROTR32(t1 ^ t2, 12);
    t0 = t0 + t1 + m[2];
    v[2] = t0;
    t3 = ROTR32(t3 ^ t0, 8);
    v[13] = t3;
    t2 = t2 + t3;
    v[8] = t2;
    t1 = ROTR32(t1 ^ t2, 7);
    v[7] = t1;

    t0 = v[3];
    t1 = v[4];
    t0 = t0 + t1 + m[8];
    t3 = v[14];
    t2 = v[9];
    t3 = ROTR32(t3 ^ t0, 16);
    t2 = t2 + t3;
    t1 = ROTR32(t1 ^ t2, 12);
    t0 = t0 + t1 + m[11];
    v[3] = t0;
    t3 = ROTR32(t3 ^ t0, 8);
    v[14] = t3;
    t2 = t2 + t3;
    v[9] = t2;
    t1 = ROTR32(t1 ^ t2, 7);
    v[4] = t1;

/* Round 7 */
    t0 = v[0];
    t1 = v[4];
    t0 = t0 + t1 + m[13];
    t3 = v[12];
    t2 = v[8];
    t3 = ROTR32(t3 ^ t0, 16);
    t2 = t2 + t3;
    t1 = ROTR32(t1 ^ t2, 12);
    t0 = t0 + t1 + m[11];
    v[0] = t0;
    t3 = ROTR32(t3 ^ t0, 8);
    v[12] = t3;
    t2 = t2 + t3;
    v[8] = t2;
    t1 = ROTR32(t1 ^ t2, 7);
    v[4] = t1;

    t0 = v[1];
    t1 = v[5];
    t0 = t0 + t1 + m[7];
    t3 = v[13];
    t2 = v[9];
    t3 = ROTR32(t3 ^ t0, 16);
    t2 = t2 + t3;
    t1 = ROTR32(t1 ^ t2, 12);
    t0 = t0 + t1 + m[14];
    v[1] = t0;
    t3 = ROTR32(t3 ^ t0, 8);
    v[13] = t3;
    t2 = t2 + t3;
    v[9] = t2;
    t1 = ROTR32(t1 ^ t2, 7);
    v[5] = t1;

    t0 = v[2];
    t1 = v[6];
    t0 = t0 + t1 + m[12];
    t3 = v[14];
    t2 = v[10];
    t3 = ROTR32(t3 ^ t0, 16);
    t2 = t2 + t3;
    t1 = ROTR32(t1 ^ t2, 12);
    t0 = t0 + t1 + m[1];
    v[2] = t0;
    t3 = ROTR32(t3 ^ t0, 8);
    v[14] = t3;
    t2 = t2 + t3;
    v[10] = t2;
    t1 = ROTR32(t1 ^ t2, 7);
    v[6] = t1;

    t0 = v[3];
    t1 = v[7];
    t0 = t0 + t1 + m[3];
    t3 = v[15];
    t2 = v[11];
    t3 = ROTR32(t3 ^ t0, 16);
    t2 = t2 + t3;
    t1 = ROTR32(t1 ^ t2, 12);
    t0 = t0 + t1 + m[9];
    v[3] = t0;
    t3 = ROTR32(t3 ^ t0, 8);
    v[15] = t3;
    t2 = t2 + t3;
    v[11] = t2;
    t1 = ROTR32(t1 ^ t2, 7);
    v[7] = t1;

    t0 = v[0];
    t1 = v[5];
    t0 = t0 + t1 + m[5];
    t3 = v[15];
    t2 = v[10];
    t3 = ROTR32(t3 ^ t0, 16);
    t2 = t2 + t3;
    t1 = ROTR32(t1 ^ t2, 12);
    t0 = t0 + t1 + m[0];
    v[0] = t0;
    t3 = ROTR32(t3 ^ t0, 8);
    v[15] = t3;
    t2 = t2 + t3;
    v[10] = t2;
    t1 = ROTR32(t1 ^ t2, 7);
    v[5] = t1;

    t0 = v[1];
    t1 = v[6];
    t0 = t0 + t1 + m[15];
    t3 = v[12];
    t2 = v[11];
    t3 = ROTR32(t3 ^ t0, 16);
    t2 = t2 + t3;
    t1 = ROTR32(t1 ^ t2, 12);
    t0 = t0 + t1 + m[4];
    v[1] = t0;
    t3 = ROTR32(t3 ^ t0, 8);
    v[12] = t3;
    t2 = t2 + t3;
    v[11] = t2;
    t1 = ROTR32(t1 ^ t2, 7);
    v[6] = t1;

    t0 = v[2];
    t1 = v[7];
    t0 = t0 + t1 + m[8];
    t3 = v[13];
    t2 = v[8];
    t3 = ROTR32(t3 ^ t0, 16);
    t2 = t2 + t3;
    t1 = ROTR32(t1 ^ t2, 12);
    t0 = t0 + t1 + m[6];
    v[2] = t0;
    t3 = ROTR32(t3 ^ t0, 8);
    v[13] = t3;
    t2 = t2 + t3;
    v[8] = t2;
    t1 = ROTR32(t1 ^ t2, 7);
    v[7] = t1;

    t0 = v[3];
    t1 = v[4];
    t0 = t0 + t1 + m[2];
    t3 = v[14];
    t2 = v[9];
    t3 = ROTR32(t3 ^ t0, 16);
    t2 = t2 + t3;
    t1 = ROTR32(t1 ^ t2, 12);
    t0 = t0 + t1 + m[10];
    v[3] = t0;
    t3 = ROTR32(t3 ^ t0, 8);
    v[14] = t3;
    t2 = t2 + t3;
    v[9] = t2;
    t1 = ROTR32(t1 ^ t2, 7);
    v[4] = t1;

/* Round 8 */
    t0 = v[0];
    t1 = v[4];
    t0 = t0 + t1 + m[6];
    t3 = v[12];
    t2 = v[8];
    t3 = ROTR32(t3 ^ t0, 16);
    t2 = t2 + t3;
    t1 = ROTR32(t1 ^ t2, 12);
    t0 = t0 + t1 + m[15];
    v[0] = t0;
    t3 = ROTR32(t3 ^ t0, 8);
    v[12] = t3;
    t2 = t2 + t3;
    v[8] = t2;
    t1 = ROTR32(t1 ^ t2, 7);
    v[4] = t1;

    t0 = v[1];
    t1 = v[5];
    t0 = t0 + t1 + m[14];
    t3 = v[13];
    t2 = v[9];
    t3 = ROTR32(t3 ^ t0, 16);
    t2 = t2 + t3;
    t1 = ROTR32(t1 ^ t2, 12);
    t0 = t0 + t1 + m[9];
    v[1] = t0;
    t3 = ROTR32(t3 ^ t0, 8);
    v[13] = t3;
    t2 = t2 + t3;
    v[9] = t2;
    t1 = ROTR32(t1 ^ t2, 7);
    v[5] = t1;

    t0 = v[2];
    t1 = v[6];
    t0 = t0 + t1 + m[11];
    t3 = v[14];
    t2 = v[10];
    t3 = ROTR32(t3 ^ t0, 16);
    t2 = t2 + t3;
    t1 = ROTR32(t1 ^ t2, 12);
    t0 = t0 + t1 + m[3];
    v[2] = t0;
    t3 = ROTR32(t3 ^ t0, 8);
    v[14] = t3;
    t2 = t2 + t3;
    v[10] = t2;
    t1 = ROTR32(t1 ^ t2, 7);
    v[6] = t1;

    t0 = v[3];
    t1 = v[7];
    t0 = t0 + t1 + m[0];
    t3 = v[15];
    t2 = v[11];
    t3 = ROTR32(t3 ^ t0, 16);
    t2 = t2 + t3;
    t1 = ROTR32(t1 ^ t2, 12);
    t0 = t0 + t1 + m[8];
    v[3] = t0;
    t3 = ROTR32(t3 ^ t0, 8);
    v[15] = t3;
    t2 = t2 + t3;
    v[11] = t2;
    t1 = ROTR32(t1 ^ t2, 7);
    v[7] = t1;

    t0 = v[0];
    t1 = v[5];
    t0 = t0 + t1 + m[12];
    t3 = v[15];
    t2 = v[10];
    t3 = ROTR32(t3 ^ t0, 16);
    t2 = t2 + t3;
    t1 = ROTR32(t1 ^ t2, 12);
    t0 = t0 + t1 + m[2];
    v[0] = t0;
    t3 = ROTR32(t3 ^ t0, 8);
    v[15] = t3;
    t2 = t2 + t3;
    v[10] = t2;
    t1 = ROTR32(t1 ^ t2, 7);
    v[5] = t1;

    t0 = v[1];
    t1 = v[6];
    t0 = t0 + t1 + m[13];
    t3 = v[12];
    t2 = v[11];
    t3 = ROTR32(t3 ^ t0, 16);
    t2 = t2 + t3;
    t1 = ROTR32(t1 ^ t2, 12);
    t0 = t0 + t1 + m[7];
    v[1] = t0;
    t3 = ROTR32(t3 ^ t0, 8);
    v[12] = t3;
    t2 = t2 + t3;
    v[11] = t2;
    t1 = ROTR32(t1 ^ t2, 7);
    v[6] = t1;

    t0 = v[2];
    t1 = v[7];
    t0 = t0 + t1 + m[1];
    t3 = v[13];
    t2 = v[8];
    t3 = ROTR32(t3 ^ t0, 16);
    t2 = t2 + t3;
    t1 = ROTR32(t1 ^ t2, 12);
    t0 = t0 + t1 + m[4];
    v[2] = t0;
    t3 = ROTR32(t3 ^ t0, 8);
    v[13] = t3;
    t2 = t2 + t3;
    v[8] = t2;
    t1 = ROTR32(t1 ^ t2, 7);
    v[7] = t1;

    t0 = v[3];
    t1 = v[4];
    t0 = t0 + t1 + m[10];
    t3 = v[14];
    t2 = v[9];
    t3 = ROTR32(t3 ^ t0, 16);
    t2 = t2 + t3;
    t1 = ROTR32(t1 ^ t2, 12);
    t0 = t0 + t1 + m[5];
    v[3] = t0;
    t3 = ROTR32(t3 ^ t0, 8);
    v[14] = t3;
    t2 = t2 + t3;
    v[9] = t2;
    t1 = ROTR32(t1 ^ t2, 7);
    v[4] = t1;

/* Round 9 */
    t0 = v[0];
    t1 = v[4];
    t0 = t0 + t1 + m[10];
    t3 = v[12];
    t2 = v[8];
    t3 = ROTR32(t3 ^ t0, 16);
    t2 = t2 + t3;
    t1 = ROTR32(t1 ^ t2, 12);
    t0 = t0 + t1 + m[2];
    v[0] = t0;
    t3 = ROTR32(t3 ^ t0, 8);
    v[12] = t3;
    t2 = t2 + t3;
    v[8] = t2;
    t1 = ROTR32(t1 ^ t2, 7);
    v[4] = t1;

    t0 = v[1];
    t1 = v[5];
    t0 = t0 + t1 + m[8];
    t3 = v[13];
    t2 = v[9];
    t3 = ROTR32(t3 ^ t0, 16);
    t2 = t2 + t3;
    t1 = ROTR32(t1 ^ t2, 12);
    t0 = t0 + t1 + m[4];
    v[1] = t0;
    t3 = ROTR32(t3 ^ t0, 8);
    v[13] = t3;
    t2 = t2 + t3;
    v[9] = t2;
    t1 = ROTR32(t1 ^ t2, 7);
    v[5] = t1;

    t0 = v[2];
    t1 = v[6];
    t0 = t0 + t1 + m[7];
    t3 = v[14];
    t2 = v[10];
    t3 = ROTR32(t3 ^ t0, 16);
    t2 = t2 + t3;
    t1 = ROTR32(t1 ^ t2, 12);
    t0 = t0 + t1 + m[6];
    v[2] = t0;
    t3 = ROTR32(t3 ^ t0, 8);
    v[14] = t3;
    t2 = t2 + t3;
    v[10] = t2;
    t1 = ROTR32(t1 ^ t2, 7);
    v[6] = t1;

    t0 = v[3];
    t1 = v[7];
    t0 = t0 + t1 + m[1];
    t3 = v[15];
    t2 = v[11];
    t3 = ROTR32(t3 ^ t0, 16);
    t2 = t2 + t3;
    t1 = ROTR32(t1 ^ t2, 12);
    t0 = t0 + t1 + m[5];
    v[3] = t0;
    t3 = ROTR32(t3 ^ t0, 8);
    v[15] = t3;
    t2 = t2 + t3;
    v[11] = t2;
    t1 = ROTR32(t1 ^ t2, 7);
    v[7] = t1;

    t0 = v[0];
    t1 = v[5];
    t0 = t0 + t1 + m[15];
    t3 = v[15];
    t2 = v[10];
    t3 = ROTR32(t3 ^ t0, 16);
    t2 = t2 + t3;
    t1 = ROTR32(t1 ^ t2, 12);
    t0 = t0 + t1 + m[11];
    v[0] = t0;
    t3 = ROTR32(t3 ^ t0, 8);
    v[15] = t3;
    t2 = t2 + t3;
    v[10] = t2;
    t1 = ROTR32(t1 ^ t2, 7);
    v[5] = t1;

    t0 = v[1];
    t1 = v[6];
    t0 = t0 + t1 + m[9];
    t3 = v[12];
    t2 = v[11];
    t3 = ROTR32(t3 ^ t0, 16);
    t2 = t2 + t3;
    t1 = ROTR32(t1 ^ t2, 12);
    t0 = t0 + t1 + m[14];
    v[1] = t0;
    t3 = ROTR32(t3 ^ t0, 8);
    v[12] = t3;
    t2 = t2 + t3;
    v[11] = t2;
    t1 = ROTR32(t1 ^ t2, 7);
    v[6] = t1;

    t0 = v[2];
    t1 = v[7];
    t0 = t0 + t1 + m[3];
    t3 = v[13];
    t2 = v[8];
    t3 = ROTR32(t3 ^ t0, 16);
    t2 = t2 + t3;
    t1 = ROTR32(t1 ^ t2, 12);
    t0 = t0 + t1 + m[12];
    v[2] = t0;
    t3 = ROTR32(t3 ^ t0, 8);
    v[13] = t3;
    t2 = t2 + t3;
    v[8] = t2;
    t1 = ROTR32(t1 ^ t2, 7);
    v[7] = t1;

    t0 = v[3];
    t1 = v[4];
    t0 = t0 + t1 + m[13];
    t3 = v[14];
    t2 = v[9];
    t3 = ROTR32(t3 ^ t0, 16);
    t2 = t2 + t3;
    t1 = ROTR32(t1 ^ t2, 12);
    t0 = t0 + t1 + m[0];
    v[3] = t0;
    t3 = ROTR32(t3 ^ t0, 8);
    v[14] = t3;
    t2 = t2 + t3;
    v[9] = t2;
    t1 = ROTR32(t1 ^ t2, 7);
    v[4] = t1;

    S->h[0] ^= v[0] ^ v[8];
    S->h[1] ^= v[1] ^ v[9];
    S->h[2] ^= v[2] ^ v[10];
    S->h[3] ^= v[3] ^ v[11];
    S->h[4] ^= v[4] ^ v[12];
    S->h[5] ^= v[5] ^ v[13];
    S->h[6] ^= v[6] ^ v[14];
    S->h[7] ^= v[7] ^ v[15];
}

#endif /* USE_ASM */

static void blake2s_update(blake2s_state *S, const unsigned char *input,
  unsigned int input_size) {
    unsigned int left, fill;

    while(input_size > 0) {
        left = S->buflen;
        fill = 2 * BLOCK_SIZE - left;
        if(input_size > fill) {
            /* Buffer fill */
            neoscrypt_copy(S->buf + left, input, fill);
            S->buflen += fill;
            /* Counter increment */
            S->t[0] += BLOCK_SIZE;
            /* Compress */
            blake2s_compress(S);
            /* Shift buffer left */
            neoscrypt_copy(S->buf, S->buf + BLOCK_SIZE, BLOCK_SIZE);
            S->buflen -= BLOCK_SIZE;
            input += fill;
            input_size -= fill;
        } else {
            neoscrypt_copy(S->buf + left, input, input_size);
            S->buflen += input_size;
            /* Do not compress */
            input += input_size;
            input_size = 0;
        }
    }
}

void neoscrypt_blake2s(const void *input, const unsigned int input_size,
  const void *key, const unsigned char key_size, void *output, const unsigned char output_size) {
    unsigned char block[BLOCK_SIZE];
    blake2s_param P[1];
    blake2s_state S[1];

    /* Initialise */
    neoscrypt_erase(P, 32);
    P->digest_length = output_size;
    P->key_length    = key_size;
    P->fanout        = 1;
    P->depth         = 1;

    neoscrypt_erase(S, 256);
    neoscrypt_copy(S, blake2s_IV, 32);
    neoscrypt_xor(S, P, 32);

    neoscrypt_erase(block, BLOCK_SIZE);
    neoscrypt_copy(block, key, key_size);
    blake2s_update(S, (unsigned char *) block, BLOCK_SIZE);

    /* Update */
    blake2s_update(S, (unsigned char *) input, input_size);

    /* Finish */
    if(S->buflen > BLOCK_SIZE) {
        S->t[0] += BLOCK_SIZE;
        blake2s_compress(S);
        S->buflen -= BLOCK_SIZE;
        neoscrypt_copy(S->buf, S->buf + BLOCK_SIZE, S->buflen);
    }
    S->t[0] += S->buflen;
    S->f[0] = ~0U;
    neoscrypt_erase(S->buf + S->buflen, 2 * BLOCK_SIZE - S->buflen);
    blake2s_compress(S);

    /* Write back */
    neoscrypt_copy(output, S, output_size);
}

#ifndef OPT

#define FASTKDF_BUFFER_SIZE 256U

/* FastKDF, a fast buffered key derivation function:
 * FASTKDF_BUFFER_SIZE must be a power of 2;
 * password_len, salt_len and output_len should not exceed FASTKDF_BUFFER_SIZE;
 * prf_output_size must be <= prf_key_size; */
void neoscrypt_fastkdf(const unsigned char *password, unsigned int password_len,
  const unsigned char *salt, unsigned int salt_len, unsigned int N, unsigned char *output, unsigned int output_len) {
    const size_t stack_align = 0x40;
    const unsigned int kdf_buf_size = FASTKDF_BUFFER_SIZE,
      prf_input_size = 64, prf_key_size = 32, prf_output_size = 32;
    unsigned int bufptr, a, b, i, j;
    unsigned char *A, *B, *prf_input, *prf_key, *prf_output;

    /* Align and set up the buffers in stack */
    unsigned char stack[2 * kdf_buf_size + prf_input_size + prf_key_size
      + prf_output_size + stack_align];
    A          = (unsigned char *) (((size_t)stack & ~(stack_align - 1)) + stack_align);
    B          = &A[kdf_buf_size + prf_input_size];
    prf_output = &A[2 * kdf_buf_size + prf_input_size + prf_key_size];

    /* Initialise the password buffer */
    if(password_len > kdf_buf_size)
       password_len = kdf_buf_size;

    a = kdf_buf_size / password_len;
    for(i = 0; i < a; i++)
      neoscrypt_copy(&A[i * password_len], &password[0], password_len);
    b = kdf_buf_size - a * password_len;
    if(b)
      neoscrypt_copy(&A[a * password_len], &password[0], b);
    neoscrypt_copy(&A[kdf_buf_size], &password[0], prf_input_size);

    /* Initialise the salt buffer */
    if(salt_len > kdf_buf_size)
       salt_len = kdf_buf_size;

    a = kdf_buf_size / salt_len;
    for(i = 0; i < a; i++)
      neoscrypt_copy(&B[i * salt_len], &salt[0], salt_len);
    b = kdf_buf_size - a * salt_len;
    if(b)
      neoscrypt_copy(&B[a * salt_len], &salt[0], b);
    neoscrypt_copy(&B[kdf_buf_size], &salt[0], prf_key_size);

    /* The primary iteration */
    for(i = 0, bufptr = 0; i < N; i++) {

        /* Map the PRF input buffer */
        prf_input = &A[bufptr];

        /* Map the PRF key buffer */
        prf_key = &B[bufptr];

        /* PRF */
        neoscrypt_blake2s(prf_input, prf_input_size, prf_key, prf_key_size,
          prf_output, prf_output_size);

        /* Calculate the next buffer pointer */
        for(j = 0, bufptr = 0; j < prf_output_size; j++)
          bufptr += prf_output[j];
        bufptr &= (kdf_buf_size - 1);

        /* Modify the salt buffer */
        neoscrypt_xor(&B[bufptr], &prf_output[0], prf_output_size);

        /* Head modified, tail updated */
        if(bufptr < prf_key_size)
          neoscrypt_copy(&B[kdf_buf_size + bufptr], &B[bufptr],
            MIN(prf_output_size, prf_key_size - bufptr));
        /* Tail modified, head updated */
        else if((kdf_buf_size - bufptr) < prf_output_size)
          neoscrypt_copy(&B[0], &B[kdf_buf_size],
            prf_output_size - (kdf_buf_size - bufptr));

    }

    /* Modify and copy into the output buffer */
    if(output_len > kdf_buf_size)
      output_len = kdf_buf_size;

    a = kdf_buf_size - bufptr;
    if(a >= output_len) {
        neoscrypt_xor(&B[bufptr], &A[0], output_len);
        neoscrypt_copy(&output[0], &B[bufptr], output_len);
    } else {
        neoscrypt_xor(&B[bufptr], &A[0], a);
        neoscrypt_xor(&B[0], &A[a], output_len - a);
        neoscrypt_copy(&output[0], &B[bufptr], a);
        neoscrypt_copy(&output[a], &B[0], output_len - a);
    }

}

#else

#ifdef USE_ASM

extern void neoscrypt_fastkdf_opt(const unsigned char *password, const unsigned char *salt,
  unsigned char *output, unsigned int mode);

#else

/* Initialisation vector with a parameter block XOR'ed in */
static const unsigned int blake2s_IV_P_XOR[8] = {
    0x6B08C647, 0xBB67AE85, 0x3C6EF372, 0xA54FF53A,
    0x510E527F, 0x9B05688C, 0x1F83D9AB, 0x5BE0CD19
};

/* Performance optimised FastKDF with BLAKE2s integrated */
void neoscrypt_fastkdf_opt(const unsigned char *password, const unsigned char *salt,
  unsigned char *output, unsigned int mode) {
    const size_t stack_align = 0x40;
    unsigned int bufptr, output_len, i, j;
    unsigned char *A, *B;
    unsigned int *S;

    /* Align and set up the buffers in stack */
    unsigned char stack[864 + stack_align];
    A = (unsigned char *) (((size_t)stack & ~(stack_align - 1)) + stack_align);
    B = &A[320];
    S = (unsigned int *) &A[608];

    neoscrypt_copy(&A[0],   &password[0], 80);
    neoscrypt_copy(&A[80],  &password[0], 80);
    neoscrypt_copy(&A[160], &password[0], 80);
    neoscrypt_copy(&A[240], &password[0], 16);
    neoscrypt_copy(&A[256], &password[0], 64);

    if(!mode) {
        output_len = 256;
        neoscrypt_copy(&B[0],   &salt[0], 80);
        neoscrypt_copy(&B[80],  &salt[0], 80);
        neoscrypt_copy(&B[160], &salt[0], 80);
        neoscrypt_copy(&B[240], &salt[0], 16);
        neoscrypt_copy(&B[256], &salt[0], 32);
    } else {
        output_len = 32;
        neoscrypt_copy(&B[0],   &salt[0], 256);
        neoscrypt_copy(&B[256], &salt[0], 32);
    }

    for(i = 0, bufptr = 0; i < 32; i++) {

        /* BLAKE2s: initialise */
        neoscrypt_copy(&S[0], blake2s_IV_P_XOR, 32);
        neoscrypt_erase(&S[8], 16);

        /* BLAKE2s: update key */
        neoscrypt_copy(&S[12], &B[bufptr], 32);
        neoscrypt_erase(&S[20], 32);

        /* BLAKE2s: compress IV using key */
        S[8] = 64;
        blake2s_compress((blake2s_state *) S);

        /* BLAKE2s: update input */
        neoscrypt_copy(&S[12], &A[bufptr], 64);

        /* BLAKE2s: compress again using input */
        S[8] = 128;
        S[10] = ~0U;
        blake2s_compress((blake2s_state *) S);

        for(j = 0, bufptr = 0; j < 8; j++) {
          bufptr += S[j];
          bufptr += (S[j] >> 8);
          bufptr += (S[j] >> 16);
          bufptr += (S[j] >> 24);
        }
        bufptr &= 0xFF;

        neoscrypt_xor(&B[bufptr], &S[0], 32);

        if(bufptr < 32)
          neoscrypt_copy(&B[256 + bufptr], &B[bufptr], 32 - bufptr);
        else if(bufptr > 224)
          neoscrypt_copy(&B[0], &B[256], bufptr - 224);

    }

    i = 256 - bufptr;
    if(i >= output_len) {
        neoscrypt_xor(&B[bufptr], &A[0], output_len);
        neoscrypt_copy(&output[0], &B[bufptr], output_len);
    } else {
        neoscrypt_xor(&B[bufptr], &A[0], i);
        neoscrypt_xor(&B[0], &A[i], output_len - i);
        neoscrypt_copy(&output[0], &B[bufptr], i);
        neoscrypt_copy(&output[i], &B[0], output_len - i);
    }
}

#endif /* USE_ASM */

#endif /* !(OPT) */


#ifndef USE_ASM

/* Configurable optimised block mixer */
static void neoscrypt_blkmix(unsigned int *X, unsigned int *Y, unsigned int r, unsigned int mixmode) {
    unsigned int i, mixer, rounds;

    mixer  = mixmode >> 8;
    rounds = mixmode & 0xFF;

    /* NeoScrypt flow:                   Scrypt flow:
         Xa ^= Xd;  M(Xa'); Ya = Xa";      Xa ^= Xb;  M(Xa'); Ya = Xa";
         Xb ^= Xa"; M(Xb'); Yb = Xb";      Xb ^= Xa"; M(Xb'); Yb = Xb";
         Xc ^= Xb"; M(Xc'); Yc = Xc";      Xa" = Ya;
         Xd ^= Xc"; M(Xd'); Yd = Xd";      Xb" = Yb;
         Xa" = Ya; Xb" = Yc;
         Xc" = Yb; Xd" = Yd; */

    if(r == 1) {
        if(mixer) {
            neoscrypt_blkxor(&X[0], &X[16], BLOCK_SIZE);
            neoscrypt_chacha(&X[0], rounds);
            neoscrypt_blkxor(&X[16], &X[0], BLOCK_SIZE);
            neoscrypt_chacha(&X[16], rounds);
        } else {
            neoscrypt_blkxor(&X[0], &X[16], BLOCK_SIZE);
            neoscrypt_salsa(&X[0], rounds);
            neoscrypt_blkxor(&X[16], &X[0], BLOCK_SIZE);
            neoscrypt_salsa(&X[16], rounds);
        }
        return;
    }

    if(r == 2) {
        if(mixer) {
            neoscrypt_blkxor(&X[0], &X[48], BLOCK_SIZE);
            neoscrypt_chacha(&X[0], rounds);
            neoscrypt_blkxor(&X[16], &X[0], BLOCK_SIZE);
            neoscrypt_chacha(&X[16], rounds);
            neoscrypt_blkxor(&X[32], &X[16], BLOCK_SIZE);
            neoscrypt_chacha(&X[32], rounds);
            neoscrypt_blkxor(&X[48], &X[32], BLOCK_SIZE);
            neoscrypt_chacha(&X[48], rounds);
            neoscrypt_blkswp(&X[16], &X[32], BLOCK_SIZE);
        } else {
            neoscrypt_blkxor(&X[0], &X[48], BLOCK_SIZE);
            neoscrypt_salsa(&X[0], rounds);
            neoscrypt_blkxor(&X[16], &X[0], BLOCK_SIZE);
            neoscrypt_salsa(&X[16], rounds);
            neoscrypt_blkxor(&X[32], &X[16], BLOCK_SIZE);
            neoscrypt_salsa(&X[32], rounds);
            neoscrypt_blkxor(&X[48], &X[32], BLOCK_SIZE);
            neoscrypt_salsa(&X[48], rounds);
            neoscrypt_blkswp(&X[16], &X[32], BLOCK_SIZE);
        }
        return;
    }

    /* Reference code for any reasonable r */
    for(i = 0; i < 2 * r; i++) {
        if(i) neoscrypt_blkxor(&X[16 * i], &X[16 * (i - 1)], BLOCK_SIZE);
        else  neoscrypt_blkxor(&X[0], &X[16 * (2 * r - 1)], BLOCK_SIZE);
        if(mixer)
          neoscrypt_chacha(&X[16 * i], rounds);
        else
          neoscrypt_salsa(&X[16 * i], rounds);
        neoscrypt_blkcpy(&Y[16 * i], &X[16 * i], BLOCK_SIZE);
    }
    for(i = 0; i < r; i++)
      neoscrypt_blkcpy(&X[16 * i], &Y[16 * 2 * i], BLOCK_SIZE);
    for(i = 0; i < r; i++)
      neoscrypt_blkcpy(&X[16 * (i + r)], &Y[16 * (2 * i + 1)], BLOCK_SIZE);
}


/* NeoScrypt core engine:
 * p = 1, salt = password;
 * Basic customisation (required):
 *   profile bit 0:
 *     0 = NeoScrypt(128, 2, 1) with Salsa20/20 and ChaCha20/20;
 *     1 = Scrypt(1024, 1, 1) with Salsa20/8;
 *   profile bits 4 to 1:
 *     0000 = FastKDF-BLAKE2s;
 *     0001 = PBKDF2-HMAC-SHA256;
 *     0010 = PBKDF2-HMAC-BLAKE256;
 * Extended customisation (optional):
 *   profile bit 31:
 *     0 = extended customisation absent;
 *     1 = extended customisation present;
 *   profile bits 7 to 5 (rfactor):
 *     000 = r of 1;
 *     001 = r of 2;
 *     010 = r of 4;
 *     ...
 *     111 = r of 128;
 *   profile bits 12 to 8 (Nfactor):
 *     00000 = N of 2;
 *     00001 = N of 4;
 *     00010 = N of 8;
 *     .....
 *     00110 = N of 128;
 *     .....
 *     01001 = N of 1024;
 *     .....
 *     11110 = N of 2147483648;
 *   profile bits 30 to 13 are reserved */
void neoscrypt(const unsigned char *password, unsigned char *output, unsigned int profile) {
    const size_t stack_align = 0x40;
    unsigned int N = 128, r = 2, dblmix = 1, mixmode = 0x14;
    unsigned int kdf, i, j;
    unsigned int *X, *Y, *Z, *V;

    if(profile & 0x1) {
        N = 1024;        /* N = (1 << (Nfactor + 1)); */
        r = 1;           /* r = (1 << rfactor); */
        dblmix = 0;      /* Salsa only */
        mixmode = 0x08;  /* 8 rounds */
    }

    if(profile >> 31) {
        N = (1 << (((profile >> 8) & 0x1F) + 1));
        r = (1 << ((profile >> 5) & 0x7));
    }

    unsigned char stack[(N + 3) * r * 2 * BLOCK_SIZE + stack_align];
    /* X = r * 2 * BLOCK_SIZE */
    X = (unsigned int *) (((size_t)stack & ~(stack_align - 1)) + stack_align);
    /* Z is a copy of X for ChaCha */
    Z = &X[32 * r];
    /* Y is an X sized temporal space */
    Y = &X[64 * r];
    /* V = N * r * 2 * BLOCK_SIZE */
    V = &X[96 * r];

    /* X = KDF(password, salt) */
    kdf = (profile >> 1) & 0xF;

    switch(kdf) {

        default:
        case(0x0):
#ifdef OPT
            neoscrypt_fastkdf_opt(password, password, (unsigned char *) X, 0);
#else
            neoscrypt_fastkdf(password, 80, password, 80, 32,
              (unsigned char *) X, r * 2 * BLOCK_SIZE);
#endif
            break;

        case(0x1):
            neoscrypt_pbkdf2_sha256(password, 80, password, 80, 1,
              (unsigned char *) X, r * 2 * BLOCK_SIZE);
            break;

    }

    /* Process ChaCha 1st, Salsa 2nd and XOR them into FastKDF; otherwise Salsa only */

    if(dblmix) {
        /* blkcpy(Z, X) */
        neoscrypt_blkcpy(&Z[0], &X[0], r * 2 * BLOCK_SIZE);

        /* Z = SMix(Z) */
        for(i = 0; i < N; i++) {
            /* blkcpy(V, Z) */
            neoscrypt_blkcpy(&V[i * (32 * r)], &Z[0], r * 2 * BLOCK_SIZE);
            /* blkmix(Z, Y) */
            neoscrypt_blkmix(&Z[0], &Y[0], r, (mixmode | 0x0100));
        }

        for(i = 0; i < N; i++) {
            /* integerify(Z) mod N */
            j = (32 * r) * (Z[16 * (2 * r - 1)] & (N - 1));
            /* blkxor(Z, V) */
            neoscrypt_blkxor(&Z[0], &V[j], r * 2 * BLOCK_SIZE);
            /* blkmix(Z, Y) */
            neoscrypt_blkmix(&Z[0], &Y[0], r, (mixmode | 0x0100));
        }
    }

    /* X = SMix(X) */
    for(i = 0; i < N; i++) {
        /* blkcpy(V, X) */
        neoscrypt_blkcpy(&V[i * (32 * r)], &X[0], r * 2 * BLOCK_SIZE);
        /* blkmix(X, Y) */
        neoscrypt_blkmix(&X[0], &Y[0], r, mixmode);
    }
    for(i = 0; i < N; i++) {
        /* integerify(X) mod N */
        j = (32 * r) * (X[16 * (2 * r - 1)] & (N - 1));
        /* blkxor(X, V) */
        neoscrypt_blkxor(&X[0], &V[j], r * 2 * BLOCK_SIZE);
        /* blkmix(X, Y) */
        neoscrypt_blkmix(&X[0], &Y[0], r, mixmode);
    }

    if(dblmix)
      /* blkxor(X, Z) */
      neoscrypt_blkxor(&X[0], &Z[0], r * 2 * BLOCK_SIZE);

    /* output = KDF(password, X) */
    switch(kdf) {

        default:
        case(0x0):
#ifdef OPT
            neoscrypt_fastkdf_opt(password, (unsigned char *) X, output, 1);
#else
            neoscrypt_fastkdf(password, 80, (unsigned char *) X,
              r * 2 * BLOCK_SIZE, 32, output, 32);
#endif
            break;

        case(0x1):
            neoscrypt_pbkdf2_sha256(password, 80, (unsigned char *) X,
              r * 2 * BLOCK_SIZE, 1, output, 32);
            break;

    }

}

#endif /* !(USE_ASM) */
