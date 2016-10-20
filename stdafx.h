#ifndef STDAFX_H_INCLUDED
#define STDAFX_H_INCLUDED

/* stdafx.h -- common includes and definitions */

#define _WIN32_WINNT 0x0501
#define WINVER 0x502

#if defined(UNICODE) && !defined(_UNICODE)
    #define _UNICODE
#elif defined(_UNICODE) && !defined(UNICODE)
    #define UNICODE
#endif

#ifndef DEBUG
# undef printf
# undef puts
# define printf(...)
# define puts(...)
#endif

#include <tchar.h>
#include <windows.h>

#include <stdbool.h>
#include <inttypes.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "resource.h"

#define ProgramName "qwafkey"
#define DefaultComposeFilename "Compose"
#define DefaultKeydefsFilename "keysymdef.h"

#define fori(i, from, to) for (i = from; i < to; i++)
#define lenof(ary) (sizeof((ary))/sizeof((ary)[0]))
#define lastof(ary) ary[lenof(ary)-1]
#define between(v, min, max) ((min) <= (v) && (v) <= (max))

#define strbcr(buf, from, to)\
    do {\
        size_t l = to - from;\
        if (l > lenof(buf)) { l = lenof(buf); }\
        memcpy(buf, from, l);\
        buf[l-1] = '\0';\
    } while (0)

#define ZeroBuf(ary) ZeroMemory((ary), sizeof((ary)))
#define ZeroPnt(pnt) ZeroMemory((pnt), sizeof(*(pnt)))

typedef unsigned char VK;
typedef unsigned short SC;
#define SC_COUNT 512
#define VK_COUNT 256

#define VK_ch(ch) LOBYTE(VkKeyScan(ch))

typedef struct {
    unsigned char mods;
    union {
        VK vk;
        SC sc;
    };
} KP;

#define KLM_SC 16
#define KLM_WCHAR 32
#define KLM_KA 64

#define KLM_PHYS_TEMP KLM_SC

typedef struct {
    unsigned char active;
    unsigned char mods;
    unsigned short binding;
} LK;

extern char *ConfigDir;
extern char *ProgramDir;
extern char *current_parsing_directory;

void read_keydefs_file(char *path);
void read_compose_file(char *path);
void restart_the_program(void);

void memcpyzn(void *dest, const void *src, size_t n, size_t max);
WCHAR *wcs_concat(WCHAR *str, ...);
WCHAR *wcs_concat_path(WCHAR *str, ...);
char *str_concat_path(char *str, ...);
char *str_concat(char *str, ...);
char *fread_to_eof(FILE *stream, char null_bytes_replacement);
size_t str_remove_cr(char *s);
int str_fills(char *s, size_t len);

KP OS_wchar_to_vk(WCHAR tc);
VK OS_sc_to_vk(SC sc);
SC OS_vk_to_sc(SC sc);
void OS_print_last_error(void);
void OS_activate_layout(HWND hwnd, HKL hkl);
void OS_activate_next_layout(HWND hwnd);
void OS_activate_prev_layout(HWND hwnd);
char *OS_get_window_class(HWND hwnd);
void OS_run_command(TCHAR *cmd);
void OS_run_executable(TCHAR *exe);
HINSTANCE OS_current_module_handle(void);
bool OS_newer_than_Vista(void);
char *OS_user_profile_directory(void);
char *OS_program_directory(void);
char *OS_keyboard_layout_name(LANGID lang);

#define LANGID_Primary LOWORD
#define LANGID_Device HIWORD

#define HK_0 32000

#endif // STDAFX_H_INCLUDED

