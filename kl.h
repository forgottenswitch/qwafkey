#ifndef KL_H_INCLUDED
#define KL_H_INCLUDED

#include "stdafx.h"
#include "km.h"

/* kl.h -- the low level keyboard hook */

/*
KPN is number of bingins on a level (binding is a mapping from SC to KP to perform)
It should cover tilde,1..0,minus,plus,backslash,brackets,semicolon,apostrophe,comma,period,slash
and others alphanumericals: a key on left of Z and a key on right of apostrophe.
All of the tilde till slash happen to have SC less than 64.
*/
#define KPN 64
typedef LK KLV[KPN];
#define KLVN 6
typedef KLV KLY[KLVN];

extern bool KL_active;
extern HHOOK KL_handle;

extern UCHAR KL_phys_mods[SC_COUNT];

extern KM KL_km_shift;
extern KM KL_km_control;
extern KM KL_km_alt;
extern KM KL_km_l3;
extern KM KL_km_l5;

LRESULT CALLBACK KL_proc(int aCode, WPARAM wParam, LPARAM lParam);

void KL_init(void);
void KL_toggle(void);
void KL_activate(void);
void KL_deactivate(void);
void KL_bind(SC sc, UINT lvl, UINT mods, SC binding);
void KL_temp_sc(SC sc, SC mods, SC binding);

void KL_add_lang(LANGID lang);
void KL_activate_lang(LANGID lang);
void KL_set_bind_lang(LANGID lang);
void KL_set_vks_lang(LANGID lang);
void KL_define_vks(void);

void KL_bind_init(void);
void KL_bind_lvls_zero(void);
void KL_bind_lvls_init(void);

void KL_dk_send_wchar(WCHAR wc);

#endif // KL_H_INCLUDED
