#ifndef CRENCRYPT_H
#define CRENCRYPT_H


#if defined(__linux__)
#include <sys/random.h>
#elif defined(_WIN32)
#include <windows.h>
#include <wincrypt.h>
#endif

#include <stdio.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

typedef struct {
    char* data;
    int length;
} CRKey;

void checkDuplicate(int value, char* key);

unsigned int getKeySeed(CRKey* key);
unsigned int getNonce();

void encryptFile(CRKey* key, char* inputFp, char* outputFp);
void decryptFile(CRKey* key, char* inputFp, char* outputFp);
void wipeFile(char* file);

int encryptText(CRKey* key, char* text, int textSize, int** cipher);
int decryptText(CRKey* key, int* data, int dataSize, char** text);

#endif