#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <errno.h>

#define HSZ 64

unsigned char *liff_hash_word(const char *word)
{
    size_t len = strlen(word);
    int i = 0, hi = 0, iterations = 0, c;
    unsigned char *p = malloc(sizeof(*p)*HSZ+1);
    memset(p, '\0', HSZ+1);
    while(iterations < 1) {
        for(i = 0; i < len; i++) {
            c = word[i];
            p[hi] += c;
            if(hi++ >= HSZ) {
                hi = 0;
                iterations++;
            }
        }
    }
    return p;
}
unsigned char *liff_hash(const char *path1)
{
    FILE *f = NULL;
    int hi = 0, iterations = 0, c;
    unsigned char *p = malloc(sizeof(*p)*HSZ+1);
    memset(p, '\0', HSZ+1);
    
    while(iterations < 1 ) {
        if(!(f = fopen(path1, "rb"))) {
            perror("fopen");
            return 0;
        }
        while((c = fgetc(f)) != EOF) {
            p[hi] += c;
            if(hi++ >= HSZ) {
                hi = 0;
                iterations++;
            }
        }
        fclose(f);
    }
    return p;
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

double liff_ratio(unsigned char *h1, unsigned char *h2)
{
    int i = 1, j;
    double total_avg = 0.0;
    int total = 0;
    uint64_t t1 = 0, t2 = 0;

    for(; i < HSZ; i++) {
        t1 += h1[i-1] * h1[i];
        t2 += h2[i-1] * h2[i];
        if(h1[i] > h2[i]) {
            total_avg += (double)h2[i] / (double)h1[i]; 
        } else {
            total_avg += (double)h1[i] / (double)h2[i]; 
        }
        ///printf("%.2f\n", total_avg);
        for(j = 0; j < HSZ; j++) {
            if(h1[i] == h2[j]) {
                total++;
                break;
            }
        }
    }
    if(t2 > t1) {
        return 100.0 * ((double)t1 / (double)t2);

    }
    return 100.0 * ((double)t2 / (double)t1);
    printf("total: %d\n", total);
    return 100.0 * ((double)total / (double)HSZ);
    //return 100.0 * (total_avg / (double)HSZ);
}

void liff_print(const unsigned char *l)
{
    int i = 0;
    for(; i < HSZ; i++)
        printf("%02x", *(l+i)&0xff);
}

int main(int argc, char **argv)
{
    unsigned char *r1 = NULL, *r2 = NULL;
    double rv;

    if(argc < 3)
        return usage();

    if(strcmp(argv[1], "-c") == 0) {
        if(argc != 4) {
            printf("incorrect parameter count. Ratio expects two arguments\n");
            return usage();
        }
        r1 = (unsigned char *)argv[2];
        r2 = (unsigned char *)argv[3];
        rv = liff_ratio(r1, r2);
        printf("%.3f\n", rv);
    } else if(strcmp(argv[1], "-x") == 0 && argc == 3) {
        r1 = liff_hash(argv[2]);
        liff_print(r1);
        printf("\n");
    } else if(strcmp(argv[1], "-w") == 0 && argc == 3) {
        r1 = liff_hash_word(argv[2]);
        printf("r1: %02x\n", r1[0]);
        liff_print(r1);
        printf("\n");
    } else if(strcmp(argv[1], "-e") == 0 && argc == 4) {
        r1 = liff_hash_word(argv[2]);
        r2 = liff_hash_word(argv[3]);
        rv = liff_ratio(r1, r2);
        printf("%.3f\n", rv);
    } else if(strcmp(argv[1], "-z") == 0) {
        if(argc != 4) {
            printf("wrong argument count.\n");
            return usage();
        }
        r1 = liff_hash(argv[2]);
        liff_print(r1); printf("\n");
        r2 = liff_hash(argv[3]);
        liff_print(r2); printf("\n");
        rv = liff_ratio(r1, r2);
        printf("%.3f\n", rv);
    } else {
        printf("unknown option: %s\n", argv[1]);
        return usage();
    }
    return 0;
}
