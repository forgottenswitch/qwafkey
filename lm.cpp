#include "lm.h"
#include "kl.h"

LM_LocalesBuffer LM_locales = { 0, 0, nil };
int LM_selected_locale = 0;

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
    for (int i = count - 1; i >= 0; i--) {
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
        printf("gkln{\"%s\",\"%s\"} ", langstr, str4);
        LANGID lang = atoi(str4);
        l->lang = lang;
        printf(" locale{%04d, %08x}\n", lang, (UINT)hkl);
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

void LM_init() {
    puts("LM_init locales...");
    LM_get_locales(true);
    puts("LM_init activate...");
    LM_activate_selected_locale();
    puts("LM_init done");
}

void LM_activate_selected_locale() {
    LM_Locale *l = LM_locales.elts + LM_selected_locale;
    LANGID lang = l->lang;
    printf(" locale %04d ", lang);
    ActivateKeyboardLayout(l->hkl, 0);
    KL_activate_lang(lang);
}
