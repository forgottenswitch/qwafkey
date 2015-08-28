#include "stdafx.h"

extern bool KL_active;
extern HHOOK KL_handle;

LRESULT CALLBACK KL_proc(int aCode, WPARAM wParam, LPARAM lParam);

void KL_init();
void KL_activate();
void KL_deactivate();
void KL_bind(SC sc, UINT mods, SC binding);

void KL_add_lang(LANGID lang);
void KL_activate_lang(LANGID lang);
void KL_set_bind_lang(LANGID lang);
