/*
 * main.c -- call all the initialize functions,
 * bind default keys,
 * call processing of command line arguments,
 * then start message loop
 * */


#include "stdafx.h"
#include "parse.h"
#include "hk.h"
#include "kl.h"
#include "eh.h"
#include "lm.h"
#include "ka.h"
#include "kn.h"
#include "kr.h"
#include "dk.h"

#ifndef NOGUI
# include "ui.h"
#endif // NOGUI

char *default_argv[] = {
    #define s (char*)

    s"keysym_file keysymdef.h",
    s"compose_file Compose",

    s"@#F2:!toggle",
    s"@#F3:!restart",
    s"@#qZ:!dim_screen",
    s"@#qX:!kr_toggle",
    s"@#qO:!toggle_on_top",
    s"@#Enter:!msys2_shell",
    s"@#qC:!lbutton_down",

    s"levels:1,2,3,4,5,6",
    s"sc027:!l5_shift",
    s"sc03A:!control",
    s"space:= ",

    s"level:1",
    s"qApostrophe:=,,=:",

    s"level:5",

    s"qH:left",
    s"qJ:down",
    s"qK:up",
    s"qL:right",
    s"qU:Backspace",
    s"qO:Delete",
    s"qI:Escape",
    s"qY:pgup",
    s"qN:pgdn",
    s"qM:Enter",
    s"qComma:Home",
    s"qPeriod:End",

    s"qS:!l2_latch",

    s"q1:=&",
    s"q2:=+",
    s"q3:=<",
    s"q4:=>",
    s"q5:=-,=_",
    s"q6:=^",
    s"q7:u2018,u201C",
    s"q8:Insert",
    s"q9:Menu",
    s"q0:u2019,u201D",

    s"qQ:=!,=?",
    s"qW:=:,=,",
    s"qE:=(,=@",
    s"qR:==,=#",
    s"qT:Tab",
    s"qA:=/,=`",
    s"qD:={,=}",
    s"qF:=[,=]",
    s"qG:=),=*",
    s"qZ:=%",
    s"qX:=$,=~",
    s"qC:=\\,=|",
    s"qV:=\",='",
    s"qB:=;,=`",

    s"Space:= ,:= ",

    s"levels:1,2,3,4",
    s"qComma:!l3_latch",

    s"level:3",
    s"qJ:=_",
    s"qX:=~,=$",
    s"qC:=\\,=|",
    s"qB:=;,=,",
    s"qM:!next_layout",
    s"qSlash:!next_os_layout",
    s"qN:!compose",
    s"qU:!dead_diaeresis",

    s"lang:409",
    s"levels:1,2",
    s"qE:=a,=A",
    s"qR:=f,=F",
    s"qT:=g,=G",
    s"qA:=v,=V",
    s"qD:=e,=E",
    s"qF:=r,=R",
    s"qG:=t,=T",
    s"qV:=d,=D",
    s"qI:=o,=O",
    s"qO:=p,=P",
    s"qP:=k,=K",
    s"qJ:=n,=N",
    s"qK:=i,=I",
    s"qN:=j,=J",
    s"vks_lang:409",

    s"lang:419",
    s"level:1",
    s"qQ:u0447,u0427",
    s"qW:u0448,u0428",
    s"qE:u0430,u0410",
    s"qR:u0432,u0412",
    s"qT:u0433,u0413",
    s"qY:u0439,u0419",
    s"qU:u0443,u0423",
    s"qI:u043e,u041e",
    s"qO:u043f,u041f",
    s"qP:u044b,u042b",
    s"qA:u044f,u042f",
    s"qS:u0441,u0421",
    s"qD:u0435,u0415",
    s"qF:u0440,u0420",
    s"qG:u0442,u0422",
    s"qH:u043a,u041a",
    s"qJ:u043d,u041d",
    s"qK:u0438,u0418",
    s"qL:u043b,u041b",
    s"qZ:u0437,u0417",
    s"qX:u0436,u0416",
    s"qC:u0446,u0426",
    s"qV:u0434,u0414",
    s"qB:u0431,u0411",
    s"qN:u044c,u042c",
    s"qM:u043c,u041c",
    s"qLBracket:=[,={",
    s"qRBracket:=],=}",
    s"qPeriod:=.,=>",
    s"qSlash:=/,=?",
    s"qTilde:=`,=~",
    s"level:3",
    s"qW:u0449,u0429",
    s"qR:u0444,u0424",
    s"qU:u044e,u042e",
    s"qP:u044a,u042a",
    s"qD:u0451,u0431",
    s"qA:u044d,u042d",
    s"qH:u0445,u0425",
    s"level:2",
    s"qBackslash:=|",
    s"q1:=!",
    s"q2:=@",
    s"q3:=#",
    s"q4:=$",
    s"q5:=%",
    s"q6:=^",
    s"q7:=&",
    s"q8:=*",
    s"q9:=(",
    s"q0:=)",

    s"class Photo_Lightweight_Viewer",
    s"qSlash=!kr_off",
    s"qPeriod=!kr_on_pt",
    s"qApostrophe=!close_window",
    s"qJ=Right",
    s"qO=Left",
    s"qY=!left10",
    s"qU=!right10",

    s"res 1920 1080",
    s"title DEAD OR ALIVE 5",
    s"qE=qM",
    s"qJ=qA",
    s"qI=qW",
    s"qL=qD",
    s"qK=qS",
    s"qS=qL",
    s"qD=qK",
    s"Space=qJ",
    s"qW=qI",
    s"qA=qU",
    s"qC=qO",
    s"qX=qN",

    s"title DARK SOULS",
    s"q7=vk_Left",
    s"q8=vk_Down",
    s"q9=vk_Up",
    s"q0=vk_Right",
    s"q1=vk_Insert",
    s"q2=vk_Delete",
    s"q5=vk_PgUp",
    s"q6=vk_PgDown",

    s"class ZDoomMainWindow",
    s"F2=!zdoom_quicksave",
    s"F3=!zdoom_quickload",

    s"title TITAN SOULS",
    s"qJ=Left",
    s"qK=Down",
    s"qI=sc48",
    s"qL=vk_Right",
    s"qA=qZ",
    s"qW=qX",
    s"qD=qC",
    s"qM=Enter",
    s"res 1024 768",

    #undef s
};

void main_loop() {
    for (;;) {
        int result;
        MSG msg;
        result = GetMessage(&msg, nil, 0, 0);
        if (result <= 0)
            break;
        UINT id;
        switch (msg.message) {
        case WM_HOTKEY:
            id = (UINT) msg.wParam;
            dput("HK %02d (%s); ", (id - HK_0), HK_to_s(LOWORD(msg.lParam), HIWORD(msg.lParam)));
            HK_KA_call(id);
            break;
        }
    }
}

char *UserProfileDir;
char *ConfigDir;
char *ProgramDir;

int main(int argc, char *argv[]) {
    UserProfileDir = OS_user_profile_directory();
    ConfigDir = str_concat_path(UserProfileDir, ProgramName, NULL);
    dput("cfgdir:%s|\n", ConfigDir);
    ProgramDir = OS_program_directory();
    dput("exedir:%s|\n", ProgramDir);

    KN_init();
    KL_init();
    KA_init();
    KR_init();
    DK_init();

    parse_args(len(default_argv), default_argv, 0);
    parse_args(argc, argv, 1);

    KL_activate_lang(LANG_NEUTRAL);
    LM_init();
    KL_activate();
    EH_activate();
    KR_activate();
    KL_define_vks();

#ifndef NOGUI
    UI_init();
    atexit(UI_TR_delete);
    UI_spawn();
#endif // NOGUI

    main_loop();
    return 0;
}
