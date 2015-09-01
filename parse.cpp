#include "stdafx.h"
#include "kl.h"
#include "ka.h"
#include "kn.h"
#include "hk.h"
#include "parse.h"

#ifndef MOD_NOREPEAT
# define MOD_NOREPEAT 0x4000
#endif // MOD_NOREPEAT

bool isspc(char tc) {
    return (tc == ' ') || (tc == '\t');
}

bool ishex(char tc) {
    return (tc >= '0' && tc <= '9') || (tc >= 'a' && tc <= 'f') || (tc >= 'A' && tc <= 'F');
}

int hdigtoi(char tc) {
    if (tc >= '0' && tc <= '9')
        return tc - '0';
    if (tc >= 'a' && tc <= 'f')
        return tc - 'a' + 10;
    if (tc >= 'A' && tc <= 'F')
        return tc - 'A' + 10;
    return -1;
}

int hextoi(char *hex) {
    while (isspc(*hex))
        hex++;
    if (hex[0] == '0' && hex[1] == 'x')
        hex+=2;
    int rv = 0;
    char c, d;
    while ((c = hex[0]) && ((d = hdigtoi(c)) >= 0)) {
        rv *= 16;
        rv += d;
        hex++;
    }
    return rv;
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

bool parse_in_mods = false;
UINT parse_hk_mods;

void parse_args(int argc, char *argv[]) {
    dput("parse_args %d, ...\n", argc);
    KL_bind_lvls_init();
    int argi;
    fori (argi, 0, argc) {
        char *arg = argv[argi];
        int sc = 0;
        int i = 0;
        char c;

        dput("\nparse arg <<%s>> ", arg);
        UINT mods = 0;
        while (isspc(arg[0])) arg++;
        while ((c = arg[0]) && (c == '!' || c == '^' || c == '+' || c == '#' || c == '@')) {
            arg++;
            switch (c) {
            case '!': mods |= MOD_ALT; break;
            case '^': mods |= MOD_CONTROL; break;
            case '+': mods |= MOD_SHIFT; break;
            case '#': mods |= MOD_WIN; break;
            case '@': mods |= MOD_NOREPEAT; break;
            }
        }
        if (arg[0] == 's' && arg[1] == 'c' && ishex(arg[2])) {
            i = 1;
            while (ishex((c = arg[i+=1]))) {
                sc *= 16;
                sc += hdigtoi(c);
            }
            dput("  sc:%03x", sc);
            goto assign;
        } else if ((sc = KN_lname_to_sc(arg))) {
            while ((c = arg[i]) && c != ':' && c != '=') i++;
            goto assign;
        } else if (str_sets(arg, "lang")) {
            parse_in_mods = false;
            i = 0;
            while ((c = arg[i]) && c != ':' && c != '=') i++;
            if (c) {
                int n = hextoi(arg + i + 1);
                dput("  bind lang: %04d", n);
                KL_bind_lvls_init();
                KL_set_bind_lang(n);
            }
        } else if (str_sets(arg, "levels")) {
            parse_in_mods = false;
            i = 0;
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
                dput(": [%s] ", arg + i);
                KL_bind(sc, 0, vk);
            } else if (c == 'u' || c == 'U') {
                dput("  u");
                WCHAR w = 0;
                while (ishex((c = arg[i+=1]))) {
                    w *= 16;
                    w += hdigtoi(c);
                }
                dput(": %04x ", w);
                KL_bind(sc, KLM_WCHAR, w);
            } else if (c == '=') {
                dput("  =");
                i++;
                while (isspc(arg[i])) i++;
                char c = arg[i];
                dput(":%03d,%c ", c, c);
                KL_bind(sc, KLM_WCHAR, c);
            } else if (c == 's' && arg[i+=1] == 'c') {
                dput("  sc1");
                int mods = KLM_SC;
                SC sc1 = 0;
                while (ishex((c = arg[i+=1]))) {
                    sc1 *= 16;
                    sc1 += hdigtoi(c);
                }
                dput(":%03x ", sc1);
                KL_bind(sc, mods, sc1);
            } else if (c == 'v' && arg[i+=1] == 'k') {
                dput("  vk1");
                int mods = 0;
                UINT vk1 = 0;
                while (ishex((c = arg[i+=1]))) {
                    vk1 *= 16;
                    vk1 += hdigtoi(c);
                }
                dput(":%02x ", vk1);
                KL_bind(sc, mods, vk1);
            } else if (c == '!') {
                dput("  ka");
                char *name = arg + i + 1;
                int id = KA_name_to_id(name);
                dput("%d ", id);
                if (id >= 0) {
                    dput(" !%s/%d ", name, mods);
                    if (mods) {
                        VK vk = OS_sc_to_vk(sc);
                        HK_KA_register(id, mods, vk);
                    } else {
                        KL_bind(sc, KLM_KA, id);
                    }
                }
            }
        }
    }
}
