#include "stdafx.h"
#include "parse.h"
#include "hk.h"
#include "kl.h"
#include "eh.h"
#include "lm.h"
#include "ka.h"
#include "kn.h"
#include "kr.h"

#ifndef NOGUI
# include "ui.h"
#endif // NOGUI

char *default_argv[] = {
    "@#F2:!toggle",
    "@#F3:!restart",
    "@#qZ:!dim_screen",
    "@#qX:!kr_toggle",
    "@#qO:!toggle_on_top",
    "@#Enter:!msys2_shell",

    "levels:1,2,3,4,5,6",
    "sc027:!l5_shift",
    "sc03A:!control",
    "space:= "

    "level:1",
    "qApostrophe:=,,=:",

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
    "q5:=-,=_",
    "q6:=^",
    "q7:u2018,u201C",
    "q8:Insert",
    "q9:Menu",
    "q0:u2019,u201D",

    "qQ:=!,=?",
    "qW:=:,=,",
    "qE:=(,=@",
    "qR:==,=#",
    "qT:Tab",
    "qA:=/,=`",
    "qD:={,=}",
    "qF:=[,=]",
    "qG:=),=*",
    "qZ:=%",
    "qX:=$,=~",
    "qC:=\\,=|",
    "qV:=\",='",
    "qB:=;,=,",

    "levels:1,2,3,4",
    "qComma:!l3_latch",

    "level:3",
    "qJ:=_",
    "qX:=~,=$",
    "qC:=\\,=|",
    "qB:=;,=,",
    "qM:!next_layout",
    "qSlash:!next_os_layout",

    "lang:409",
    "levels:1,2",
    "qE:=a,=A",
    "qR:=f,=F",
    "qT:=g,=G",
    "qA:=v,=V",
    "qD:=e,=E",
    "qF:=r,=R",
    "qG:=t,=T",
    "qV:=d,=D",
    "qI:=o,=O",
    "qO:=p,=P",
    "qP:=k,=K",
    "qJ:=n,=N",
    "qK:=i,=I",
    "qN:=j,=J",

    "lang:419",
    "level:1",
    "qQ:u0447,u0427",
    "qW:u0448,u0428",
    "qE:u0430,u0410",
    "qR:u0432,u0412",
    "qT:u0433,u0413",
    "qY:u0439,u0419",
    "qU:u0443,u0423",
    "qI:u043e,u041e",
    "qO:u043f,u041f",
    "qP:u044b,u042b",
    "qA:u044f,u042f",
    "qS:u0441,u0421",
    "qD:u0435,u0415",
    "qF:u0440,u0420",
    "qG:u0442,u0422",
    "qH:u043a,u041a",
    "qJ:u043d,u041d",
    "qK:u0438,u0418",
    "qL:u043b,u041b",
    "qZ:u0437,u0417",
    "qX:u0436,u0416",
    "qC:u0446,u0426",
    "qV:u0434,u0414",
    "qB:u0431,u0411",
    "qN:u044c,u042c",
    "qM:u043c,u041c",
    "level:3"
    "qW:u0449,u0429",
    "qR:u0444,u0424",
    "qU:u044e,u042e",
    "qP:u044a,u042a",
    "qD:u0451,u0431",
    "qA:u044d,u042d",
    "qH:u0445,u0425",

    "class Photo_Lightweight_Viewer",
    "qSlash=!kr_off",
    "qPeriod=!kr_on_pt",
    "qApostrophe=!close_window",
    "qJ=Right",
    "qO=Left",
    "qY=!left10",
    "qU=!right10",

    "class CabinetWClass",
    "qSlash=!kr_off",
    "qPeriod=!kr_on_pt",
    "qApostrophe=!close_window",
    "qH=Left",
    "qJ=Down",
    "qK=Up",
    "qL=Right",
    "qI=Enter",
    "qM=Enter",
    "qU=Backspace",
    "Space=PgDn",
    "qB=PgUp",
    "qN=PgDn",
    "qY=PgUp",

    "res 1920 1080",
    "title DEAD OR ALIVE 5",
    "qE=qM",
    "qJ=qA",
    "qI=qW",
    "qL=qD",
    "qK=qS",
    "qS=qL",
    "qD=qK",
    "Space=qJ",
    "qW=qI",
    "qA=qU",
    "qC=qO",
    "qX=qN",

    "title DARK SOULS",
    "q7=vk_Left",
    "q8=vk_Down",
    "q9=vk_Up",
    "q0=vk_Right",
    "q1=vk_Insert",
    "q2=vk_Delete",
    "q5=vk_PgUp",
    "q6=vk_PgDown",

    "class ZDoomMainWindow",
    "F2=!zdoom_quicksave",
    "F3=!zdoom_quickload",
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
            UINT id = (UINT) msg.wParam;
            dput("HK %02d (%s); ", (id - HK_0), HK_to_s(LOWORD(msg.lParam), HIWORD(msg.lParam)));
            HK_KA_call(id);
            break;
        }
    }
}

int main(int argc, char *argv[]) {
    KN_init();
    KL_init();
    KA_init();
    KR_init();

    parse_args(len(default_argv), default_argv, 0);
    parse_args(argc, argv, 1);

    KL_activate_lang(LANG_NEUTRAL);
    LM_init();
    KL_activate();
    EH_activate();
    KR_activate();

#ifndef NOGUI
    UI_init();
    atexit(UI_TR_delete);
    UI_spawn();
#endif // NOGUI

    main_loop();
    return 0;
}
