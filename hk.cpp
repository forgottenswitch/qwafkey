#include "hk.h"
#include "ka.h"
#ifndef NOGUI
#include "ui.h"
#endif // NOGUI

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
    dput("Register HK%d+%02d (%s): ", HK_0, (id - HK_0), HK_to_s(mods, vk));
    int ret = RegisterHotKey(nil, id, mods, vk);
    if (ret == 1) {
        dput("OK ");
    } else {
        OS_print_last_error();
    }
}

size_t HK_KA_curid = HK_0;

int HK_KA_ary[255];

bool HK_KA_put_pair(UINT hk_id, int ka_id) {
    size_t idx = hk_id - HK_0;
    if (idx < len(HK_KA_ary)) {
        HK_KA_ary[idx] = ka_id;
        return true;
    }
    return false;
}

void HK_KA_register(int ka, UINT mods, VK vk) {
    HK_KA_curid += 1;
    if (HK_KA_put_pair(HK_KA_curid, ka))
        HK_register(HK_KA_curid, mods, vk);
}

void HK_KA_call(UINT hk_id) {
    UINT ka_idx = hk_id - HK_0, ka_id = HK_KA_ary[ka_idx];
    dput("ka_call %d;", ka_id);
    KA_call(ka_id, true, 0);
    KA_call(ka_id, false, 0);
}

void HK_init() {
    ZeroMemory(HK_KA_ary, sizeof(HK_KA_ary));
}
