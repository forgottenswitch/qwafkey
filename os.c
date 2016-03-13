#include "stdafx.h"
#include "shellapi.h"

/* os.c -- various WinAPI-like functions
 * there is no os.h, as prototypes for these functions reside in stdafx.h
 * */

#ifndef MAPVK_VK_TO_VSC
# define MAPVK_VK_TO_CHAR 2
# define MAPVK_VK_TO_VSC 0
# define MAPVK_VK_TO_VSC_EX 4
# define MAPVK_VSC_TO_VK 1
# define MAPVK_VSC_TO_VK_EX 3
#endif // MAPVK_VK_TO_CHAR

HINSTANCE OS_current_module_handle() {
    return GetModuleHandle(0);
}

KP OS_wchar_to_sc(WCHAR w) {
    KP ret;
    SHORT vks = VkKeyScan((TCHAR)w);
    VK vk = LOBYTE(vks);
    ret.mods = HIBYTE(vks);
    printf("vksc{/%d,%02x}", ret.mods, vk);
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
    printf("Error %ld: %s\n", error_code, (char*)lpMsgBuf);
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

char *OS_get_window_class(HWND hwnd) {
  static char buf[256];
  GetClassName(hwnd, buf, sizeof(buf));
  lastof(buf) = '\0';
  return buf;
}

void OS_run_command(TCHAR *cmd) {
    printf("cmd|%s| ", cmd);
    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(STARTUPINFO));
    ZeroMemory(&pi, sizeof(PROCESS_INFORMATION));
    CreateProcess(NULL, cmd, 0, 0, 0, 0, 0, NULL, &si, &pi);
}

void OS_run_executable(TCHAR *exe) {
    printf("exe|%s| ", exe);
    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(STARTUPINFO));
    ZeroMemory(&pi, sizeof(PROCESS_INFORMATION));
    CreateProcess(exe, NULL, 0, 0, 0, 0, 0, NULL, &si, &pi);
}

bool OS_newer_than_Vista() {
    return false;
}

char *OS_user_profile_directory(void) {
    return getenv("USERPROFILE");
}

char *OS_program_directory(void) {
    char *s = _pgmptr;
    char *s1 = strrchr(s, '\\');
    if (!s1) { s1 = s + strlen(s); }
    int l = s1 - s;
    char *s2 = malloc(l+1);
    int i;
    fori (i, 0, l) { s2[i] = s[i]; }
    s2[l] = 0;
    return s2;
}
