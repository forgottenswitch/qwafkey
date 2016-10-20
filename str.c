#include "stdafx.h"
#include <stdarg.h>

void memcpyzn(void *dest, const void *src, size_t n, size_t max) {
    size_t i, l = (n > max ? max : n);
    fori(i, 0, l) { ((char*) dest)[i] = ((char*) src)[i]; }
    ((char*) dest)[l] = 0;
}

WCHAR *wcs_concat(WCHAR *str, ...) {
    va_list ap, ap1;
    WCHAR *s, *str1;
    size_t l = wcslen(str);

    va_start(ap, str);
    va_copy(ap1, ap);
    while (1) {
        str1 = va_arg(ap, WCHAR*);
        if (!str1) { break; }
        l += wcslen(str1);
    }
    va_end(ap);

    size_t i = 0;
    s = malloc((l+1) * sizeof(WCHAR));
    while (*str) { s[i++] = *str++; }
    while (1) {
        str1 = va_arg(ap1, WCHAR*);
        if (!str1) { break; }
        while (*str1) { s[i++] = *str1++; }
    }
    va_end(ap1);
    s[l] = 0;
    return s;
}

WCHAR *wcs_concat_path(WCHAR *str, ...) {
    va_list ap, ap1;
    WCHAR *s, *str1;
    size_t l = wcslen(str);

    va_start(ap, str);
    va_copy(ap1, ap);
    while (1) {
        str1 = va_arg(ap, WCHAR*);
        if (!str1) { break; }
        l += wcslen(str1) + 1;
    }
    if (l >= MAX_PATH) { l += 4; }
    va_end(ap);

    size_t i = 0;
    s = malloc((l+1) * sizeof(WCHAR));
    if (l >= MAX_PATH) { s[0] = '\\'; s[1] = '\\'; s[2] = '?'; s[3] = '\\'; i = 4; }
    while (*str) { s[i++] = *str++; }
    s[i++] = '\\';
    while (1) {
        str1 = va_arg(ap1, WCHAR*);
        if (!str1) { break; }
        while (*str1) { s[i++] = *str1++; };
        s[i++] = '\\';
    }
    va_end(ap1);
    s[l] = 0;
    return s;
}

char *str_concat_path(char *str, ...) {
    va_list ap, ap1;
    char *s, *str1;
    size_t l = strlen(str);

    va_start(ap, str);
    va_copy(ap1, ap);
    while (1) {
        str1 = va_arg(ap, char*);
        if (!str1) { break; }
        l += strlen(str1) + 1;
    }
    if (l >= MAX_PATH) { return NULL; }
    va_end(ap);

    size_t i = 0;
    s = malloc(l+1);
    while (*str) { s[i++] = *str++; }
    s[i++] = '\\';
    while (1) {
        str1 = va_arg(ap1, char*);
        if (!str1) { break; }
        while (*str1) { s[i++] = *str1++; };
        s[i++] = '\\';
    }
    va_end(ap1);
    s[l] = 0;
    return s;
}

char *str_concat(char *str, ...) {
    va_list ap, ap1;
    char *s, *str1;
    size_t l = strlen(str);

    va_start(ap, str);
    va_copy(ap1, ap);
    while (1) {
        str1 = va_arg(ap, char*);
        if (!str1) { break; }
        l += strlen(str1);
    }
    va_end(ap);

    size_t i = 0;
    s = malloc(l+1);
    while (*str) { s[i++] = *str++; }
    while (1) {
        str1 = va_arg(ap1, char*);
        if (!str1) { break; }
        while (*str1) { s[i++] = *str1++; }
    }
    va_end(ap1);
    s[l] = 0;
    return s;
}

size_t str_remove_cr(char *s) {
    size_t distance = 0;
    char *s0 = s;
    char c;
    while ((c = *s)) {
        if (c == '\r') { break; }
        s++;
    }
    while ((c = *s)) {
        if (c == '\r') { distance++; s++; continue; }
        s[-distance] = c;
        s++;
    }
    s[-distance] = 0;
    return s - distance - s0;
}

char *fread_to_eof(FILE *stream, char null_bytes_replacement) {
    char *s;
    size_t pos, maxpos, l;

    pos = ftell(stream);
    fseek(stream, 0, SEEK_END);
    maxpos = ftell(stream);
    l = maxpos - pos;
    s = malloc(l+1);
    fseek(stream, pos, SEEK_SET);
    l = fread(s, 1, l, stream);
    fori (pos, 0, l) { if (!s[pos]) { s[pos] = null_bytes_replacement; }; }
    s[l] = 0;
    //printf("fread_eof:<<EOF\n%s\nEOF\n", s);
    l = str_remove_cr(s);
    s = realloc(s, l+1);
    return s;
}

int str_fills(char *s, size_t len) {
    while (1) {
        if (len <= 0) { break; }
        if (*s == 0) { return 1; }
        s++;
    }
    return 0;
}
