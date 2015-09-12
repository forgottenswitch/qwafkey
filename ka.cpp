#include "ka.h"
#include "lm.h"
#include "kl.h"
#include "kr.h"
#include "scancodes.h"

#define ka(name) void name(KA_PARAMS)

ka(KA_toggle) {
    if (!down)
        return;
    KL_toggle();
}

ka(KA_next_layout) {
    if (!down)
        return;
    LM_activate_next_locale();
}

ka(KA_prev_layout) {
    if (!down)
        return;
    LM_activate_prev_locale();
}

ka(KA_next_os_layout) {
    if (!down)
        return;
    OS_activate_next_layout(LM_hwnd);
}

ka(KA_prev_os_layout) {
    if (!down)
        return;
    OS_activate_prev_layout(LM_hwnd);
}

ka(KA_control) {
    KM_shift_event(&KL_km_control, down, sc);
    if (!(!down && KL_km_control.shifts_count > 0))
        keybd_event(VK_LCONTROL, SC_LCONTROL, (down ? 0 : KEYEVENTF_KEYUP), 0);
}

ka(KA_l5_shift) {
    KL_phys_mods[sc] = KLM_TEMP;
    KM_shift_event(&KL_km_l5, down, sc);
}

ka(KA_l5_lock) {
    KM_lock_event(&KL_km_l5, down, sc);
}

ka(KA_l3_latch) {
    KM_latch_event(&KL_km_l3, down, sc);
}

ka(KA_l2_latch) {
    KL_km_shift.latch_faked = VK_LSHIFT;
    KM_latch_event(&KL_km_shift, down, sc);
}

ka(KA_kr_toggle) {
    if (!down)
        return;
    KR_toggle();
}

ka(KA_kr_toggle_clear) {
    if (!down)
        return;
    KR_toggle_clear();
}

ka(KA_dim_screen) {
    if (!down)
        return;
    dputs("dim_screen");
    Sleep(500);
    SendMessage(HWND_BROADCAST, WM_SYSCOMMAND, SC_MONITORPOWER, (LPARAM)2);
}

#undef ka

typedef struct {
    KA_FUNC func;
    char* name;
} KA_Pair;

#define ka(name) { name, #name }
KA_Pair KA_fns[] = {
    ka(KA_toggle),
    ka(KA_next_layout),
    ka(KA_prev_layout),
    ka(KA_next_os_layout),
    ka(KA_prev_os_layout),
    ka(KA_control),
    ka(KA_l5_shift),
    ka(KA_l5_lock),
    ka(KA_l3_latch),
    ka(KA_l2_latch),
    ka(KA_kr_toggle),
    ka(KA_kr_toggle_clear),
    ka(KA_dim_screen),
};
#undef ka

int KA_call(UINT id, KA_PARAMS) {
    if (id >= len(KA_fns))
        return -1;
    KA_fns[id].func(down, sc);
    return 0;
}

int KA_name_to_id(char *name) {
    UINT i;
    fori (i, 0, len(KA_fns)) {
        KA_Pair *p = KA_fns + i;
        if (!strcmp(p->name, name)) {
            return i;
        }
    }
    return -1;
}

void KA_init() {
    UINT i;
    fori (i, 0, len(KA_fns)) {
        KA_Pair *ka_pair = KA_fns + i;
        ka_pair->name += 3;
        dput(" ka%d{%x,%s}", i, (UINT)ka_pair->func, ka_pair->name);
    }
    dputs("");
}
