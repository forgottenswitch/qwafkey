#ifndef FREADLINE_H
#define FREADLINE_H

#include "stdafx.h"

/* freadline.h -- reading a line from file */

typedef struct {
    char *s;
    size_t len;
    size_t size;
} read_line_buf;

void read_line_buf_init(read_line_buf *buf);
void read_line_buf_free(read_line_buf *buf);
char *fread_line(FILE *f, read_line_buf *buf);

#endif // FREADLINE_H
