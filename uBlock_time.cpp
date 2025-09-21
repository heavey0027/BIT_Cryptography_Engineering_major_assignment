/*
    此程序用于测试AES（硬件加速）、传统AES与uBlock的吞吐量
    硬件加速AES实现源代码来自openssl/evp.h
    传统AES实现源代码来自openssl/aes.h
    注意：成功运行程序会有两个警报，意思为这两个函数已经被Openssl3.0在以后弃用，不影响实验结果
*/
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <openssl/evp.h>
#include <openssl/aes.h>  
#include "lib/uBlock_128.h"

// 测试配置
#define BLOCK_SIZE      16      // AES和uBlock的块大小
#define TOTAL_DATA_SIZE (16 * 10000000) // 160MB数据
#define WARMUP_ROUNDS   10      // 预热轮次

// 统一的性能测试函数
void benchmark(const char *algo_name, 
               void (*encrypt_func)(unsigned char*, int, unsigned char*, int*, unsigned char*, int),
               unsigned char *key, int key_len) {
    // 分配对齐内存（64字节对齐，提升缓存效率）
    unsigned char *plaintext = (unsigned char *)_aligned_malloc(TOTAL_DATA_SIZE, 64);
    unsigned char *ciphertext = (unsigned char *)_aligned_malloc(TOTAL_DATA_SIZE, 64);
    
    // 初始化数据（模式化数据便于验证）
    for (int i = 0; i < TOTAL_DATA_SIZE; i++) {
        plaintext[i] = i % 256;
    }

    // 预热（减少冷启动误差）
    int out_len;
    for (int i = 0; i < WARMUP_ROUNDS; i++) {
        encrypt_func(plaintext, BLOCK_SIZE, ciphertext, &out_len, key, key_len);
    }

    // 高精度计时
    LARGE_INTEGER start, end, freq;
    QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&start);

    // 执行加密（一次性处理全部数据）
    encrypt_func(plaintext, TOTAL_DATA_SIZE, ciphertext, &out_len, key, key_len);

    QueryPerformanceCounter(&end);
    double elapsed = (double)(end.QuadPart - start.QuadPart) / freq.QuadPart;
    printf("[%-18s] 吞吐量: %.2f MB/s\n", algo_name, (TOTAL_DATA_SIZE / (1024.0 * 1024)) / elapsed);

    _aligned_free(plaintext);
    _aligned_free(ciphertext);
}

// uBlock测试适配器
void uBlock_encrypt_wrapper(unsigned char *in, int in_len, 
                           unsigned char *out, int *out_len, 
                           unsigned char *key, int key_len) {
    Crypt_Enc_Block(in, in_len, out, out_len, key, key_len);
}

// OpenSSL EVP AES测试（现代推荐方式）
void aes_evp_encrypt(unsigned char *in, int in_len, 
                    unsigned char *out, int *out_len, 
                    unsigned char *key, int key_len) {
    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    EVP_EncryptInit_ex(ctx, EVP_aes_128_ecb(), NULL, key, NULL);
    EVP_CIPHER_CTX_set_padding(ctx, 0);
    
    int total_encrypted = 0;
    while (total_encrypted < in_len) {
        int chunk_size = (in_len - total_encrypted > 1024 * 1024) ? 1024 * 1024 : in_len - total_encrypted;
        EVP_EncryptUpdate(ctx, out + total_encrypted, out_len, in + total_encrypted, chunk_size);
        total_encrypted += chunk_size;
    }
    
    EVP_CIPHER_CTX_free(ctx);
}

// 传统AES测试（直接使用AES_encrypt）
void aes_legacy_encrypt(unsigned char *in, int in_len, 
                       unsigned char *out, int *out_len, 
                       unsigned char *key, int key_len) {
    AES_KEY aes_key;
    AES_set_encrypt_key(key, 128, &aes_key);
    
    for (int i = 0; i < in_len; i += BLOCK_SIZE) {
        AES_encrypt(in + i, out + i, &aes_key);
    }
}

int main() {
    // 初始化OpenSSL
    OPENSSL_init_crypto(OPENSSL_INIT_LOAD_CRYPTO_STRINGS, NULL);

    // 统一测试密钥
    unsigned char key[16] = {0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF,
                            0xFE, 0xDC, 0xBA, 0x98, 0x76, 0x54, 0x32, 0x10};

    // 执行三种算法的测试
    benchmark("AES-EVP (OpenSSL)", aes_evp_encrypt, key, 16);
    benchmark("AES-Legacy", aes_legacy_encrypt, key, 16);
    benchmark("uBlock-128", uBlock_encrypt_wrapper, key, 16);

    return 0;
}