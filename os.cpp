#include "stdafx.h"
#include "shellapi.h"

#ifndef MAPVK_VK_TO_VSC
# define MAPVK_VK_TO_CHAR 2
# define MAPVK_VK_TO_VSC 0
# define MAPVK_VK_TO_VSC_EX 4
# define MAPVK_VSC_TO_VK 1
# define MAPVK_VSC_TO_VK_EX 3
#endif // MAPVK_VK_TO_CHAR

KP OS_wchar_to_sc(WCHAR w) {
    KP ret;
    SHORT vks = VkKeyScan((TCHAR)w);
    VK vk = LOBYTE(vks);
    ret.mods = HIBYTE(vks);
    dput("vksc{/%d,%02x}", ret.mods, vk);
    ret.sc = MapVirtualKey(vk, MAPVK_VK_TO_VSC);
    return ret;
}

KP OS_wchar_to_vk(WCHAR w) {
    KP ret;
    SHORT vks = VkKeyScanW(w);
    ret.vk = LOBYTE(vks);
    BYTE modsVKS = HIBYTE(vks);
    ret.mods = ((modsVKS & 4)>>2) + (modsVKS & 2) + ((modsVKS & 1)<<2);
    return ret;
}

VK OS_sc_to_vk(SC sc) {
    return MapVirtualKey(sc, MAPVK_VSC_TO_VK);
}

SC OS_vk_to_sc(SC sc) {
    return MapVirtualKey(sc, MAPVK_VK_TO_VSC);
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
    dput("Error %ld: %s\n", error_code, (char*)lpMsgBuf);
    LocalFree(lpMsgBuf);
}

void OS_activate_layout(HWND hwnd, HKL hkl) {
    PostMessage(hwnd, WM_INPUTLANGCHANGEREQUEST, 0, (LPARAM)hkl);
}

#define INPUTLANGCHANGE_FORWARD 0x0002
#define INPUTLANGCHANGE_BACKWARD 0x0004
void OS_activate_next_layout(HWND hwnd) {
    PostMessage(hwnd, WM_INPUTLANGCHANGEREQUEST, INPUTLANGCHANGE_FORWARD, 0);
}

void OS_activate_prev_layout(HWND hwnd) {
    PostMessage(hwnd, WM_INPUTLANGCHANGEREQUEST, INPUTLANGCHANGE_BACKWARD, 0);
}

bool OS_is_console(HWND hwnd) {
  char winclass[256];
  GetClassName(hwnd, winclass, sizeof(winclass));
  return !strncmp(winclass, "ConsoleWindowClass", sizeof(winclass) - 1);
}
