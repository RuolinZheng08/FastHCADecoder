#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include "clHCA.h"
#include "HCADecodeService.h"

#ifndef MAX_PATH
#define MAX_PATH 260
#endif

int main(int argc, char *argv[]) {
    unsigned int count = 0;
    char *filenameOut = NULL;
    float volume = 1;
    unsigned int ciphKey1 = 0x8D0C10FD;
    unsigned int ciphKey2 = 0x00A06A0B;
    unsigned int subKey = 0;
    int mode = 16;
    int loop = 0;
    unsigned int threads = 0;
    bool info = false;
    bool decrypt = false;
    for (int i = 1; i<argc; ++i) {
        if (argv[i][0] == '-' || argv[i][0] == '/') {
            switch (argv[i][1]) {
            case 'o':if (i + 1<argc) { filenameOut = argv[++i]; }break;
            case 'v':volume = (float)atof(argv[++i]); break;
            case 'a':if (i + 1<argc) { ciphKey1 = strtoul(argv[++i], NULL, 16); }break;
            case 'b':if (i + 1<argc) { ciphKey2 = strtoul(argv[++i], NULL, 16); }break;
            case 's':if (i + 1<argc) { subKey = strtoul(argv[++i], NULL, 16); }break;
            case 'm':if (i + 1<argc) { mode = atoi(argv[++i]); }break;
            case 'l':if (i + 1<argc) { loop = atoi(argv[++i]); }break;
            case 't':if (i + 1<argc) { threads = atoi(argv[++i]); }break;
            case 'i':info = true; break;
            case 'c':decrypt = true; break;
            case 'h': 
                printf("usage: ./clHCA [options] input.hca\n"
                    "options:\n"
                      "\t-o      output filename\n"
                      "\t-a      set key (lower 8 hex digits)\n"
                      "\t-b      set key (higher 8 hex digits)\n"
                      "\t-s      set sub key (4 hex digits)\n"
                      "\t-c      try decrypting the file without generating an output\n"
                      "\t-m      mode\n"
                      "\t-l      loop\n"
                      "\t-v      volume\n"
                      "\t-t      number of threads (to speed up decoding)\n"
                      "\t-i      show info about input file and quit\n"
                      "\t-h      show this help message\n\n"); 
                return 0;
            }
        }
        else if (*argv[i]) {
            argv[count++] = argv[i];
        }
    }

    printf("-a %x, -b %X, -s %X\n", ciphKey1, ciphKey2, subKey);

    if (!count) {
        printf("Error: No input file specified.\n");
        return -1;
    }

    HCADecodeService dec{ threads }; // Start decode service
    auto fileslist = new std::pair<std::string, std::pair<void*, size_t>>[count];

    // decode
    for (unsigned int i = 0; i < count; ++i) {

        if (i)filenameOut = NULL;

        char path[MAX_PATH];
        if (!(filenameOut&&filenameOut[0])) {
            path[0] = '\0';
            strncat(path, argv[i], sizeof(path) - 1);
            char *d1 = strrchr(path, '\\');
            char *d2 = strrchr(path, '/');
            char *e = strrchr(path, '.');
            if (e && d1 < e && d2 < e)*e = '\0';
            strcat(path, ".wav");
            filenameOut = path;
        }

        if (info) {
            printf("Showing info for %s:\n", argv[i]);
            clHCA hca(0, 0);
            hca.PrintInfo(argv[i]);
            printf("\n");
        }

        // decrypt
        else if (decrypt) {
            printf("Decrypting %s...\n", argv[i]);
            clHCA hca(ciphKey1, ciphKey2, subKey);
            if (!hca.Decrypt(argv[i])) {
                printf("Error: Failed to decrypt %s\n", argv[i]);
            } else {
                printf("Decrypted successfully with the keys provided.\n");
            }
        }

        // decode
        else {
            printf("Decoding %s...\n", argv[i]);
            auto wavout = dec.decode(argv[i], 0, ciphKey1, ciphKey2, subKey, volume, mode, loop);
            if (!wavout.first) {
                printf("Error: Failed to decode %s\n", argv[i]);
            }
            else {
                fileslist[i] = std::make_pair(std::string(filenameOut), wavout);
            }
        }
    }
    // if decrypt is set, no file produced, skip this part
    if (!info && !decrypt) { // decode
        for (unsigned int i = 0; i < count; ++i) {
            printf("Writing to %s...\n", fileslist[i].first.c_str());
            FILE* outfile = fopen(fileslist[i].first.c_str(), "wb");
            if (!outfile) {
                printf("Error: Failed to write to output\n");
                dec.cancel_decode(fileslist[i].second.first);
            }
            else {
                dec.wait_on_request(fileslist[i].second.first);
                fwrite(fileslist[i].second.first, 1, fileslist[i].second.second, outfile);
                fclose(outfile);
            }
            operator delete(fileslist[i].second.first);
        }
    }
    
    delete[] fileslist;

    return 0;
}
