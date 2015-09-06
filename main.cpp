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

    "levels:1,2,3,4,5,6",
    "sc027:!l5_shift",
    "sc03A:!control",

    "level:5",

    "qH:left",
    "qJ:down",
    "qK:up",
    "qL:right",
    "qU:Backspace",
    "qO:Delete",
    "qI:Escape",
    "qY:pgup",
    "qN:pgdn",
    "qM:Enter",
    "qComma:Home",
    "qPeriod:End",

    "qS:!l2_latch",

    "q1:=&",
    "q2:=+",
    "q3:=<",
    "q4:=>",
    "q5:=-",
    "q6:=^",
    "q7:u2018",
    "q8:Insert",
    "q9:Menu",
    "q0:u2019",

    "qQ:=!",
    "qW:=:",
    "qE:=(",
    "qR:==",
    "qT:Tab",
    "qA:=/",
    "qD:={",
    "qF:=[",
    "qG:=)",
    "qZ:=%",
    "qX:=$",
    "qC:=\\",
    "qV:=\"",
    "qB:=;",

    "level:6",
    "q5:=_",
    "q7:u201C",
    "q0:u201D",
    "qQ:=,",
    "qW:=?",
    "qE:=@",
    "qR:=#",
    "qT:Tab",
    "qA:=,",
    "qD:=}",
    "qF:=]",
    "qG:=*",
    "qZ:=%",
    "qX:=~",
    "qC:=|",
    "qV:='",
    "qB:=`",

    "level:1",
    "qComma:!l3_latch",

    "level:3",
    "qJ:=_",
    "qM:!next_layout",
    "qSlash:!next_os_layout",

    "lang:409",
    "levels:1,2",
    "qD:=e",

    "lang:419",
    "level:1",
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
    parse_args(len(default_argv), default_argv, 0);
    parse_args(argc, argv, 1);

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
