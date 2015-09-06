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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "resource.h"

#define nil NULL
#define fori(i, from, to) for (i = from; i < to; i++)
#define len(ary) (sizeof((ary))/sizeof((ary)[0]))
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

typedef unsigned char VK;
typedef unsigned short SC;

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

#define KLM_L3 16
#define KLM_L5 32

#define KLM_TEMP 16

/*

.mods field: bitwise mask
  MOD_ALT 1
  MOD_CONTROL 2
  MOD_SHIFT 4
  KLM_WCHAR 16 - for sending characters
   not sendable by a single keypress
  KLM_MOD - the bound key is a modifier

.binding field: either virtual key-code or WCHAR

*/
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
bool OS_is_console(HWND hwnd);

#define LANGID_Primary LOWORD
#define LANGID_Device HIWORD

#define HK_0 32000

#endif // STDAFX_H_INCLUDED

