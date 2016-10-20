/*
 * main.c -- call all the initialization routines,
 * bind default keys,
 * call processing of command line arguments,
 * then start message loop.
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

void main_loop(void) {
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

void read_main_config_file(void);
void read_lang_config_files(void);

int main(int argc, char *argv[]) {
    UserProfileDir = OS_user_profile_directory();
    ConfigDir = str_concat_path(UserProfileDir, ProgramName, NULL);
    printf("cfgdir:%s|\n", ConfigDir);
    ProgramDir = OS_program_directory();
    printf("exedir:%s|\n", ProgramDir);

    #ifndef NOGUI
    InitCommonControls();
    #endif

    KN_init();
    KL_init();
    KA_init();
    KR_init();
    DK_init();

    read_main_config_file();
    parse_args(argc, argv, 1);

    KL_activate_lang(LANG_NEUTRAL);
    puts("LM_init locales...");
    LM_get_locales(true);
    read_lang_config_files();
    puts("LM_init activate...");
    LM_activate_selected_locale();
    KL_activate();
    KR_hash_the_titles();
    KR_hash_the_wndcls();
    KR_activate();
    KL_define_vks();

    Sleep(1);
    EH_activate();
    
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
    path = str_concat_path(ProgramDir, "config/hjkl.txt", NULL);
    printf("default main cfg:%s|\n", path);
    f = fopen(path, "rb");
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
    char *path = str_concat_path(ProgramDir, "config", DefaultKeydefsFilename, NULL);
    read_keydefs_file(path);
    free(path);
}

void read_default_compose_file(void) {
    char *path = str_concat_path(ProgramDir, "config", DefaultComposeFilename, NULL);
    read_compose_file(path);
    free(path);
}

void read_main_config_file(void) {
    char *path;
    FILE *f;
    puts("read main cfg...");
    path = str_concat_path(ConfigDir, "config.txt", NULL);
    printf("main cfg:%s|\n", path);
    f = fopen(path, "rb");
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
    path = str_concat_path(ProgramDir, "config", lang_config_filename(lang), NULL);
    printf("default lang %04x cfg:%s|\n", lang, path);
    f = fopen(path, "rb");
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
            FILE *f = fopen(path, "rb");
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
