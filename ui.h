#ifndef UI_H_INCLUDED
#define UI_H_INCLUDED

#include "stdafx.h"

/* ui.h -- the graphical user interface */

extern HWND UI_MW;

#define UI_TRAY_MSG (WM_APP + 1)

#define UI_STR_MAXLEN 128
typedef char UI_STR[UI_STR_MAXLEN];

bool UI_init(void);
void UI_spawn(void);
void UI_quit(int);

void UI_TR_delete(void);
bool UI_TR_update(void);

bool UI_ask_for_creating_config_file(char *path);
void UI_maybe_show_errors_for_config_file(char *path);

#endif // UI_H_INCLUDED
