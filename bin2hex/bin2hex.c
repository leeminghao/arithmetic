#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <unistd.h>
#include <sys/stat.h>

int main(int argc, char *argv[])
{
    int ret = 0;
    FILE *in = NULL;
    FILE *out = NULL;
    struct stat st;
    unsigned char *buf = NULL;
    int i, n;

    if (stat(argv[1], &st) < 0) {
        fprintf(stderr, "stat '%s' error: %s\n", argv[1], strerror(errno));
        return 1;
    }

    in = fopen(argv[1], "rb");
    if (in == NULL) {
        fprintf(stderr, "open '%s' error: %s\n", argv[1], strerror(errno));
        return 2;
    }

    out = fopen(argv[2], "w");
    if (out == NULL) {
        fprintf(stderr, "open '%s' error: %s\n", argv[2], strerror(errno));
        ret = 3;
        goto done;
    }

    buf = malloc(st.st_size);
    if (buf == NULL) {
        fprintf(stderr, "malloc '%lu' bytes error", st.st_size);
        ret = 4;
        goto done;
    }

    n = fread(buf, 1, st.st_size, in);
    for (i = 0; i < n; ++i) {
        if (i % 16 == 0) {
            fprintf(out, "\n");
        }

        if (buf[i] < 16) {
            fprintf(out, "0x0%x, ", buf[i]);
        } else {
            fprintf(out, "0x%x, ", buf[i]);
        }
    }
    fflush(out);

done:
    if (in != NULL) fclose(in);
    if (out != NULL) fclose(out);
    if (buf != NULL) free(buf);

    return ret;
}
