#if (__cplusplus)
extern "C" {
#endif

void neoscrypt(const unsigned char *password, unsigned char *output,
  unsigned int profile);

void neoscrypt_blake2s(const void *input, const unsigned int input_size,
  const void *key, const unsigned char key_size,
  void *output, const unsigned char output_size);

void neoscrypt_copy(void *dstp, const void *srcp, unsigned int len);
void neoscrypt_erase(void *dstp, unsigned int len);
void neoscrypt_xor(void *dstp, const void *srcp, unsigned int len);

#if (__cplusplus)
}
#else

#ifndef MIN
#define MIN(a, b) ((a) < (b) ? a : b)
#endif

#ifndef MAX
#define MAX(a, b) ((a) > (b) ? a : b)
#endif

#define BLOCK_SIZE 64
#define DIGEST_SIZE 32

typedef unsigned char hash_digest[DIGEST_SIZE];

#define ROTL32(a,b) (((a) << (b)) | ((a) >> (32 - b)))
#define ROTR32(a,b) (((a) >> (b)) | ((a) << (32 - b)))

#define U8TO32_BE(p) \
    (((unsigned int)((p)[0]) << 24) | ((unsigned int)((p)[1]) << 16) | \
    ((unsigned int)((p)[2]) <<  8) | ((unsigned int)((p)[3])))

#define U32TO8_BE(p, v) \
    (p)[0] = (unsigned char)((v) >> 24); (p)[1] = (unsigned char)((v) >> 16); \
    (p)[2] = (unsigned char)((v) >>  8); (p)[3] = (unsigned char)((v)      );

#define U64TO8_BE(p, v) \
    U32TO8_BE((p),     (unsigned int)((v) >> 32)); \
    U32TO8_BE((p) + 4, (unsigned int)((v)      ));

#endif