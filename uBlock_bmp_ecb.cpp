#include <stdio.h>
#include <stdlib.h>
#include "lib/uBlock_128.h"

// ---------------------------------------------
// ECB加密
// ---------------------------------------------
int Crypt_Enc_Block_ECB(unsigned char *input,int in_len,unsigned char *output,unsigned char *key,int keylen) {
    // 检查输入长度是否为16的倍数
    if (in_len % 16 != 0) {
        printf("错误：输入长度(%d)不是16的倍数\n", in_len);
        return -1;
    }
    // 加密每个完整块
    for (int g = 0; g < in_len / 16; g++) {
        uBlock_128_Encrypt(input + g * 16, output + g * 16, 16);
    }
    return 0;
}

// ---------------------------------------------
// ECB解密
// ---------------------------------------------
int Crypt_Dec_Block_ECB(unsigned char *input,int in_len,unsigned char *output,unsigned char *key,int keylen) {
    // 检查输入长度是否为16的倍数
    if (in_len % 16 != 0) {
        printf("错误：输入长度(%d)不是16的倍数\n", in_len);
        return -1;
    }
    // 解密每个完整块
    for (int g = 0; g < in_len / 16; g++) {
        uBlock_128_Decrypt(input + g * 16, output + g * 16, 16);
    }
    return 0;
}

int main() {
    unsigned char key[16] = {0x01,0x23,0x45,0x67,0x89,0xAB,0xCD,0xEF,0xFE,0xDC,0xBA,0x98,0x76,0x54,0x32,0x10};
    FILE *bmpFile, *encFile, *decFile;
    unsigned char header[54]; 

    // ===== 加密流程 =====
    bmpFile = fopen("photo/dai_sheng_bird.bmp", "rb");
    if (!bmpFile) {
        printf("无法打开原始文件！\n");
        return 1;
    }

    // 读取BMP头
    fread(header, 1, 54, bmpFile);

    // 创建输出文件（写入头）
    encFile = fopen("photo/encrypted_ecb.bmp", "wb");
    fwrite(header, 1, 54, encFile);

    // 初始化密钥
    uBlock_128_KeySchedule(key);

    // ECB加密
    unsigned char buffer[16], encrypted[16];
    while (fread(buffer, 1, 16, bmpFile) == 16) {
        Crypt_Enc_Block_ECB(buffer, 16, encrypted, key, 16);
        fwrite(encrypted, 1, 16, encFile);
    }
    fclose(bmpFile);
    fclose(encFile);

    // ===== 解密流程 =====
    encFile = fopen("photo/encrypted_ecb.bmp", "rb");
    fread(header, 1, 54, encFile); // 跳过头

    decFile = fopen("photo/decrypted_ecb.bmp", "wb");
    fwrite(header, 1, 54, decFile);

    // 解密数据部分
    while (fread(buffer, 1, 16, encFile) == 16) {
        Crypt_Dec_Block_ECB(buffer, 16, encrypted, key, 16);
        fwrite(encrypted, 1, 16, decFile);
    }
    fclose(encFile);
    fclose(decFile);

    printf("ECB模式加解密操作成功完成!\n");
    return 0;
}