#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <errno.h>
#include <math.h>

#define PREVS 32

uint64_t liff_hash_word(const char *word)
{
    size_t len = strlen(word);
    int c;
    unsigned int prevs[PREVS] = {0};
    int i = 0, x = 0;
    uint64_t hash = 0, prevs_t = 0;
    for(i = 0; i < len; i++) {
        c = word[i];
        hash += (i+1) * c;
        prevs_t = 0;
        for(x = PREVS-1; x > 0; x--) {
            prevs[x] = prevs[x-1];
            prevs_t += prevs[x];
        }
        prevs[0] = c;
        hash += prevs_t * c;
    }
    hash *= i/2;
    return hash;
}
uint64_t liff_hash(const char *path1)
{
    FILE *f = NULL;
    int c;
    int i = 0, x;
    unsigned int prevs[PREVS] = {0};
    uint64_t hash = 0, prevs_t = 0;
    if(!(f = fopen(path1, "rb"))) {
        perror("fopen");
        return 0;
    }
    while((c = fgetc(f)) != EOF) {
        hash += (i+1) * c;
        prevs_t = 0;
        for(x = PREVS-1; x > 0; x--) {
            prevs[x] = prevs[x-1];
            prevs_t += prevs[x];
        }
        prevs[0] = c;
        hash += prevs_t * c;
        i++;
    }
    fclose(f);
    hash *= i/2;
    return hash;
}

int usage()
{
    printf("usage: liff [OPTION] [ARG]...\n");
    printf("\t-c\tGenerate liff ratio for two liff numbers\n");
    printf("\t-e\tGenerate liff ratio for two words\n");
    printf("\t-w\tGenerate liff number for a word\n");
    printf("\t-x\tGenerate liff number for a file\n");
    printf("\t-z\tGenerate liff ratio for two files\n");
    return 1;
}

double liff_ratio(uint64_t r1, uint64_t r2)
{
    if(r1 > r2)
        return (100.0*((double)r2/(double)r1));
    return (100.0*((double)r1/(double)r2));
}


int main(int argc, char **argv)
{
    uint64_t r1, r2;
    double rv;
    char *endptr;

    if(argc < 3)
        return usage();

    if(strcmp(argv[1], "-c") == 0) {
        if(argc != 4) {
            printf("incorrect parameter count. Ratio expects two arguments\n");
            return usage();
        }
        errno = 0;
        r1 = strtoull(argv[2], &endptr, 10);
        if((errno == ERANGE && (r1 == LONG_MAX || r1 == LONG_MIN)) || (errno != 0 && r1 == 0) || endptr == argv[2]) {
            printf("invalid number: %s\n", argv[2]);
            return usage();
        }
        errno = 0;
        r2 = strtoull(argv[3], &endptr, 10);
        if((errno == ERANGE && (r2 == LONG_MAX || r2 == LONG_MIN)) || (errno != 0 && r2 == 0) || endptr == argv[3]) {
            printf("invalid number: %s\n", argv[3]);
            return usage();
        }
        rv = liff_ratio(r1, r2);
        printf("%.3f\n", rv);
    } else if(strcmp(argv[1], "-x") == 0 && argc == 3) {
        r1 = liff_hash(argv[2]);
        //printf("%s ", argv[2]);
        printf("%ju\n", r1);
    } else if(strcmp(argv[1], "-w") == 0 && argc == 3) {
        r1 = liff_hash_word(argv[2]);
        //printf("%s ", argv[2]);
        printf("%ju\n", r1);
    } else if(strcmp(argv[1], "-e") == 0 && argc == 4) {
        r1 = liff_hash_word(argv[2]);
        r2 = liff_hash_word(argv[3]);
        //printf("%s %ju\n%s %ju\n", argv[2], r1, argv[3], r2);
        rv = liff_ratio(r1, r2);
        printf("%.3f\n", rv);
    } else if(strcmp(argv[1], "-z") == 0) {
        if(argc != 4) {
            printf("wrong argument count.\n");
            return usage();
        }
        r1 = liff_hash(argv[2]);
        r2 = liff_hash(argv[3]);
        //printf("%s %ju\n", argv[2], r1);
        //printf("%s %ju\n", argv[3], r2);
        rv = liff_ratio(r1, r2);
        printf("%.3f\n", rv);
    } else {
        printf("unknown option: %s\n", argv[1]);
        return usage();
    }
    return 0;
}
