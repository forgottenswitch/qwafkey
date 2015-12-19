#ifndef STDAFX_H_INCLUDED
#define STDAFX_H_INCLUDED

#define _WIN32_WINNT 0x0501
#define WINVER 0x500

#if defined(UNICODE) && !defined(_UNICODE)
    #define _UNICODE
#elif defined(_UNICODE) && !defined(UNICODE)
    #define UNICODE
#endif

#ifdef DEBUG
# define dput printf
# define dputs puts
#else
# define dput(...)
# define dputs(...)
#endif

#include <tchar.h>
#include <windows.h>

#include <stdbool.h>
#include <inttypes.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "resource.h"

#define nil NULL
#define fori(i, from, to) for (i = from; i < to; i++)
#define len(ary) (sizeof((ary))/sizeof((ary)[0]))
#define lenof len
#define last(ary) ary[len(ary)-1]
#define between(v, min, max) ((min) <= (v) && (v) <= (max))
#define isidx(i, ary) (between((i), 0, len((ary))))

#define strbcr(buf, from, to)\
    do {\
        size_t l = to - from;\
        if (l > len(buf)) { l = len(buf); }\
        memcpy(buf, from, l);\
        buf[l-1] = '\0';\
    } while (0)

#define ZeroBuf(ary) ZeroMemory((ary), sizeof((ary)))
#define ZeroPnt(pnt) ZeroMemory((pnt), sizeof(*(pnt)))

typedef unsigned char VK;
typedef unsigned short SC;
#define MAXSC 512

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

KP OS_wchar_to_vk(WCHAR tc);
VK OS_sc_to_vk(SC sc);
SC OS_vk_to_sc(SC sc);
void OS_print_last_error();
void OS_activate_layout(HWND hwnd, HKL hkl);
void OS_activate_next_layout(HWND hwnd);
void OS_activate_prev_layout(HWND hwnd);
char *OS_get_window_class(HWND hwnd);
void OS_run_command(TCHAR *cmd);
void OS_run_executable(TCHAR *exe);
HINSTANCE OS_current_module_handle();
bool OS_newer_than_Vista();

#define LANGID_Primary LOWORD
#define LANGID_Device HIWORD

#define HK_0 32000

#ifdef __MINGW32__
# define EH_CALLBACK
#else
# define EH_CALLBACK CALLBACK
#endif // __MINGW32__

#endif // STDAFX_H_INCLUDED

