#include "freadline.h"

/* freadline.c -- reading a line from file
 * */

void read_line_buf_init(read_line_buf *buf) {
    buf->len = 0;
    buf->s = (char*)malloc(buf->size = 255);
}

void read_line_buf_free(read_line_buf *buf) {
    buf->size = 0;
    buf->len = 0;
    free(buf->s);
    buf->s = NULL;
}

char *fread_line(FILE *f, read_line_buf *buf) {
    size_t size = buf->size;
    char *s = buf->s;
    do {
        s = fgets(s, size, f);
        size_t i;
        if (s == NULL) {
            buf->len = 0;
            break;
        }
        fori (i, 0, size) {
            if (!s[i]) {
                break;
            }
        }
        if (i < size-1) {
            break;
        }
        s = (char*) realloc(s, (size = buf->size = buf->size * 3 / 2));
        buf->s = s;
        fseek(f, -i, SEEK_CUR);
    } while (1);
    //printf("RL |||%s|||\n", s);
    return s;
}

