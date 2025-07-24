#include "crencrypt.h"

int main(int argc, char *argv[]) {
    
    if (argc == 2 && strcmp(argv[1], "-help") == 0) {
        printf("Usage: crencrypt <-(encrypt/e)/(decrypt/d)> [-(n/nopreserve)/(p/preserve)] <key> <infile> [out]\n");
        printf("-encrypt -e: encrypt mode\n");
        printf("-decrypt -d: decrypt mode\n");
        printf("-preserve -p (OPTIONAL): preserve the infile\n");
        printf("-nopreserve -n (OPTIONAL): safe delete the infile\n");
        printf("key: the characters used to encrypt\n");
        printf("infile: the input file\n");
        printf("outfile (OPTIONAL): the output file\n");
        exit(0);
    }

    if (argc < 4) {
        printf("Too little arguments. Use -help.");
        exit(0);
    }

    //flags
    char mode = 0b10;
    char preserve = 0b10;
    CRKey key;
    char* infile = NULL;
    char* outfile = NULL;

    int x;

    for (x = 1; x < argc; x++) {
        
        // Check for null strings
        if (argv[x][0] == '\0') {
            printf("Null string argument.");
            exit(0);
        }

        // End of flags
        if (argv[x][0] != '-') break;


        if (argv[x][1] == 'e') {
            checkDuplicate(mode, argv[x]);
            mode = 0;
        } else if (argv[x][1] == 'd') {
            checkDuplicate(mode, argv[x]);
            mode = 1;
        } else if (argv[x][1] == 'p') {
            checkDuplicate(preserve, argv[x]);
            preserve = 0;
        } else if (argv[x][1] == 'n') {
            checkDuplicate(preserve, argv[x]);
            preserve = 1;
        }

        /*if (strcmp(argv[x], "-e") == 0 || strcmp(argv[x], "-encrypt")) {
            checkDuplicate(mode, argv[x]);
            mode = 0;
        } else if (strcmp(argv[x], "-d") == 0) {
            checkDuplicate(mode, argv[x]);
            mode = 1;
        } else if (strcmp(argv[x], "-p") == 0) {
            checkDuplicate(preserve, argv[x]);
            preserve = 0;
        } else if (strcmp(argv[x], "-n") == 0) {
            checkDuplicate(preserve, argv[x]);
            preserve = 1;
        } else {
            break;
        }*/

    }

    if (argc <= x) {
        printf("Missing key. Use -help.");
        exit(0);
    }

    key.data = argv[x++];
    key.length = strlen(key.data);

    if (argc <= x) {
        printf("Missing in file. Use -help.");
        exit(0);
    }

    infile = argv[x++];

    if (argc <= x) {

        int otfSize = strlen(infile) + 4 + 1;
        outfile = calloc(sizeof(char), otfSize);

        printf("Outfile not provided. Using Default...\n");

        if (mode & 1) {
            snprintf(outfile, otfSize, "%s.dec", infile);
        } else {
            snprintf(outfile, otfSize, "%s.enc", infile);
        }

        outfile[otfSize - 1] = '\0';

    } else {
        outfile = argv[x++];
    }

    printf("Mode: %d, Preserve: %d, Key: %s, KeyL: %d, if: %s, of: %s\n", 
            mode, preserve, key.data, key.length, infile, outfile);

    if (mode & 1) {
        decryptFile(&key, infile, outfile);
    } else {
        encryptFile(&key, infile, outfile);
    }

    if (preserve & 1) {
        wipeFile(infile);
    }

    return 0;
}