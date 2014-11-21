#include <stdio.h>

#define MAX 24
void liff_hash(const char *path1)
{
    FILE *f = fopen(path1, "rb");
    off_t fsize = 0;
    unsigned char hash[MAX] = {0};
    int c, p = 0;
    size_t loops = 0;
    size_t x = 0, y;
    if(!f) {
        perror("fopen");
        return;
    }

    int direction = 0;
    while((c = fgetc(f)) != EOF) {
        hash[x] = c >> 2;
        //printf("%d\n", c >> 4);
        //hash[c >> 4] = c << 2;
        if(direction < 1) {
            x += 1;
        } else {
            x -= 1;
        }
        if(x < 1)
            direction = 0;
        if(x > MAX-1)
            direction = 1;
    }
    fclose(f);
    for(x = 0; x < MAX; x++) {
        printf("%02x", hash[x]);
    }
    printf("\n");
}

int main(int argc, char **argv)
{
    liff_hash(argv[1]);
    return 0;
}
