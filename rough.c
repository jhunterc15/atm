// #include "atm.h"
// #include "ports.h"
#include <string.h>
#include <stdlib.h>
#include<stdio.h>
// #include <unistd.h>
// #include <openssl/conf.h>
// #include <openssl/evp.h>
// #include <openssl/err.h>
// #include <openssl/ssl.h>
// #include <time.h>

// int encrypt(unsigned char* plainText, int plainTextLen, unsigned char* cipherText); 
// int decrypt(unsigned char* cipherText, int cipherTextLen, unsigned char * plainText); 
// unsigned char key[32];
// unsigned char iv[32];

int main() {
    // // Credits: https://stackoverflow.com/questions/31011041/creating-aes-key-with-c
    // int i = 0;
    // srand(time(NULL));
    // for (i = 0; i < sizeof(key); i++) {
    //     key[i] = rand() % 256;
    // }
    // unsigned char iv[16];
    // i = 0;
    // srand(time(NULL));
    // for (i = 0;  i < sizeof(iv); i++) {
    //     iv[i] = rand() % 256;
    // }
    // char text[50] = "The quick brown fox\0";
    // char cipher[50];
    // for (int u = 0; u < 50; u++)
    //     cipher[u] = '\0'; 
    // int l = encrypt(text, strlen(text), cipher);
    // printf("%d", l);
    // char plainText[l];
    // int l2 = decrypt(cipher, l, plainText); 
    // plainText[l2] = '\0'; 
    // printf("%d\n", l2);
    // printf("Encrypt -> Decrypt: %s\n", plainText); 

    char *temp = "9147483648"; // a number larger than integer 
    long x = strtoll(temp); 
    
    printf("%d", x); 
    
}

// int encrypt(unsigned char* plainText, int plainTextLen, unsigned char* cipherText) {
//     int len, ciphertext_len; 
// 	EVP_CIPHER_CTX *ctx; 
// 	ctx = EVP_CIPHER_CTX_new();
// 	EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, (unsigned char *)key, (unsigned char*) iv);
// 	EVP_CipherUpdate(ctx, cipherText, &len,  plainText, plainTextLen);
//     ciphertext_len = len; 
// 	EVP_CipherFinal(ctx, cipherText + len, &len); 
//     ciphertext_len += len;
//     EVP_CIPHER_CTX_free(ctx);

//  	return ciphertext_len; 
// }

// int decrypt(unsigned char* cipherText, int cipherTextLen, unsigned char * plainText)  {
// 	EVP_CIPHER_CTX *ctx;
// 	int len, plainTextLen;
// 	ctx = EVP_CIPHER_CTX_new();
//     EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, (unsigned char *) key, (unsigned char*)iv);
// 	EVP_DecryptUpdate(ctx, plainText, &len, cipherText, cipherTextLen);
//     plainTextLen = len; 
// 	EVP_DecryptFinal_ex(ctx, plainText + len, &len);
//     plainTextLen += len;
// 	EVP_CIPHER_CTX_free(ctx);
//     return plainTextLen;
// }
