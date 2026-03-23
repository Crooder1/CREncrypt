#include "crencrypt.h"

void checkDuplicate(int value, char* arg) {

    if (!(value >> 1)) {
        printf("Duplicate Argument: %s", arg);
        exit(0);
    }

}

uint32_t getKeySeed(CRKey* key) {

    uint32_t seed = 0;

    for (uint32_t x = 0; x < key->length; x++) {
        seed = seed * 31 + key->data[x];
    }

    return seed;
}

uint32_t getNonce() {
    
    int8_t buffer[4];

#if defined(__linux__)

    if (getrandom(buffer, sizeof(buffer), 0) == -1) {
        printf("Error from random.");
        exit(0);
    }

#elif defined(_WIN32)
    
    HCRYPTPROV hProvider = 0;

    if (!CryptAcquireContext(&hProvider, NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT)) {
        printf("CryptAcquireContext failed.");
        exit(0);
    }

    if (!CryptGenRandom(hProvider, sizeof(buffer), (BYTE *)buffer)) {
        printf("CryptGenRandom failed.");
        CryptReleaseContext(hProvider, 0);
        exit(0);
    }

    CryptReleaseContext(hProvider, 0);

#else
    printf("Unknown Platform.");
    exit(0);
#endif

    uint32_t nonce = 0;

    for (uint8_t i = 0; i < 4; i++) {

        nonce = nonce << 8;
        nonce += (uint32_t) buffer[i];

    }

    return nonce;
}

void encryptFile(CRKey* key, char* inputFp, char* outputFp) {
    
    FILE *ifp;
    FILE *ofp;

    ifp = fopen(inputFp, "rb");
    ofp = fopen(outputFp, "wb");

    if (ifp == NULL) {
        printf("File %s doesn't exist.", inputFp);
        exit(0);
    }

    if (ofp == NULL) {
        printf("File %s doesn't exist.", outputFp);
        exit(0);
    }

    // Set seed and write nonce
    uint32_t seed = getKeySeed(key);
    uint32_t nonce = getNonce();
    fwrite(&nonce, sizeof(uint32_t), sizeof(uint32_t), ofp);
    pcg32_seed(seed + nonce, nonce);

    char buffer[128];

    uint32_t read;
    uint32_t size;

    while ((read = fread(buffer, sizeof(char), 128, ifp)) != 0) {

        uint32_t* result;
        size = encryptText(key, buffer, read, &result);

        fwrite(result, sizeof(uint32_t), size, ofp);

        free(result);

    }

    fclose(ifp);
    fclose(ofp);

}

void decryptFile(CRKey* key, char* inputFp, char* outputFp) {

    FILE *ifp;
    FILE *ofp;

    ifp = fopen(inputFp, "rb");
    ofp = fopen(outputFp, "wb");

    if (ifp == NULL) {
        printf("File %s doesn't exist.", inputFp);
        exit(0);
    }

    if (ofp == NULL) {
        printf("File %s doesn't exist.", outputFp);
        exit(0);
    }

    uint32_t seed = getKeySeed(key);
    uint32_t nonce;
    fread(&nonce, sizeof(uint32_t), sizeof(uint32_t), ifp);
    pcg32_seed(seed + nonce, nonce);

    uint32_t buffer[128];

    uint32_t read;
    uint32_t size;

    while ((read = fread(buffer, sizeof(uint32_t), 128, ifp)) != 0) {

        char* result;
        size = decryptText(key, buffer, read, &result);

        fwrite(result, sizeof(char), size, ofp);

        free(result);

    }

    fclose(ifp);
    fclose(ofp);

}

void wipeFile(char* fn) {

    FILE* file;
    struct stat st;

    file = fopen(fn, "w");
    
    if (file == NULL) {
        printf("File couldn't be opened.");
        exit(0);
    }

    if (stat(fn, &st) != 0) {
        printf("File stats were not found.");
        exit(0);
    }

    long written = 0;
    long fileSize = st.st_size;
    char buffer[4096] = {0};

    while (fileSize - written > 0) {

        uint32_t toWrite = fileSize - written > 4096 ? 4096 : fileSize - written;

        if (fwrite(buffer, 1, toWrite, file) != toWrite) {
            printf("File writing failed.");
            exit(0);
        }

        written += toWrite;
    }

    fclose(file);

    if (remove(fn) != 0) {
        printf("File couldn't be deleted.");
        exit(0);
    }

}

uint32_t encryptText(CRKey* key, char* text, uint32_t textSize, uint32_t** cipher) {

    uint32_t* result = malloc((textSize) * sizeof(uint32_t));

    for (uint32_t i = 0; i < textSize; i++) {

        uint32_t r = (uint32_t) text[i];

        for (uint32_t j = 0; j < key->length; j++) {

            r += pcg32_random() % key->data[j];

        }

        result[i] = r;
    }

    *cipher = result;

    return textSize;
}

uint32_t decryptText(CRKey* key, uint32_t* data, uint32_t dataSize, char** text) {

    char* result = malloc((dataSize) * sizeof(char));

    for (uint32_t i = 0; i < dataSize; i++) {

        uint32_t r = data[i];

        for (uint32_t j = 0; j < key->length; j++) {

            r -= pcg32_random() % key->data[j];
            
        }

        result[i] = (char) r;

    }

    *text = result;

    return dataSize;
}