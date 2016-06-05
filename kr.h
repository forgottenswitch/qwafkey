#ifndef KR_H_INCLUDED
#define KR_H_INCLUDED

#include "stdafx.h"

/* kr.h -- key remapper; allows for keys to be assigned per-application */

#define KR_MAXTITLE 64
#define KR_MAXCLASS 32

extern bool KR_active;
extern size_t KR_id;

void KR_activate(void);
void KR_deactivate(void);
void KR_toggle(void);
void KR_clear(void);
void KR_resume(bool on_pt_only);
void KR_on_task_switch(HWND hwnd, char *wndclass, bool on_pt_only);

void KR_hash_the_titles(void);
void KR_hash_the_wndcls(void);

void KR_add_app(void);
void KR_set_bind_title(char *title);
void KR_set_bind_class(char *wndclass);
void KR_bind(SC sc, SC binding, USHORT mods);

void KR_init(void);

#endif // KR_H_INCLUDED
