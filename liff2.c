#include <stdio.h>
#include <stdint.h>

#define MAX 64
enum opcodes {
    INC,
    DEC,
    ADD,
    SUB,
    MUL,
    DIV,
    MOD,
    SHIFTL,
    SHIFTR,
    TWIDDLE,
    XOR,
    AND,
    OR
};

#define OPCODE_SZ 13

int opcode_map[256] = {0};

void init_opcode_map()
{
    int i = 0;
    int every = MAX / OPCODE_SZ;
    int curopcode = INC;
    for(; i < 256; i++) {
        opcode_map[i] = curopcode;
        if(i % every == 0 && i > 0) {
            curopcode++;
            if(curopcode > OPCODE_SZ)
                curopcode = INC;
        }
    }
}


inline uint64_t unpack_uint64(unsigned char *buf)
{
    return  ((uint64_t)buf[0] << 56) | ((uint64_t)buf[1] << 48) |\
            ((uint64_t)buf[2] << 40) | ((uint64_t)buf[3] << 32) |\
            ((uint64_t)buf[4] << 24) | ((uint64_t)buf[5] << 16) |\
            ((uint64_t)buf[6] << 8)  | ((uint64_t)buf[7]);
}
void liff_hash(const char *path1)
{
    FILE *f = NULL;
    off_t fsize = 0;
    unsigned char hash[MAX] = {0};
    int c, p = 0;
    size_t loops = 0;
    size_t x = 0, y;
    int opcode = INC;
    int fulfilled = 0;
    int maxtries = 100;
    int since_last_inc = 0;

    while(!fulfilled && maxtries--) {
        if(!(f = fopen(path1, "rb"))) {
            perror("fopen");
            return;
        }
        while((c = fgetc(f)) != EOF) {
            //printf("c:%d\n", c);
            opcode = opcode_map[(unsigned char)c];
            //printf("op:%d\n", opcode);
            since_last_inc++;
            switch(opcode) {
            case INC:
                x++;
                if(x >= MAX) {
                    fulfilled = 1;
                    x = 0;
                }
                hash[x] = c;
                since_last_inc = 0;
                break;
            case DEC:
                if(x > 0)
                    x--;
                hash[x] = c;
                break;
            case ADD:
                hash[x] = c+x; break;
            case SUB:
                hash[x] = c-x; break;
            case MUL:
                hash[x] = c*x; break;
            case DIV:
                hash[x] = c+1/(x+1); break;
            case MOD:
                //hash[x] = c % x+1; break;
                hash[x] = c ;
            case SHIFTL:
                hash[x] = c<<1; break;
            case SHIFTR:
                hash[x] = c>>1; break;
            case TWIDDLE:
                hash[x] = ~c; break;
            case XOR:
                hash[x] = c^x; break;
            case AND:
                hash[x] = c&x; break;
            case OR:
                hash[x] = c|x; break;
            }
            /*if(since_last_inc > MAX/5) {
                since_last_inc = 0;
                if(x++ >= MAX) {
                    x = 0;
                    fulfilled = 1;
                }
                
            }*/
        }
        if(x+1 >= MAX)
            fulfilled = 1;
        fclose(f);
    }
    //printf("fullfilled: %d, maxtries: %d\n", fulfilled, maxtries);
    printf("%s ", path1);
    uint64_t total_p = 0;
    for(x = 0; x < MAX; x++) {
        printf("%02x", hash[x]);
    }
    for(x = 0; x < MAX/8; x+=8) {
        total_p += unpack_uint64(hash+x);
    }
    printf("\n");
    printf("total_p: %d\n", total_p);
}

int main(int argc, char **argv)
{
    init_opcode_map();
    liff_hash(argv[1]);
    return 0;
}
