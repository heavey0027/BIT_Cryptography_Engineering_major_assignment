#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include "lib/uBlock_128.h"

// 生成随机IV
void generate_iv(unsigned char *iv, int len) {
        srand(time(NULL));
        for (int i = 0; i < len; i++) iv[i] = rand() % 256;
}

// ---------------------------------------------
// CBC加密
// ---------------------------------------------
int Crypt_Enc_Block_CBC(unsigned char *input, int in_len, unsigned char *output,unsigned char *key, int keylen, unsigned char *iv) {
    for (int i = 0; i < in_len; i += 16) {
        // 明文块与IV异或
        for (int j = 0; j < 16; j++) {
            input[i + j] ^= iv[j];
        }
        // 加密
        uBlock_128_Encrypt(input + i, output + i, 16);
        // 更新IV为当前密文块
        memcpy(iv, output + i, 16);
    }
    return 0;
}

// ---------------------------------------------
// CBC解密
// ---------------------------------------------
int Crypt_Dec_Block_CBC(unsigned char *input, int in_len, unsigned char *output,unsigned char *key, int keylen, unsigned char *iv) {
    unsigned char temp[16];
    for (int i = 0; i < in_len; i += 16) {
        // 解密当前块
        uBlock_128_Decrypt(input + i, temp, 16);
        // 与IV异或
        for (int j = 0; j < 16; j++) {
            output[i + j] = temp[j] ^ iv[j];
        }
        // 更新IV为当前密文块
        memcpy(iv, input + i, 16);
    }
    return 0;
}

int main() {
    unsigned char key[16] = {0x01,0x23,0x45,0x67,0x89,0xAB,0xCD,0xEF, 0xFE,0xDC,0xBA,0x98,0x76,0x54,0x32,0x10};
    FILE *bmpFile, *encFile, *decFile;
    unsigned char header[54];

    // ===== 加密流程 =====
    bmpFile = fopen("photo/dai_sheng_bird.bmp", "rb");
    if (!bmpFile) {
        printf("无法打开原始文件！\n");
        return 1;
    }
    fread(header, 1, 54, bmpFile);

    // 生成并存储IV
    unsigned char iv[16];
    generate_iv(iv, 16);
    
    encFile = fopen("photo/encrypted_cbc.bmp", "wb");
    fwrite(header, 1, 54, encFile);  // 写入原始头

    // 加密数据部分
    unsigned char buffer[16], encrypted[16];
    while ((fread(buffer, 1, 16, bmpFile)) == 16) {
        Crypt_Enc_Block_CBC(buffer, 16, encrypted, key, 16, iv);
        fwrite(encrypted, 1, 16, encFile);
    }
    fclose(bmpFile);
    fclose(encFile);

    // ===== 解密流程 =====
    encFile = fopen("photo/encrypted_cbc.bmp", "rb");
    fread(header, 1, 54, encFile);  // 读取原始头

    decFile = fopen("photo/decrypted_cbc.bmp", "wb");
    fwrite(header, 1, 54, decFile);  // 恢复原始头

    // 解密数据部分
    unsigned char decrypted[16];
    while ((fread(buffer, 1, 16, encFile)) == 16) {
        Crypt_Dec_Block_CBC(buffer, 16, decrypted, key, 16, iv);
        fwrite(decrypted, 1, 16, decFile);
    }
    fclose(encFile);
    fclose(decFile);

    printf("CBC模式加解密操作成功完成!\n");
    return 0;
}