#include "stdafx.h"
#include <stdarg.h>

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
