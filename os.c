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
    static size_t alloc = 512;
    static char *s = NULL;
    if (!s) { s = malloc(alloc); }
    while (1) {
         GetModuleFileName(NULL, s, alloc);
         if (str_fills(s, alloc)) { break; }
         s = realloc(s, alloc *= 1.5);
    }
    char *s1 = strrchr(s, '\\');
    if (!s1) { s1 = s + strlen(s); }
    int l = s1 - s;
    char *s2 = malloc(l+1);
    int i;
    fori (i, 0, l) { s2[i] = s[i]; }
    s2[l] = 0;
    return s2;
}

char *OS_keyboard_layout_name(LANGID lang) {
    static BYTE buf2[256];
    char buf[256], lang_str[16];
    DWORD buf_len, buf2_len;
    snprintf(lang_str, sizeof(lang_str)-1, "%08x", lang);
    HKEY hkey;
    char *subkey = str_concat_path("SYSTEM\\CurrentControlSet\\Control\\Keyboard Layouts", lang_str, NULL);
    if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, subkey, 0, KEY_QUERY_VALUE, &hkey) != ERROR_SUCCESS) { 
        printf("failed to open |%s|\n", subkey);
        goto ret_null;
    }
    long text_idx = -1;
    DWORD i, l;
    RegQueryInfoKey(hkey, NULL, NULL, NULL, NULL, NULL, NULL, &l, NULL, NULL, NULL, NULL);
    fori (i, 0, l) {
        buf_len = lenof(buf);
        if (RegEnumValue(hkey, i, buf, &buf_len, NULL, NULL, NULL, NULL) != ERROR_SUCCESS) { continue; }
        //printf("%d/%d in |%s|: |%s|\n", i+1, l, subkey, buf);
        if (!stricmp(buf, "Layout Text")) { text_idx = i; break; }
    }
    if (text_idx < 0) { printf("no Layout Text in |%s|\n", subkey); goto ret_null; }
    buf_len = lenof(buf); buf2_len = lenof(buf2);
    RegEnumValue(hkey, (DWORD)text_idx, buf, &buf_len, NULL, NULL, buf2, &buf2_len);
    free(subkey);
    return (char*)buf2;
  ret_null:
    free(subkey);
    return NULL;
}

int OS_ToUnicodeThroghVkKeyScan(void *cache, VK vk, BOOL shift_down) {
    INT i;
    //INT ret;
    shift_down = !!shift_down;

    /* for all the UCS-2 codepoints ... */
    for (i = 0; i <= 0xffff; i++) {
        SHORT i_x;
        BYTE i_vk, i_mods;
        BOOL i_shift_down, i_ctrl_down;

        i_x = ((SHORT*)cache)[i];
        i_vk = LOBYTE(i_x);
        i_mods = HIBYTE(i_x);
        i_shift_down = i_mods & 1;
        i_ctrl_down = !!(i_mods & 2);

        if (0) {
            BYTE i_char = (char) i;
            if (i_char != i)
                i_char = 0;
            printf(" %04x '%c' => vk%02x vk'%c' shift:%d\n", i, i_char, i_vk, i_vk, i_shift_down);
        }

        if (i_vk == vk && i_shift_down == shift_down && !i_ctrl_down) {
            // ret = i;
            // puts(">>>");
            return i;
        }
    }

    return 0;
}

void *OS_ToUnicodeThroghVkKeyScan_new_cache(void) {
    SHORT *ret;
    INT i;

    ret = malloc(0x10000 * sizeof(SHORT));

    /* for all the UCS-2 codepoints ... */
    for (i = 0; i <= 0xffff; i++) {
        ret[i] = VkKeyScanW(i);
    }

    return (void*) ret;
}
