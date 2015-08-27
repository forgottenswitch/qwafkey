#include "stdafx.h"

#ifndef MAPVK_VK_TO_VSC
# define MAPVK_VK_TO_CHAR 2
# define MAPVK_VK_TO_VSC 0
# define MAPVK_VK_TO_VSC_EX 4
# define MAPVK_VSC_TO_VK 1
# define MAPVK_VSC_TO_VK_EX 3
#endif // MAPVK_VK_TO_CHAR

KP OS_tchar_to_vk(TCHAR tc, HKL hkl) {
    KP ret;
    SHORT vks = VkKeyScanEx(tc, hkl);
    VK vk = LOBYTE(vks);
    ret.mods = HIBYTE(vks);
    ret.vk = MapVirtualKeyEx(vk, MAPVK_VK_TO_VSC, hkl);
    return ret;
}

VK OS_sc_to_vk(SC sc, HKL hkl) {
    return MapVirtualKeyEx(sc, MAPVK_VSC_TO_VK, hkl);
}
