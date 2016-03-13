#include "lm.h"
#include "kl.h"
#include "parse.h"

/* lm.c -- layout/locale manager
 *
 * Functions for querying and activating of system input locales,
 * that is, keyboard layouts.
 *
 * */

LM_LocalesBuffer LM_locales = { 0, 0, nil };
ssize_t LM_selected_locale = 0;

int hexntoi(char *str, size_t n) {
    size_t i;
    int rv = 0;
    fori (i, 0, n) {
        char c = *str;
        str++;
        rv *= 16;
        if (between(c, '0', '9')) {
            rv += c - '0';
        } else if (between(c, 'a', 'f')) {
            rv += c - 'a' + 10;
        } else if (between(c, 'A', 'F')) {
            rv += c - 'A' + 10;
        } else {
            return rv /= 16;
        }
    }
    return rv;
}

void LM_get_locales(bool set_selected) {
    int count = LM_locales.count, size = LM_locales.size;
    LM_Locale *elts = LM_locales.elts;
    ZeroMemory(elts, size * sizeof(LM_Locale));
    count = GetKeyboardLayoutList(0, nil);
    if (elts == nil) {
        elts = (LM_Locale*) calloc((size = count), sizeof(LM_Locale));
    }
    if (size < count) {
        elts = (LM_Locale*) realloc(elts, (size = count * sizeof(LM_Locale)));
    }
    GetKeyboardLayoutList(count, (HKL*)elts);
    HKL cur_hkl = GetKeyboardLayout(0);
    int i;
    for (i = count - 1; i >= 0; i--) {
        LM_Locale *l = (elts + i);
        HKL* elts_as_hkls = (HKL*) elts;
        HKL hkl = elts_as_hkls[i];
        l->hkl = hkl;
        ActivateKeyboardLayout(hkl, 0);
        char langstr[KL_NAMELENGTH + 1];
        char str4[5];
        GetKeyboardLayoutName(langstr);
        langstr[KL_NAMELENGTH] = '\0';
        CopyMemory(str4, langstr+4, 4);
        str4[4] = '\0';
        dput("gkln{\"%s\",\"%s\"} ", langstr, str4);
        LANGID lang = hexntoi(str4, len(str4));
        l->lang = lang;
        dput(" locale{%04x, %08x}\n", lang, (UINT)hkl);
    }
    if (set_selected) {
        bool found;
        fori (i, 0, count) {
            if (elts[i].hkl == cur_hkl) {
                found = true;
                break;
            }
        }
        LM_selected_locale = (found ? i : 0);
    }
    ActivateKeyboardLayout(cur_hkl, 0);
    LM_locales.size = size;
    LM_locales.count = count;
    LM_locales.elts = elts;
}

HWND LM_hwnd = 0;

void LM_activate_nth_locale(size_t idx) {
    LM_Locale *l = LM_locales.elts + idx;
    LANGID lang = l->lang;
    dput(" locale %04x ", lang);
    OS_activate_layout(LM_hwnd, l->hkl);
    KL_activate_lang(lang);
}

void LM_activate_selected_locale() {
    LM_activate_nth_locale(LM_selected_locale);
}

void LM_activate_next_locale() {
    if ((LM_selected_locale += 1) >= LM_locales.count)
        LM_selected_locale = 0;
    LM_activate_selected_locale();
}

void LM_activate_prev_locale() {
    if ((LM_selected_locale -= 1) >= 0)
        LM_selected_locale = LM_locales.count;
    LM_activate_selected_locale();
}

LANGID LM_selected_langid() {
    return LM_locales.elts[LM_selected_locale].lang;
}

HKL LM_langid_to_hkl(LANGID langid) {
    ssize_t i;
    fori (i, 0, LM_locales.count) {
        LM_Locale *loc = LM_locales.elts + i;
        if (loc->lang == langid) {
            return loc->hkl;
        }
    }
    return nil;
}
