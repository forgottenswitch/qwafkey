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

void OS_print_last_error() {
    DWORD error_code = GetLastError();
    LPVOID lpMsgBuf;
    FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |
                  FORMAT_MESSAGE_FROM_SYSTEM |
                  FORMAT_MESSAGE_IGNORE_INSERTS,
                  NULL,
                  error_code,
                  MAKELANGID(LANG_ENGLISH, SUBLANG_DEFAULT),
                  (LPTSTR) &lpMsgBuf,
                  0, NULL );
    printf("Error %d: %s\n", error_code, lpMsgBuf);
    LocalFree(lpMsgBuf);
}
