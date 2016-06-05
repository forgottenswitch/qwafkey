#ifndef LM_H_INCLUDED
#define LM_H_INCLUDED

#include "stdafx.h"

/* lm.h -- layout/locale manager */

typedef struct {
    HKL hkl;
    LANGID lang;
} LM_Locale;

typedef struct {
    size_t size;
    ssize_t count;
    LM_Locale *elts;
} LM_LocalesBuffer;

extern ssize_t LM_selected_locale;
extern LM_LocalesBuffer LM_locales;
extern HWND LM_hwnd;

void LM_get_locales(bool set_selected);
void LM_activate_nth_locale(size_t idx);
void LM_activate_selected_locale(void);
void LM_activate_next_locale(void);
void LM_activate_prev_locale(void);
LANGID LM_selected_langid(void);
HKL LM_langid_to_hkl(LANGID langid);

#endif // LM_H_INCLUDED
