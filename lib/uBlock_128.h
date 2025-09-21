// uBlock_128.h
#ifndef UBLOCK_128_H
#define UBLOCK_128_H

#include <cstdio>
#include <cstring>
#include <ctime>
#include <cstdlib>
#include <windows.h>
#include <malloc.h>
#include <emmintrin.h>  // SSE2
#include <pmmintrin.h>  // SSE3
#include <tmmintrin.h>  // SSSE3
#include <smmintrin.h>  // SSE4.1
#include <nmmintrin.h>  // SSE4.2
#include <intrin.h>     // Intrinsics

// 声明为extern
extern __m128i S;
extern __m128i S_Inv;
extern __m128i con;
extern __m128i A1, A2, A3;
extern unsigned char Subkey[17][32];
extern unsigned char RC[16][16];

// 核心函数
void uBlock_128_KeySchedule(unsigned char *key);
void uBlock_128_Encrypt(unsigned char *plain, unsigned char *cipher, int round);
void uBlock_128_Decrypt(unsigned char *cipher, unsigned char *plain, int round);

// 加密接口
int Crypt_Enc_Block(unsigned char *input, int in_len, 
                   unsigned char *output, int *out_len,
                   unsigned char *key, int keylen);

int Crypt_Dec_Block(unsigned char *input, int in_len,
                   unsigned char *output, int *out_len,
                   unsigned char *key, int keylen);

int Crypt_Enc_Block_Round(unsigned char *input, int in_len,
                         unsigned char *output, int *out_len,
                         unsigned char *key, int keylen, int cryptround);

// 密钥接口
int Key_Schedule(unsigned char *seedkey, int keylen,
                int direction, unsigned char *subkey);


#endif // UBLOCK_128_H