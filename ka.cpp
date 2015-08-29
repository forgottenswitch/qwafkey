#include "ka.h"
#include "lm.h"
#include "kl.h"

#define ka(name) void name(KA_PARAMS)

ka(KA_next_layout) {
    if ((LM_selected_locale += 1) >= LM_locales.count)
        LM_selected_locale = 0;
    LM_activate_selected_locale();
}

ka(KA_prev_layout) {
    if ((LM_selected_locale -= 1) >= 0)
        LM_selected_locale = LM_locales.count;
    LM_activate_selected_locale();
}

ka(KA_control) {
    keybd_event(VK_LCONTROL, 0x01D, (down ? 0 : KEYEVENTF_KEYUP), 0);
}

ka(KA_l5_shift) {
    if (down) {
        KL_mods |= KLM_L5;
    } else {
        printf("/%d,%d,%d,%d ", KL_mods, KL_mods ^ KLM_L5, KL_mods & KLM_L5, KL_mods | KLM_L5);
        KL_mods ^= KLM_L5;
    }
}

ka(KA_l5_lock) {
    if (down) {
        KL_mods ^= KLM_L5;
    }
}

#undef ka

typedef struct {
    KA_FUNC func;
    char* name;
} KA_Pair;

#define ka(name) { name, #name }
KA_Pair KA_fns[] = {
    ka(KA_next_layout),
    ka(KA_prev_layout),
    ka(KA_control),
    ka(KA_l5_shift),
    ka(KA_l5_lock),
};
#undef ka

int KA_call(UINT id, KA_PARAMS) {
    if (id >= len(KA_fns))
        return -1;
    KA_fns[id].func(down);
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
        printf(" ka%d{%x,%s}", i, (UINT)ka_pair->func, ka_pair->name);
    }
    puts("");
}
