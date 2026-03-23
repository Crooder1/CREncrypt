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
#include <stdint.h>

#include "pcg32_random.h"

typedef struct {
    char* data;
    uint32_t length;
} CRKey;

void checkDuplicate(int value, char* key);

uint32_t getKeySeed(CRKey* key);
uint32_t getNonce();

void encryptFile(CRKey* key, char* inputFp, char* outputFp);
void decryptFile(CRKey* key, char* inputFp, char* outputFp);
void wipeFile(char* file);

uint32_t encryptText(CRKey* key, char* text, uint32_t textSize, uint32_t** cipher);
uint32_t decryptText(CRKey* key, uint32_t* data, uint32_t dataSize, char** text);

#endif