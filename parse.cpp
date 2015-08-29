#include "stdafx.h"
#include "kl.h"
#include "ka.h"
#include "kn.h"

bool isspc(TCHAR tc) {
    return (tc == ' ') || (tc == '\t');
}

bool ishex(TCHAR tc) {
    return (tc >= '0' && tc <= '9') || (tc >= 'a' && tc <= 'f') || (tc >= 'A' && tc <= 'F');
}

int hdigtoi(TCHAR tc) {
    if (tc >= '0' && tc <= '9')
        return tc - '0';
    if (tc >= 'a' && tc <= 'f')
        return tc - 'a' + 10;
    if (tc >= 'A' && tc <= 'F')
        return tc - 'A' + 10;
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
    KL_bind_lvls_init();
    int argi;
    fori (argi, 0, argc) {
        char *arg = argv[argi];
        int sc = 0;
        int i = 1;
        char c;

        printf(" parse arg <<%s>>", arg);
        if (arg[0] == 's' && arg[1] == 'c' && ishex(arg[2])) {
            while (ishex((c = arg[i+=1]))) {
                sc *= 16;
                sc += hdigtoi(c);
            }
            printf("  sc:%03x", sc);
            goto assign;
        } else if ((sc = KN_lname_to_sc(arg))) {
            while ((c = arg[i]) && c != ':' && c != '=') i++;
            goto assign;
        } else if (str_sets(arg, "lang")) {
            int i = 0;
            char c;
            while ((c = arg[i]) && c != ':' && c != '=') i++;
            if (c) {
                int n = atoi(arg + i + 1);
                printf("  bind lang: %04d\n", n);
                KL_bind_lvls_init();
                KL_set_bind_lang(n);
            }
        } else if (str_sets(arg, "levels")) {
            int i = 0;
            char c;
            while ((c = arg[i]) && c != ':' && c != '=') i++;
            if (c) {
                i++;
                while (isspc(arg[i])) i++;
                KL_bind_lvls_zero();
                while ((c = arg[i])) {
                    i++;
                    if (c >= '0' && c <= '9') {
                        int d = c - '0';
                        KL_bind_lvls[d-1] = true;
                    }
                }
            }
            puts("");
        }
        continue;

        assign:
        while (isspc(arg[i])) i++;
        if ((c = arg[i]) == '=' || c == ':') {
            i++;
            while (isspc(arg[i]))
                i++;
            c = arg[i];
            VK vk;
            if ((vk = KN_name_to_vk(arg + i))) {
                printf(": [%s]\n", arg + i);
                KL_bind(sc, 0, vk);
            } else if (c == 'u' || c == 'U') {
                printf("  u");
                WCHAR w = 0;
                while (ishex((c = arg[i+=1]))) {
                    w *= 16;
                    w += hdigtoi(c);
                }
                printf(": %04x\n", w);
                KL_bind(sc, KLM_WCHAR, w);
            } else if (c == '=') {
                printf("  =");
                i++;
                while (isspc(arg[i])) i++;
                char c = arg[i];
                printf(":%03d,%c\n", c, c);
                KL_bind(sc, KLM_WCHAR, c);
            } else if (c == 's' && arg[i+=1] == 'c') {
                printf("  sc1");
                int mods = KLM_SC;
                SC sc1 = 0;
                while (ishex((c = arg[i+=1]))) {
                    sc1 *= 16;
                    sc1 += hdigtoi(c);
                }
                printf(":%03x\n", sc1);
                KL_bind(sc, mods, sc1);
            } else if (c == 'v' && arg[i+=1] == 'k') {
                printf("  vk1");
                int mods = 0;
                UINT vk1 = 0;
                while (ishex((c = arg[i+=1]))) {
                    vk1 *= 16;
                    vk1 += hdigtoi(c);
                }
                printf(":%02x\n", vk1);
                KL_bind(sc, mods, vk1);
            } else if (c == '!') {
                printf("  ka");
                char *name = arg + i + 1;
                int id = KA_name_to_id(name);
                printf("%d", id);
                if (id >= 0) {
                    printf(" %s\n", name);
                    KL_bind(sc, KLM_KA, id);
                }
                printf("\n");
            }

        }
    }
}
