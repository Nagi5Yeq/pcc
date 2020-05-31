#include <assert.h>
#include <stdio.h>

typedef struct ctx_s {
    int h[5];
    int bytes;
    char buf[64];
} ctx;

void sha1_init(ctx*);
void sha1_update(ctx*, const char*, int);
void sha1_final(unsigned char*, ctx*);

int main(int argc, char* argv[]) {
    ctx c;
    int i;
    size_t len;
    char buf[1024];
    unsigned char md[20];
    FILE* f;

    assert(argc == 2);
    f = fopen(argv[1], "r");
    sha1_init(&c);
    while (!feof(f)) {
        len = fread(buf, 1, 1024, f);
        sha1_update(&c, buf, (int) len);
    }
    sha1_final(md, &c);
    for (i = 0; i < 20; i++) {
        printf("%02x", md[i]);
    }
    putchar('\n');
    fclose(f);
    return 0;
}
