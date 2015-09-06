#include "stdafx.h"
#include "kl.h"
#include "ka.h"
#include "kn.h"
#include "hk.h"
#include "parse.h"

#ifndef MOD_NOREPEAT
# define MOD_NOREPEAT 0x4000
#endif // MOD_NOREPEAT

size_t parse_lineno;
size_t parse_colno;

bool isspc(char c) {
    return (c == ' ') || (c == '\t');
}

bool ishex(char c) {
    return between(c, '0', '9') || between(c, 'a', 'f') || between(c, 'A', 'F');
}

bool isdecimal(char c) {
    return between(c, '0', '9');
}

int hexdigtoi(char c) {
    if (between(c, '0', '9'))
        return c - '0';
    if (between(c, 'a', 'f'))
        return c - 'a' + 10;
    if (between(c, 'A', 'F'))
        return c - 'A' + 10;
    return -1;
}

bool isidentchar0(char c) {
    return between(c, 'a', 'z') || between(c, 'A', 'Z') || (c == '_');
}

bool isidentchar(char c) {
    return isidentchar0(c) || between(c, '0', '9');
}

#define READ_PARMS char **input
#define RET(str1, val) do { *input = (str1); return (val); } while (0)

int read_Z_decimal(READ_PARMS) {
    char *str = *input, c;
    int sign = 1;
    if ((c = *str) && (c == '-')) {
        sign = -1;
        str++;
    } else if (c == '+') {
        str++;
    }
    while (isspc(*str)) str++;
    if (isdecimal((c = *str))) {
        int n = c - '0';
        while (isdecimal((c = *(str+=1)))) {
            n *= 10;
            n += c - '0';
        }
        RET(str, sign * n);
    }
    return 0;
}

int read_hex(READ_PARMS) {
    char *str = *input, c = *str;
    int sign = 1;
    if (c && (c == '-')) {
        sign = -1;
        str++;
    } else if (c == '+') {
        str++;
    } else if (c == '0' && str[1] == 'x') {
        str+=2;
    }
    while (isspc(*str)) str++;
    if (ishex((c = *str))) {
        int n = hexdigtoi(c);
        while (ishex((c = *(str+=1)))) {
            n *= 16;
            n += hexdigtoi(c);
        }
        RET(str, sign * n);
    }
    return 0;
}

int read_ka(READ_PARMS) {
    char *str = *input, c;
    int id;
    if (*str == '!') {
        str++;
        if (isidentchar0((c = *str))) {
            char buf[256];
            size_t i = 0;
            buf[0] = c;
            while (isidx(i, buf) && isidentchar((c = *(str++)))) {
                buf[i++] = c;
            }
            buf[i++] = '\0';
            id = KA_name_to_id(buf);
            RET(str, id);
        }
    }
    return 0;
}

int read_mods(READ_PARMS) {
    char *str = *input;
    int mods = 0;
    while (1) {
        char c = *str;
        switch (c) {
        case '!':
            mods |= MOD_ALT;
            break;
        case '^':
            mods |= MOD_CONTROL;
            break;
        case '+':
            mods |= MOD_SHIFT;
            break;
        case '#':
            mods |= MOD_WIN;
            break;
        case '@':
            mods |= MOD_NOREPEAT;
            break;
        default:
            RET(str, mods);
        }
        str++;
    }
}

bool read_char(READ_PARMS, char ch) {
    if (**input == ch) {
        RET((*input)+1, true);
    }
    return false;
}

bool read_comma(READ_PARMS) {
    return read_char(input, ',');
}

bool read_colon(READ_PARMS) {
    return read_char(input, ':');
}

bool read_newline(READ_PARMS) {
    char *str = *input;
    if (read_char(&str, '\r') &&
        read_char(&str, '\n')) {
        RET(str, true);
    }
    return false;
}

bool read_spc(READ_PARMS) {
    char *str = *input;
    bool moved = false;
    while (isspc(*str)) {
        str++;
        moved = true;
    }
    if (moved) {
        RET(str, true);
    }
    return false;
}

void read_to_bol(READ_PARMS) {
    char *str = *input, c;
    while ((c = *str) && (c != '\n')) str++;
    str++;
    *input = str;
}

bool read_empty_line(READ_PARMS) {
    char *str = *input;
    read_spc(&str);
    if ((str[0] == '\r') && (str[1] == '\n')) {
        RET(str + 2, true);
    }
    return false;
}

bool read_comment(READ_PARMS) {
    char *str = *input, c = *str;
    if (c == ';') {
        read_to_bol(&str);
        RET(str, true);
    } else if ((c == '/') && (str[1] == '/')) {
        read_to_bol(&str);
        RET(str, true);
    } else if ((c == '/') && (str[1] == '*')) {
        str+=2;
        while ((c = str[0]) && !((c == '*') && (str[1] == '/'))) {
            str++;
        }
        RET(str, true);
    }
    return false;
}

bool read_whitespace(READ_PARMS) {
    char *str = *input;
    bool moved = false;
    while (read_spc(&str) || read_comment(&str) || read_newline(&str)) {
        moved = true;
    }
    if (moved) {
        RET(str, true);
    }
    return false;
}

int read_sc_alias(READ_PARMS) {
    char *str = *input, *str0 = str;
    if (isidentchar0(str[0])) {
        char buf[256];
        do str++; while (isidentchar(*str));
        strbcr(buf, str0, str+1);
        SC sc = KN_lname_to_sc(buf);
        if (sc) {
            RET(str, sc);
        }
    }
    return 0;
}

int read_vk_alias(READ_PARMS) {
    char *str = *input, *str0 = str;
    if (isidentchar0(str[0])) {
        char buf[256];
        do str++; while (isidentchar(*str));
        strbcr(buf, str0, str+1);
        RET(str, KN_name_to_vk(buf));
    }
    return 0;
}

typedef struct {
    SC sc;
    char mods;
    SC binding;
} Bind;

#define getter(name, type)            /*
*/  name(                              /*
*/    READ_PARMS,                      /*
*/    type *rv,                        /*
*/    type (fn)(READ_PARMS)            /*
*/  ) {                                /*
*/    char *str = *input, *str0 = str; /*
*/    *rv = fn(&str);                  /*
*/    if (str != str0) {               /*
*/      RET(str, true);                /*
*/     }                               /*
*/    return false;                    /*
*/  }
bool getter(get_int, int);
bool getter(get_size_t, size_t);
bool getter(get_bind, Bind);
#undef getter

#define RET_get(str, type, getter, reader)    /*
*/  do {                                      /*
*/    type rv;                                /*
*/    if (getter(&(str), &rv, (reader))) {    /*
*/      RET(str, rv);                         /*
*/    }                                       /*
*/  } while (0)

int read_sc(READ_PARMS) {
    char *str = *input;
    if ((str[0] == 's') && (str[1] == 'c')) {
        str+=2;
        int sc;
        if (get_int(&str, &sc, read_hex)) {
            RET(str, sc);
        }
    }
    return 0;
}

int read_vk(READ_PARMS) {
    char *str = *input;
    if ((str[0] == 'v') && (str[1] == 'k')) {
        str+=2;
        int vk;
        if (get_int(&str, &vk, read_hex)) {
            RET(str, vk);
        }
    }
    return 0;
}

int read_unicode_char(READ_PARMS) {
    char *str = *input, c = str[0];
    if (c == 'u' || c == 'U') {
        str++;
        RET_get(str, int, get_int, read_hex);
    }
    return 0;
}

int read_utf8_ch(READ_PARMS) {
    char *str = *input, c = *str;
    int ch = 0, cl = 0;
    if (c>>7 == 0) {
        RET(str+1, c);
    } else if (c>>5 == 0b110) {
        ch = c & (32-1);
        cl = 1;
    } else if (c>>4 == 0b1110) {
        ch = c & (16-1);
        cl = 2;
    } else if (c>>6 == 0b11110) {
        ch = c & (8-1);
        cl = 3;
    }
    for (; cl; cl--) {
        str++;
        if ((c = *str)) {
            if (c>>6 != 0b10) {
                RET(str+1, ch);
            }
            ch *= (64-1);
            ch += c & (64-1);
        }
    }
    RET(str+1, ch);
}

int read_raw_char(READ_PARMS) {
    char *str = *input;
    if (*str == '=') {
        str++;
        RET_get(str, int, get_int, read_utf8_ch);
    }
    return 0;
}

Bind read_binding(READ_PARMS) {
    char *str = *input;
    int w, sc1, vk1, ka;
    Bind rv = { 0, 0, 0 };
    if (get_int(&str, &w, read_unicode_char) || get_int(&str, &w, read_raw_char)) {
        rv.mods = KLM_WCHAR;
        rv.binding = w;
    } else if (get_int(&str, &vk1, read_vk) || get_int(&str, &vk1, read_vk_alias)) {
        rv.binding = vk1;
    } else if (get_int(&str, &sc1, read_sc) || get_int(&str, &sc1, read_sc_alias)) {
        rv.mods = KLM_SC;
        rv.binding = sc1;
    } else if (get_int(&str, &ka, read_ka)) {
        rv.mods = KLM_KA;
        rv.binding = ka;
    } else {
        return rv;
    }
    dput("rb{%x} ", rv.binding);
    RET(str, rv);
}

bool Bind_lvls[KLVN];
bool Bind_sole_lvl = false;

bool read_bind(READ_PARMS) {
    char *str = *input;
    int sc, vk;
    Bind bind, binds[KLVN];
    size_t binds_count = 0;
    if (get_int(&str, &sc, read_sc) || get_int(&str, &vk, read_vk) || get_int(&str, &sc, read_sc_alias) ) {
        read_spc(&str);
        if (read_colon(&str)) {
            do {
                read_spc(&str);
                if (get_bind(&str, &bind, read_binding)) {
                    if (isidx(binds_count, binds)) {
                        binds[binds_count] = bind;
                    }
                    binds_count++;
                }
                read_spc(&str);
            } while (read_comma(&str));
            size_t binds_i = 0;
            if (binds_count) {
                if (Bind_sole_lvl) {
                    size_t i;
                    fori (i, 0, KLVN) {
                        if (Bind_lvls[i]) {
                            bind = binds[0];
                            KL_bind(sc, i, bind.mods, bind.binding);
                            i++;
                            if ((i < KLVN) && (i % 2)) {
                                dput("B(%d) ", binds_count);
                                bind = binds[(binds_count > 1 ? 1 : 0)];
                                KL_bind(sc, i, bind.mods, bind.binding);
                            }
                            break;
                        }
                    }
                } else {
                    size_t i;
                    fori (i, 0, KLVN) {
                        if (Bind_lvls[i]) {
                            bind = binds[binds_i];
                            KL_bind(sc, i, bind.mods, bind.binding);
                            binds_i++;
                            if (binds_i >= binds_count) {
                                binds_i = 0;
                            }
                        }
                    }
                }
            }
            RET(str, true);
        }
    }
    return false;
}

bool read_hotk(READ_PARMS) {
    char *str = *input;
    int mods, sc = 0, vk, ka;
    if (get_int(&str, &mods, read_mods)) {
        if (get_int(&str, &vk, read_vk) || get_int(&str, &sc, read_sc) || get_int(&str, &sc, read_sc_alias)) {
            if (!vk) {
                if (!sc) {
                    return 0;
                }
                vk = OS_sc_to_vk(sc);
            }
            if (read_colon(&str)) {
                if (get_int(&str, &ka, read_ka)) {
                    HK_KA_register(ka, mods, vk);
                    RET(str, true);
                }
            }
        }
    }
    return false;
}

bool read_word(READ_PARMS, char *wrd) {
    char *str = *input;
    size_t wrdlen = strlen(wrd);
    if (!strncmp(str, wrd, wrdlen)) {
        if (!isidentchar(str[wrdlen])) {
            RET(str+wrdlen, true);
        }
    }
    return false;
}

bool read_lang(READ_PARMS) {
    char *str = *input;
    if (read_word(&str, "lang")) {
        if (read_colon(&str)) {
            int n;
            if ((n = read_hex(&str))) {
                KL_set_bind_lang(n);
                RET(str, true);
            }
        }
    }
    return false;
}

size_t read_N_decimal(READ_PARMS) {
    char *str = *input;
    int n;
    if (get_int(&str, &n, read_Z_decimal)) {
        if (n > 0) {
            size_t n_gz = n;
            RET(str, n_gz);
        }
    }
    return 0;
}

bool read_levs(READ_PARMS) {
    char *str = *input;
    size_t n;
    bool zeroed = false;
    if (read_word(&str, "level") && read_colon(&str)) {
        read_spc(&str);
        read_colon(&str);
        read_spc(&str);
        if (get_size_t(&str, &n, read_N_decimal)) {
            dput("lvl(%d) ", n);
            n--;
            if (isidx(n, Bind_lvls)) {
                ZeroBuf(Bind_lvls);
                Bind_sole_lvl = true;
                Bind_lvls[n] = true;
            };
        }
        RET(str, true);
    } else if (read_word(&str, "levels")) {
        read_spc(&str);
        read_colon(&str);
        do {
            read_spc(&str);
            if (get_size_t(&str, &n, read_N_decimal)) {
                dput("lvn(%d) ", n);
                n--;
                if (isidx(n, Bind_lvls)) {
                    if (!zeroed) {
                        ZeroBuf(Bind_lvls);
                        zeroed = true;
                    }
                    Bind_sole_lvl = false;
                    Bind_lvls[n] = true;
                }
            }
            read_spc(&str);
        } while (read_comma(&str));
        RET(str, true);
    };
    return false;
}

bool read_line(READ_PARMS) {
    dput("unrecognized line %d\n", parse_lineno);
    read_to_bol(input);
    return (**input != '\0');
}

#undef READ_PARMS

void parse_args(int argc, char *argv[], int argb) {
    int argi;
    ZeroBuf(Bind_lvls);
    KL_bind_init();
    fori (argi, argb, argc) {
        char *arg = argv[argi];
        read_spc(&arg);
        dput("%20s| ", arg);
        if (!(read_bind(&arg) ||
              read_hotk(&arg) ||
              read_lang(&arg) ||
              read_levs(&arg)
        )) {
            dput("unrecognized arg %d: %s\n", argi, arg);
        }
        dput("\n");
    }
}

void parse_str(char *str) {
    parse_lineno = 0;
    parse_colno = 0;
    ZeroBuf(Bind_lvls);
    KL_bind_init();
    while (read_whitespace(&str) ||
           read_bind(&str) ||
           read_hotk(&str) ||
           read_lang(&str) ||
           read_levs(&str) ||
           read_line(&str)
    ) {
        parse_lineno += 1;
    }
}
