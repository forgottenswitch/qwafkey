#ifndef HK_H_INCLUDED
#define HK_H_INCLUDED

#include "stdafx.h"

typedef enum _HKs {
    HK_0 = 32000,
    HK_ONOFF,
    HK2,
} HKs;

extern char *HK_to_s(UINT mods, VK vk);
extern void HK_register(int id, UINT mods, VK vk);

#endif // HK_H_INCLUDED
