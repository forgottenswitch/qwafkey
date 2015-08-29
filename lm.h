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

void LM_init();
void LM_activate_selected_locale();

#endif // LM_H_INCLUDED
