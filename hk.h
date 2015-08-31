#ifndef HK_H_INCLUDED
#define HK_H_INCLUDED

#include "stdafx.h"

char *HK_to_s(UINT mods, VK vk);
void HK_register(int id, UINT mods, VK vk);

void HK_KA_register(int ka, UINT mods, VK vk);
void HK_KA_call(UINT hk_id);

#endif // HK_H_INCLUDED
