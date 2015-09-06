#ifndef LM_H_INCLUDED
#define LM_H_INCLUDED

#include "stdafx.h"

typedef struct {
    HKL hkl;
    LANGID lang;
} LM_Locale;

typedef struct {
    int size;
    int count;
    LM_Locale *elts;
} LM_LocalesBuffer;

extern int LM_selected_locale;
extern LM_LocalesBuffer LM_locales;
extern HWND LM_hwnd;

void LM_init();
void LM_activate_nth_locale(size_t idx);
void LM_activate_selected_locale();
void LM_activate_next_locale();
void LM_activate_prev_locale();
LANGID LM_selected_langid();
HKL LM_langid_to_hkl(LANGID langid);

#endif // LM_H_INCLUDED
