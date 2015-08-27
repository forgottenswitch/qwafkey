#ifndef STDAFX_H_INCLUDED
#define STDAFX_H_INCLUDED

#define _WIN32_WINNT 0x0500

#if defined(UNICODE) && !defined(_UNICODE)
    #define _UNICODE
#elif defined(_UNICODE) && !defined(UNICODE)
    #define UNICODE
#endif

#include <tchar.h>
#include <windows.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "resource.h"

#define nil NULL
#define fori(i, from, to) for (i = from; i < to; i++)
#define len(ary) (sizeof(ary)/sizeof(ary[0]))

typedef unsigned char VK;
typedef unsigned short SC;

#define VK_ch(ch) LOBYTE(VkKeyScan(ch))

typedef struct {
    unsigned char mods;
    VK vk;
} KP;

#define KLM_WCHAR 16
#define KLM_MOD 32
#define KLM_SC 64

#define KLM_L3 16
#define KLM_L5 32

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

KP OS_tchar_to_vk(TCHAR tc, HKL hkl);
VK OS_sc_to_vk(SC sc, HKL hkl);

#endif // STDAFX_H_INCLUDED

