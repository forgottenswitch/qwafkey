#include "stdafx.h"
#include "parse.h"
#include "hk.h"
#include "kl.h"
#include "eh.h"
#include "lm.h"
#include "ka.h"
#include "kn.h"

#ifndef NOGUI
# include "ui.h"
#endif // NOGUI

char *default_argv[] = {
    "@#F3:!toggle",
    "@#qZ:!dim_screen",

    "levels:123456",
    "sc027:!l5_shift",
    "sc03A:!control",

    "levels:56",
/*
    "qH:left",
    "qJ:down",
    "qK:up",
    "qL:right",
    "qU:Backspace",
    "qO:Delete",
    "qI:Escape",
    "qY:pgup",
    "qN:pgdn",
    "qM:Enter"
    "qComma:Home",
    "qPeriod:End",
    "q8:Insert",
    "q9:Menu",
*/
    "sc004:=<",
    "qS:!l2_latch",
    "qF:=[",

    "levels:6",
    "qF:=]",

    "levels:12",
    "qComma:!l3_latch",
    "sc002:sc003",

    "levels:34",
    "qJ:=_",
    "qM:!next_layout",
    "qSlash:!next_os_layout",

    "lang:409",
    "sc003:sc004",

    "lang:419",
    "sc004:sc005",

};

void main_loop() {
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
            UINT id = (UINT) msg.wParam;
            dput("HK %02d (%s); ", (id - HK_0), HK_to_s(mods, vk));
            HK_KA_call(id);
            break;
        }
    }
}

int main(int argc, char *argv[]) {
    KN_init();
    KL_init();
    KA_init();
    parse_args(len(default_argv), default_argv);
    parse_args(argc-1, argv+1);

    KL_activate_lang(LANG_NEUTRAL);
    LM_init();

    KL_activate();

    EH_activate();

#ifndef NOGUI
    UI_init();
    atexit(UI_TR_delete);
    UI_spawn();
#endif // NOGUI

    main_loop();
    return 0;
}
