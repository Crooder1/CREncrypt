#include "crencrypt.h"

void checkDuplicate(int value, char* arg) {

    if (!(value >> 1)) {
        printf("Duplicate Argument: %s", arg);
        exit(0);
    }

}

unsigned int getKeySeed(CRKey* key) {

    unsigned int seed = 0;

    for (int x = 0; x < key->length; x++) {
        seed = seed * 31 + key->data[x];
    }

    return seed;
}

unsigned int getNonce() {
    
    unsigned char buffer[4];

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

    unsigned int nonce = 0;

    for (int i = 0; i < 4; i++) {

        nonce = nonce << 8;
        nonce += (int) buffer[i];

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
    int seed = getKeySeed(key);
    int nonce = getNonce();
    fwrite(&nonce, sizeof(int), sizeof(int), ofp);
    srand(seed + nonce);

    char buffer[128];

    int read;
    int size;

    while ((read = fread(buffer, sizeof(char), 128, ifp)) != 0) {

        int* result;
        size = encryptText(key, buffer, read, &result);

        fwrite(result, sizeof(int), size, ofp);

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

    int seed = getKeySeed(key);
    int nonce;
    fread(&nonce, sizeof(int), sizeof(int), ifp);
    srand(seed + nonce);

    int buffer[128];

    int read;
    int size;

    while ((read = fread(buffer, sizeof(int), 128, ifp)) != 0) {

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

        int toWrite = fileSize - written > 4096 ? 4096 : fileSize - written;

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

int encryptText(CRKey* key, char* text, int textSize, int** cipher) {

    int* result = malloc((textSize) * sizeof(int));

    for (int i = 0; i < textSize; i++) {

        int r = (int) text[i];

        for (int j = 0; j < key->length; j++) {

            r += rand() % key->data[j];

        }

        result[i] = r;
    }

    *cipher = result;

    return textSize;
}

int decryptText(CRKey* key, int* data, int dataSize, char** text) {

    char* result = malloc((dataSize) * sizeof(char));

    for (int i = 0; i < dataSize; i++) {

        int r = data[i];

        for (int j = 0; j < key->length; j++) {

            r -= rand() % key->data[j];
            
        }

        result[i] = (char) r;

    }

    *text = result;

    return dataSize;
}