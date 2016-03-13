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
#include "lm.h"

#ifndef NOGUI
# include "ui.h"
#endif // NOGUI

char *default_argv[] = {
    #define s (char*)

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
        result = GetMessage(&msg, NULL, 0, 0);
        if (result <= 0)
            break;
        UINT id;
        switch (msg.message) {
        case WM_HOTKEY:
            id = (UINT) msg.wParam;
            printf("HK %02d (%s); ", (id - HK_0), HK_to_s(LOWORD(msg.lParam), HIWORD(msg.lParam)));
            HK_KA_call(id);
            break;
        }
    }
}

char *UserProfileDir;
char *ConfigDir;
char *current_parsing_directory;
char *ProgramDir;

void read_main_config_file();
void read_lang_config_files();

int main(int argc, char *argv[]) {
    UserProfileDir = OS_user_profile_directory();
    ConfigDir = str_concat_path(UserProfileDir, ProgramName, NULL);
    printf("cfgdir:%s|\n", ConfigDir);
    ProgramDir = OS_program_directory();
    printf("exedir:%s|\n", ProgramDir);

    KN_init();
    KL_init();
    KA_init();
    KR_init();
    DK_init();

    read_main_config_file();
    parse_args(lenof(default_argv), default_argv, 0);
    parse_args(argc, argv, 1);

    KL_activate_lang(LANG_NEUTRAL);
    puts("LM_init locales...");
    LM_get_locales(true);
    read_lang_config_files();
    puts("LM_init activate...");
    LM_activate_selected_locale();
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

#ifdef NOGUI
# define UI_ask_for_creating_config_file(...) 0
# define UI_maybe_show_errors_for_config_file(...)
#endif

void read_default_main_config_file(void) {
    char *path;
    FILE *f;
    puts("read default main cfg...");
    current_parsing_directory = ProgramDir;
    path = str_concat_path(ProgramDir, "config.txt", NULL);
    printf("default main cfg:%s|\n", path);
    f = fopen(path, "r");
    if (f) {
        printf("reading config file |%s|...\n", path);
        char *s = fread_to_eof(f, '\n');
        parse_str(s);
        UI_maybe_show_errors_for_config_file(path);
        free(s);
        fclose(f);
    } else {
        printf("no config file |%s| found\n", path);
    }
    free(path);
}

void read_keydefs_file(char *path) {
    printf("reading keysym file |%s|...\n", path);
    DK_read_keydef_file(path);
    KA_update_dk_names();
}

void read_compose_file(char *path) {
    printf("reading compose file |%s|...\n", path);
    DK_read_compose_file(path);
}

void read_default_keydefs_file(void) {
    char *path = str_concat_path(ProgramDir, DefaultKeydefsFilename, NULL);
    read_keydefs_file(path);
    free(path);
}

void read_default_compose_file(void) {
    char *path = str_concat_path(ProgramDir, DefaultComposeFilename, NULL);
    read_compose_file(path);
    free(path);
}

void read_main_config_file(void) {
    char *path;
    FILE *f;
    puts("read main cfg...");
    path = str_concat_path(ConfigDir, "config.txt", NULL);
    printf("main cfg:%s|\n", path);
    f = fopen(path, "r");
    if (f) {
        printf("reading config file |%s|...\n", path);
        char *s = fread_to_eof(f, '\n');
        bool nodefault = parse_str_has_nodefault(s);
        if (!parse_nodefault_keydefs) { read_default_keydefs_file(); }
        if (!parse_nodefault_compose) { read_default_compose_file(); }
        if (!nodefault) { read_default_main_config_file(); }
        current_parsing_directory = ConfigDir;
        parse_str(s);
        UI_maybe_show_errors_for_config_file(path);
        free(s);
        fclose(f);
    } else {
        printf("no config file |%s| found\n", path);
        if (UI_ask_for_creating_config_file(path)) {
            read_main_config_file();
        } else {
            read_default_keydefs_file();
            read_default_compose_file();
            read_default_main_config_file();
        }
    }
    free(path);
}

char *lang_config_filename(LANGID lang) {
    static char filename[16];
    snprintf(filename, sizeof(filename)-1, "lang_%04x.txt", lang);
    return filename;
}

void read_default_config_file_for_lang(LANGID lang) {
    char *path;
    FILE *f;
    printf("read default lang %04x cfg...", lang);
    current_parsing_directory = ProgramDir;
    path = str_concat_path(ProgramDir, lang_config_filename(lang), NULL);
    printf("default lang %04x cfg:%s|\n", lang, path);
    f = fopen(path, "r");
    if (f) {
        printf("reading config file |%s|...\n", path);
        char *s = fread_to_eof(f, '\n');
        parse_str(s);
        UI_maybe_show_errors_for_config_file(path);
        free(s);
        fclose(f);
    } else {
        printf("no config file |%s| found\n", path);
    }
    free(path);
}

void read_lang_config_files(void) {
    ssize_t i, maxi = LM_locales.count;
    if (maxi > 0) {
        fori (i, 0, maxi) {
            LM_Locale loc = LM_locales.elts[i];
            LANGID lang = loc.lang;
            char *path = str_concat_path(ConfigDir, lang_config_filename(lang), NULL);
            FILE *f = fopen(path, "r");
            if (f) {
                printf("reading lang config file |%s| ...\n", path);
                char *s = fread_to_eof(f, '\n');
                if (!parse_str_has_nodefault(s)) {
                    read_default_config_file_for_lang(lang);
                    current_parsing_directory = ConfigDir;
                }
                parse_str(s);
                UI_maybe_show_errors_for_config_file(path);
                free(s);
                fclose(f);
            } else {
                printf("no lang config file |%s| found\n", path);
                read_default_config_file_for_lang(lang);
            }
            free(path);
        }
    }
}

void restart_the_program(void) {
    TCHAR buf[MAX_PATH];
    GetModuleFileName(0, buf, lenof(buf));
    lastof(buf) = '\0';
    OS_run_executable(buf);
    exit(0);
}
