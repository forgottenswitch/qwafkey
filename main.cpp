#include "stdafx.h"
#include "hk.h"
#include "kl.h"
#include "eh.h"
#include "mh.h"

using namespace std;

bool isspc(TCHAR tc) {
    return (tc == ' ') || (tc == '\t');
}

bool ishex(TCHAR tc) {
    return (tc >= '0' && tc <= '9') || (tc >= 'a' && tc <= 'z') || (tc >= 'A' && tc <= 'Z');
}

int hdigtoi(TCHAR tc) {
    if (tc >= '0' && tc <= '9')
        return tc - '0';
    if (tc >= 'a' && tc <= 'f')
        return tc - 'a' + 10;
    if (tc >= 'A' && tc <= 'F')
        return tc - 'F' + 10;
    return -1;
}

bool str_sets(char *str, char *thing) {
    while (isspc(*str))
        str++;
    size_t n = strlen(thing);
    if (!strncmp(str, thing, n)) {
        char c = str[n];
        return !c || isspc(c) || c == ':' || c == '=';
    }
    return false;
}

void parse_args(int argc, char *argv[]) {
    printf("parse_args %d, ...\n", argc);
    int argi;
    fori (argi, 0, argc) {
        char *arg = argv[argi];
        printf(" parse arg <<%s>>\n", arg);
        if (arg[0] == 's' && arg[1] == 'c' && ishex(arg[2])) {
            int sc = 0;
            int i = 2;
            char c;
            while (ishex((c = arg[i+=1]))) {
                sc *= 16;
                sc += hdigtoi(c);
            }
            printf("  sc: %03x\n", sc);
            while (isspc(arg[i]))
                i++;
            c = arg[i];
            printf("  c: <<%c>>\n", c);
            if (c == '=' || c == ':') {
                i++;
                while (isspc(arg[i]))
                    i++;
                c = arg[i];
                if (c == 'u' || c == 'U') {
                    printf("  u");
                    WCHAR w = 0;
                    while (ishex((c = arg[i+=1]))) {
                        w *= 16;
                        w += hdigtoi(c);
                    }
                    printf(": %04x\n", w);
                    KL_bind(sc, KLM_WCHAR, w);
                } else if (c == 's' && arg[i+=1] == 'c') {
                    printf("  sc1");
                    int mods = KLM_SC;
                    SC sc1 = 0;
                    i++;
                    while (ishex((c = arg[i+=1]))) {
                        sc1 *= 16;
                        sc1 += hdigtoi(c);
                    }
                    printf(": sc%03x\n", sc1);
                    KL_bind(sc, mods, sc1);
                }
            }
        } else if (str_sets(arg, "lang")) {
            int i = 0;
            char c;
            while (isspc(arg[i])) i++;
            while ((c = arg[i]) && !isspc(c) && c != ':' && c != '=') i++;
            while (isspc(arg[i])) { i++; }
            c = arg[i];
            if (c == ':' || c == '=') {
                int n = atoi(arg + i + 1);
                printf("  bind lang: %04d\n", n);
                KL_set_bind_lang(n);
            }
        }
    }
}

char *default_argv[] = {
    "sc002:sc003",
    "lang:4",
    "sc003:sc004"
};

int main(int argc, char *argv[]) {
    KL_init();
    KL_activate();

    HK_register(HK_ONOFF, MOD_WIN, VK_F2);

    EH_activate();
    MH_activate();

    parse_args(len(default_argv), default_argv);
    parse_args(argc-1, argv+1);

    KL_activate_lang(LANG_NEUTRAL);
    KL_activate_lang(4);

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
            int id = (int)msg.wParam;
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
