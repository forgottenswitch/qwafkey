#include "stdafx.h"
#include "parse.h"
#include "hk.h"
#include "kl.h"
#include "eh.h"
#include "lm.h"
#include "ka.h"
#include "kn.h"

char *default_argv[] = {
    "levels:123456",
    "sc027:!l5_shift",
    "sc03A:!control",
    "levels:56",
    "sc026:vk25",
    "sc025:up",
    "qJ:down",
    "sc004:=<",
    "levels:12",
    "sc002:sc003",
    "lang:409",
    "sc003:sc004",
    "lang:419",
    "sc004:sc005",
};

int main(int argc, char *argv[]) {
    KN_init();
    KL_init();
    KA_init();
    parse_args(len(default_argv), default_argv);
    parse_args(argc-1, argv+1);

    KL_activate_lang(LANG_NEUTRAL);
    LM_init();

    KL_activate();
    HK_register(HK_ONOFF, MOD_WIN, VK_F2);

    EH_activate();

    for (;;) {
        int result;
        MSG msg;
        result = GetMessage(&msg, nil, 0, 0);
        if (result <= 0)
            break;
        switch (msg.message) {
        case WM_HOTKEY:
            VK vk = HIWORD(msg.lParam);
            WORD mods = LOWORD(msg.lParam);
            int id = (int) msg.wParam;
            printf("HK %02d (%s); ", (id - HK_0), HK_to_s(mods, vk));
            switch (id) {
            case HK_ONOFF:
                if (KL_active) {
                    KL_deactivate();
                    puts("suspended");
                } else {
                    KL_activate();
                    puts("resumed");
                }
                break;
            }
            break;
        }
    }

    return 0;
}
