#include "hk.h"

char HK_to_s_buf[255];

char *HK_to_s(UINT mods, VK vk) {
    char *s = HK_to_s_buf;
    ZeroMemory(s, 255);
    #define m(mod, ch) do { if (mods & mod) { s[0]=ch; s[1]='+'; s[2]='\0'; s += 2; }; } while (0)
    m(MOD_CONTROL, 'C');
    m(MOD_ALT, 'A');
    m(MOD_WIN, 'W');
    m(MOD_SHIFT, 'S');
    if ('A' <= vk && vk <= 'Z') {
        s[0] = vk;
        s++;
    } else {
        s += snprintf(s, 128, "vk%03d", vk);
    }
    #undef m
    s = '\0';
    return &(HK_to_s_buf[0]);
}

void HK_register(int id, UINT mods, VK vk) {
    printf("Register HK%d+%02d (%s): ", HK_0, (id - HK_0), HK_to_s(mods, vk));
    int ret = RegisterHotKey(nil, id, mods, vk);
    if (ret == 1) {
        puts("OK");
    } else {
        OS_print_last_error();
    }
}

