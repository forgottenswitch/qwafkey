/*

  A program for:
   Key remappings:
    lcontrol for capslock,
    a letter on level5 for an arrow,
    etc.
   Custom key actions:
    Level2_Shift,
    Level2_Latch,
    Level5_Shift,
    Level3_Latch being AltGr latch,
    User-defined in external DLLs:
     Level5_Lock, Screen dimming, etc.

  Custom keys could be defined by DLLs.

  Usage: l5 [DLL to load]...

*/

// ---------------------------
// Features

#ifndef FEAT_GUI
# define FEAT_GUI 1
#endif

// Whether to allow run-time dll loading
#ifndef FEAT_DLL
# define FEAT_DLL 1
#endif

// OS-managed hotkeys, VK-based
// depend on FEAT_GUI
#ifndef FEAT_HOTKEYS_OS
# define FEAT_HOTKEYS_OS 1
#endif
// program-managed hotkeys, SC-based
#ifndef FEAT_HOTKEYS_SC
# define FEAT_HOTKEYS_SC 0
#endif
// program-managed hotkeys, VK-based
#ifndef FEAT_HOTKEYS_VK
# define FEAT_HOTKEYS_VK 1
#endif

// hotkey definitions
#ifndef FEAT_ACTIONS_ONOFF
# define FEAT_ACTIONS_ONOFF 1
#endif
#ifndef FEAT_ACTIONS_UNICODE
# define FEAT_ACTIONS_UNICODE 1
#endif
#ifndef FEAT_ACTIONS_POWER
# define FEAT_ACTIONS_POWER 1
#endif
#ifndef FEAT_ACTIONS_MOUSE
# define FEAT_ACTIONS_MOUSE 1
#endif

// program-managed keyboard layouts
#ifndef FEAT_LANGS
# define FEAT_LANGS 1
#endif

// ----------------------------
// Includes

// use XP features
#define _WIN32_WINNT 0x0500

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "resource.h"
#include "scancodes.h"

#if FEAT_ACTIONS_POWER
# include <powrprof.h>
#endif

// ---------------------------
// Constants

// The maximum number of scancodes.
// KeyboardProc() of WinAPI handles 9-bit scancodes,
// and so this value should be 512.
#ifndef KEYS
# define KEYS 512
#endif

// How many keyboard layout levels exist.
// 1-2 are for unshifted and shifted keys.
// 3-4 are for (un)/shifted altgr keys.
// 5-6 are for (un)/shifted level5 keys (layout.h).
// Levels 1-2 are required to place a level5 shift key.
#ifndef LEVELS
# define LEVELS 6
#endif

// The dwExtraInfo value for keybd_event() and SendInput().
// MSDN: bits 25..28 are reserved.
#ifndef KEYBD_EXTRA_INFO
# define KEYBD_EXTRA_INFO 0
#endif

// How many system layouts to remember for switching between.
#ifndef OS_LAYOUT_MAX
# define OS_LAYOUT_MAX 16
#endif

// ---------------------------
// Keycode constants

#ifndef VK_LEVEL2_SHIFT
# define VK_LEVEL2_SHIFT VK_LSHIFT
#endif

// ---------------------------
// General utilites

#define nil NULL
#define fori(counter, min, max) for(counter = (min); counter < (max); counter++)

// ---------------------------
// Application-specific utils

#define updn(down) (down ? '_' : '^')
#define updnf(flags) (down ? flags : flags | KEYEVENTF_KEYUP)

// ---------------------------
// Keyboard physical state
//
// Represents whether a particular physical key is down or released.
// Keyboard state is tracked in-program rather than relying on
// GetKeyStateAsync() for perfomance reasons.
// Keys are referenced using scancodes (not virtual keycodes VK_*),
// as virtual keycodes are locale (thread's keyboard layout) dependent,
// while scancodes represent a location rather than 'meaning' of a key.
// Virtual keycodes, however, do not represent the final characters,
// as there are dead keys.

bool keystate[KEYS];

bool keystate_none() {
    UINT sc;

    fori (sc, 0, KEYS) {
        if (keystate[sc]) {
            //printf("sc%04x _\n", sc);
            return false;
        }
    }
    return true;
}

void Keystate_Init(void) {
    int i;
    fori (i, 0, KEYS) {
        keystate[i] = false;
    }
}

// ---------------------------
// Modifiers state
//
// Variables used to keep keyboard logical state.
// Similarly to physical state, Shift state
// is tracked in-program for perfomance reasons.
// The variables are modified by actions fired on keypresses.
// The 'logical state' is used to determine which Layout level
// is currently in effect.

UCHAR // layout_levelN must be either 0 or N-1
  layout_level2 = 0, // shift state
  layout_level3 = 0, // altgr state
  layout_level5 = 0; // level5 state

UCHAR Layout_GetLevel() {
    UCHAR level = layout_level2 + layout_level3 + layout_level5;
    level = min(LEVELS - 1, level);
    return level;
}

// ---------------------------
// Layout
//
// Describes key remappings used (by the program) on a particular keyboard level:
//  scancode
//  virtual keycode
//  action (a function to call)
// as well as accompanying information:
//  whether key is a modifier (so that it would affect latch state),
//  flags passed to keybd_event (so that it could map to a Unicode character or Extended key)
//   (Extended keys are RAlt, RControl, arrows, and some numpad keys).

#define KBD_ACTION bool
#define KBD_ACTION_ARGS KBDLLHOOKSTRUCT *ev, bool down
#define KBD_ACTION_PARAMS ev, down
typedef KBD_ACTION (*KBD_ACTION_FUNC)(KBD_ACTION_ARGS);

#define SC USHORT
#define VK UCHAR

// since KEYS are 9 bits, and SC is 16 bits, define the mask
#define SC_MAX 0x1FF
#define SC_IDX(sc) ((sc) & SC_MAX)

typedef struct {
    SC sc; // 2 bytes
    VK vk; // 1 byte
    UCHAR modifier; // 1 byte
    DWORD flags; // 4 bytes
    KBD_ACTION_FUNC action; // platform bits
} Layout_Key;

typedef Layout_Key Layout_Level[KEYS];

Layout_Level layout[LEVELS];

void Layout_Key_Init(Layout_Key *key) {
    key->sc = 0;
    key->vk = 0;
    key->flags = 0;
    key->action = nil;
    key->modifier = 0;
}

void Layout_Init(void) {
    int i, j;
    fori (i, 0, LEVELS) {
        fori (j, 0, KEYS) {
            Layout_Key_Init(&layout[i][j]);
        }
    }
}

Layout_Key* Layout_GetKey(SC sc) {
    return &layout[Layout_GetLevel()][(VK)sc];
}

/*  ---------------------------
    Layout defining

    Functions used to define the layout described in layout.h.
*/

// Modifiers - to indicate whether a key on a given level is a modifier.

typedef struct {
    SC key;
    UCHAR level;
} Layout_BindMod_Key;

void Layout_BindMod(SC key, UCHAR level) {
    Layout_Key *lk = &layout[level - 1][SC_IDX(key)];
    lk->modifier = true;
}

void Layout_BindMods(Layout_BindMod_Key *keys) {
    for(; keys->key && keys->level; keys++) {
        Layout_BindMod(keys->key, keys->level);
    }
}

// Actions - to be executed on key press(es) and release.

typedef struct {
    SC key;
    UCHAR level;
    KBD_ACTION_FUNC action;
} Layout_BindAct_Key;

void Layout_BindAct(SC key, UCHAR level, KBD_ACTION_FUNC action) {
    Layout_Key *lk = &layout[level - 1][SC_IDX(key)];
    Layout_Key_Init(lk);
    lk->action = action;
}

void Layout_BindActs(Layout_BindAct_Key *keys) {
    for (; keys->key && keys->level && (keys->action != nil); keys++) {
        Layout_BindAct(keys->key, keys->level, keys->action);
    }
}

// Key - virtual keycode to be sent on key event.

typedef struct {
    SC key;
    UCHAR level;
    VK vk;
} Layout_BindKey_Key;

void Layout_BindKey(SC key, UCHAR level, VK vk) {
    Layout_Key *lk = &layout[level - 1][SC_IDX(key)];
    Layout_Key_Init(lk);
    lk->vk = vk;
}

void Layout_BindKeys(Layout_BindKey_Key *keys) {
    for (; keys->key && keys->level && keys->vk; keys++) {
        Layout_BindKey(keys->key, keys->level, keys->vk);
    }
}

// Character - UTF-16 character to be sent on key event

typedef struct {
    SC key;
    UCHAR level;
    USHORT unicode_chr;
} Layout_BindChr_Key;

void Layout_BindChr(SC key, UCHAR level, USHORT unicode_chr) {
    Layout_Key *lk = &layout[level - 1][SC_IDX(key)];
    Layout_Key_Init(lk);
    lk->vk = 0; // MSDN
    lk->sc = unicode_chr;
    lk->flags = KEYEVENTF_UNICODE;
}

void Layout_BindChrs(Layout_BindChr_Key *keys) {
    for (; keys->key || keys->level || keys->unicode_chr; keys++) {
        Layout_BindChr(keys->key, keys->level, keys->unicode_chr);
    }
}

// ---------------------------
// Keyboard modifier utilites
//
// This (KbdMod struct) is used to make modifiers tracking
// independent of their number.
// Used by Layout_Key actions to keep track
// of Shift keys and Level5 shift.

#define KBDMOD_CAPACITY 4

typedef struct {
    UCHAR count, capacity;
    SC keys[KBDMOD_CAPACITY];
} KbdMod;

void KbdMod_Init(KbdMod *mod) {
    UCHAR i;
    SC *key = &(mod->keys[0]);
    mod->capacity = KBDMOD_CAPACITY;
    mod->count = 0;
    fori (i, 0, KBDMOD_CAPACITY) {
        *key = 0;
        key++;
    }
}

void KbdMod_Press(KbdMod *mod, SC sc) {
    UCHAR i = -1, len = mod->count;
    SC *keys = &(mod->keys[0]);
    //printf("mod_press.count %d:%d %x\n", len, mod->capacity, sc);
    //printf("mods: %x %x %x %x\n", mod->keys[0], mod->keys[1], mod->keys[2], mod->keys[3]);
    if (len >= mod->capacity)
        return;
    while ((i+=1) < len) {
        if (*keys == sc) {
            return;
        }
        keys++;
    }
    mod->keys[len] = sc;
    mod->count += 1;
}

void KbdMod_Release(KbdMod *mod, SC sc) {
    UCHAR i = 255, len = mod->count;
    SC *keys = &(mod->keys[0]);
    //printf("mod_release.count %d %x\n", len, sc);
    //printf("mods: %x %x %x %x\n", mod->keys[0], mod->keys[1], mod->keys[2], mod->keys[3]);
    while ((i+=1) < len) {
        //printf("<> %x %x", *keys, sc);
        if (*keys == sc) {
            *keys = mod->keys[len - 1];
            mod->count -= 1;
            break;
        }
        keys++;
    }
}

void KbdMod_Switch(KbdMod *mod, SC sc, bool down) {
    if (down) {
        KbdMod_Press(mod, sc);
    } else {
        KbdMod_Release(mod, sc);
    }
}

/* ---------------------------
   SendInput utilites
*/

void SendInput_UTF16Chr(SC sc) {
    INPUT inp;
    // const
    inp.type = INPUT_KEYBOARD;
    inp.ki.wVk = 0;
    inp.ki.dwFlags = KEYEVENTF_UNICODE;
    inp.ki.dwExtraInfo = KEYBD_EXTRA_INFO;
    // non-const
    inp.ki.wScan = sc;
    inp.ki.time = GetTickCount();

    //printf("sendinput %x %c\n", sc, sc);
    SendInput(1, &inp, sizeof(INPUT));
}

void SendInput_DownUp(VK vk, SC sc) {
    INPUT inp[2];
    inp[0].type = INPUT_KEYBOARD;
    inp[0].ki.wVk = vk;
    inp[0].ki.wScan = sc;
    inp[0].ki.dwFlags = 0;
    inp[0].ki.dwExtraInfo = KEYBD_EXTRA_INFO;
    inp[0].ki.time = GetTickCount();
    inp[1] = inp[0];
    inp[1].ki.dwFlags = KEYEVENTF_KEYUP;
    SendInput(2, &(inp[0]), sizeof(INPUT));
}

void SendInput_Init() {
}

/* ---------------------------
   Hook actions utilites

  This (KbdHook_Action() array) is used for in-program hooks,
  as such are required for latches,
  so that latch processing is not performed if latch is not active.
  KbdHook_Actions_RunAll() is to be called on each key event.
*/

#define ACTIONS_MAX 15

UCHAR KbdHook_Actions_Count = 0;
KBD_ACTION_FUNC KbdHook_Actions[ACTIONS_MAX + 1];

bool KbdHook_Actions_Install(KBD_ACTION_FUNC kbd_action) {
    printf("installing an action... ");
    if (KbdHook_Actions_Count >= ACTIONS_MAX)
        return false;
    {
        int i;
        KBD_ACTION_FUNC *f;

        f = &(KbdHook_Actions[0]);
        fori (i, 0, KbdHook_Actions_Count) {
            if (*f == kbd_action) {
                printf("refusing to install duplicate kbdhook_action\n");
                return false;
            }
            f++;
        }
    }
    KbdHook_Actions[KbdHook_Actions_Count] = kbd_action;
    KbdHook_Actions_Count += 1;
    KbdHook_Actions[KbdHook_Actions_Count] = nil;
    printf("ok\n");
    return true;
}

bool KbdHook_Actions_Uninstall(KBD_ACTION_FUNC kbd_action) {
    KBD_ACTION_FUNC *f;
    int i;

    printf("uninstalling an action... ");
    f = &(KbdHook_Actions[0]);
    fori (i, 0, KbdHook_Actions_Count) {
        if (*f == kbd_action) {
            int j, jmax = (KbdHook_Actions_Count - 1);

            f = &(KbdHook_Actions[j]);
            fori(j, i, jmax) {
                *f = *(++f);
            }
            KbdHook_Actions_Count = jmax;
            KbdHook_Actions[jmax] = nil;
            printf("ok\n");
            return true;
        }
        f++;
    }
    printf("fail\n");
    return false;
}

// return value indicates whether to let key event through
bool KbdHook_Actions_RunAll(KBD_ACTION_ARGS) {
    int i;
    bool res = true;
    fori (i, 0, KbdHook_Actions_Count) {
        // printf(" .runall.f() [%d/%d]\n", i + 1, KbdHook_Actions_Count);
    	res = res && KbdHook_Actions[i](ev, down);
    }
    return res;
}

// --------------------------
// Keyboard modifiers
//
// Needed by Hotkeys.

#define MOD_ALL (MOD_CONTROL|MOD_ALT|MOD_WIN|MOD_SHIFT)

UCHAR keyboard_mods;

void keyboard_mods_update(bool down, UCHAR bitmask) {
    if (down) {
        keyboard_mods = keyboard_mods | bitmask;
    } else {
        keyboard_mods = keyboard_mods & (MOD_ALL - bitmask);
    }
    //printf(" keyboard_mods: %x {%x}\n", keyboard_mods, bitmask);
}

KbdMod keyboard_mod_ctrl;
KBD_ACTION KbdAction_Ctrl(KBD_ACTION_ARGS) {
    KbdMod_Switch(&keyboard_mod_ctrl, ev->scanCode, down);
    keyboard_mods_update(keyboard_mod_ctrl.count, MOD_CONTROL);
    return true;
}

KBD_ACTION KbdAction_Ctrl_Rebound(KBD_ACTION_ARGS) {
    SC sc = ev->scanCode;
    KbdMod_Switch(&keyboard_mod_ctrl, sc, down);
    keyboard_mods_update(keyboard_mod_ctrl.count, MOD_CONTROL);
    keybd_event(VK_LCONTROL, sc, updnf(0), ev->dwExtraInfo);
    //printf("caps.keybd_event;");
    return false;
}

//bool keyboard_alt;
KbdMod keyboard_mod_alt;
KBD_ACTION KbdAction_Alt(KBD_ACTION_ARGS) {
    KbdMod_Switch(&keyboard_mod_alt, ev->scanCode, down);
    keyboard_mods_update(keyboard_mod_alt.count, MOD_ALT);
    return true;
}

//bool keyboard_win;
KbdMod keyboard_mod_win;
KBD_ACTION KbdAction_Win(KBD_ACTION_ARGS) {
    KbdMod_Switch(&keyboard_mod_win, ev->scanCode, down);
    keyboard_mods_update(keyboard_mod_win.count, MOD_WIN);
    return true;
}

void keyboard_mods_init() {
    KbdMod_Init(&keyboard_mod_ctrl);
    KbdMod_Init(&keyboard_mod_alt);
    KbdMod_Init(&keyboard_mod_win);
}

// ---------------------------
// Key actions
//
// The functions to be assigned to Layout_Key.action slot, and be fired
// on key presses and releases.
// Each action must be a KBD_ACTION_CALLBACK function taking KBD_ACTION_ARGS.
// Return value (a bool) indicates whether to let physical key event through.

// -- Locks

// A ShiftLock action
bool KbdAction_Level2_Lock_State = false;
KBD_ACTION KbdAction_Level2_Lock(KBD_ACTION_ARGS) {
    if (down) {
        KbdAction_Level2_Lock_State = !KbdAction_Level2_Lock_State;
        layout_level2 = (layout_level2 ? 0 : 1);
        keyboard_mods_update(KbdAction_Level2_Lock_State, MOD_SHIFT);
        keybd_event(VK_LEVEL2_SHIFT, (KbdAction_Level2_Lock_State), 0, KEYBD_EXTRA_INFO);
        //printf("%c lock2\n", KbdAction_Level2_Lock_State);
    }
    return false;
}

// A level5 lock action
bool KbdAction_Level5_Lock_State = false;
KBD_ACTION KbdAction_Level5_Lock(KBD_ACTION_ARGS) {
    if (down) {
        KbdAction_Level5_Lock_State = !KbdAction_Level5_Lock_State;
        layout_level5 = (KbdAction_Level5_Lock_State ? 4 : 0);
        //printf("%c lock2\n", KbdAction_Level2_Lock_State);
    }
    return false;
}

// -- Shifts

// An action for LShift and RShift
void KbdAction_Level2_Latch_ReleaseMaybe(void); // forward declaration
KbdMod layout_mod_level2;
KBD_ACTION KbdAction_Level2_Shift(KBD_ACTION_ARGS) {
    KbdAction_Level2_Latch_ReleaseMaybe();
    KbdMod_Switch(&layout_mod_level2, ev->scanCode, down);
    layout_level2 = (KbdAction_Level2_Lock_State ^ layout_mod_level2.count ? 1 : 0);
    keyboard_mods_update(layout_level2, MOD_SHIFT);
    //printf("level2.count: %d %lx => l%d\n", layout_mod_level2.count, ev->scanCode, Layout_GetLevel()+1);
    return true;
}

// An action for level5 shift
KbdMod layout_mod_level5;
KBD_ACTION KbdAction_Level5_Shift(KBD_ACTION_ARGS) {
    KbdMod_Switch(&layout_mod_level5, ev->scanCode, down);
    KbdAction_Level5_Lock_State = (layout_level5 = (layout_mod_level5.count ? 4 : 0));
    //printf("level5.count: %d %lx\n", layout_mod_level5.count, ev->scanCode);
    return false;
}

// -- Latches

// An action for level2 latch
//  holds level2 shift (VK_LEVEL2_SHIFT) until either:
//   a non-mod key is released while still pressing the latch, or
//   any key is released after releasing the latch.
//  (where mod is any modifier key: shift, latch, control, alt, win)

// latch states:
// 0 - inactive
// 1 - latch trigger key pressed but not released
// 2 - latch trigger key released after being pressed
UCHAR KbdAction_Level2_Latch_State = 0;
bool KbdAction_Level2_Latch_OtherKey = 0;
SC KbdAction_Level2_Latch_scanCode = 0;

KBD_ACTION KbdAction_Level2_Latch_HookAction(KBD_ACTION_ARGS); // forward declaration

void KbdAction_Level2_Latch_Release(void) {
    KbdHook_Actions_Uninstall(KbdAction_Level2_Latch_HookAction);
    KbdAction_Level2_Latch_State = 0;
    KbdAction_Level2_Latch_OtherKey = false;
    layout_level2 = 0;
    keyboard_mods_update(0, MOD_SHIFT);
    keybd_event(VK_LEVEL2_SHIFT, KbdAction_Level2_Latch_scanCode, KEYEVENTF_KEYUP, KEYBD_EXTRA_INFO);
    //printf("^ latch2\n");
}

void KbdAction_Level2_Latch_ReleaseMaybe(void) {
    if (KbdAction_Level2_Latch_State > 1) {
        KbdAction_Level2_Latch_Release();
    }
}

KBD_ACTION KbdAction_Level2_Latch(KBD_ACTION_ARGS); // forward declaration
void KbdAction_Level3_Latch_ReleaseMaybe(void); // forward declaration
bool KbdAction_Level3_Latch_OtherKey;
SC KbdAction_Level3_Latch_scanCode;

KBD_ACTION KbdAction_Level2_Latch_HookAction(KBD_ACTION_ARGS) {
    if (!down) {
        SC sc = ev->scanCode;
    	UCHAR level = Layout_GetLevel();
		Layout_Key *lk = &(layout[level][sc]);

        //printf("+ latch2\n");
        if (sc == KbdAction_Level2_Latch_scanCode) { // if (lk->action == KbdAction_Level2_Latch)
            // released any non-mod key while pressing the latch?
            if (KbdAction_Level2_Latch_OtherKey) {
                KbdAction_Level2_Latch_Release();
            } else {
                KbdAction_Level2_Latch_State = 2;
            }
        } else {
            if (!lk->modifier && (sc != KbdAction_Level3_Latch_scanCode)) {
                //printf("= latch2 on %d\n", Layout_GetLevel()+1);
                KbdAction_Level2_Latch_OtherKey = true;
                if (KbdAction_Level2_Latch_State >= 2) {
                    KbdAction_Level2_Latch_Release();
                }
                if (KbdAction_Level3_Latch_OtherKey) {
                    KbdAction_Level3_Latch_ReleaseMaybe();
                }
            }
        }
    }
    return true;
}

KBD_ACTION KbdAction_Level2_Latch(KBD_ACTION_ARGS) {
    if (down && !KbdAction_Level2_Latch_State) {
        KbdAction_Level2_Latch_State = 1;
        KbdAction_Level2_Latch_OtherKey = false;
        layout_level2 = 1;
        keyboard_mods_update(1, MOD_SHIFT);
        KbdHook_Actions_Install(KbdAction_Level2_Latch_HookAction);
        keybd_event(VK_LEVEL2_SHIFT, (KbdAction_Level2_Latch_scanCode = ev->scanCode), 0, KEYBD_EXTRA_INFO);
        //printf("_ latch2\n");
    }
    return false;
}

// An action for AltGr latch
//
// AltGr is either just RAlt (if system layout does not have AltGr),
// or LControl followed by RAlt, which produces characters from layout's AltGr keys level.
// Be LControl physically released while still holding the AltGr-enabled RAlt,
// AltGr would become just RAlt until its release.

void KbdAction_Level3_Latch_ReleaseMaybe(void); // forward declaration

KBD_ACTION KbdAction_AltGr(KBD_ACTION_ARGS) {
    //printf("AltGr action.\n");
    KbdAction_Level3_Latch_ReleaseMaybe();
    layout_level3 = (down ? 2 : 0);
    return true;
}

// An action for level3 latch (AltGr latch)

UCHAR KbdAction_Level3_Latch_State = 0;
bool KbdAction_Level3_Latch_OtherKey = 0;
SC KbdAction_Level3_Latch_scanCode = 0;

KBD_ACTION KbdAction_Level3_Latch_HookAction(KBD_ACTION_ARGS); // forward declaration

void KbdAction_Level3_Latch_Release(void) {
    KbdHook_Actions_Uninstall(KbdAction_Level3_Latch_HookAction);
    KbdAction_Level3_Latch_State = 0;
    KbdAction_Level3_Latch_OtherKey = false;
    layout_level3 = 0;
    keybd_event(VK_LCONTROL, 0x01D, KEYEVENTF_KEYUP, KEYBD_EXTRA_INFO);
    keybd_event(VK_RMENU, 0x038, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, KEYBD_EXTRA_INFO);
    //printf("^ latch3\n");
}

void KbdAction_Level3_Latch_ReleaseMaybe(void) {
    if (KbdAction_Level3_Latch_State > 1) {
        KbdAction_Level3_Latch_Release();
    }
}

KBD_ACTION KbdAction_Level3_Latch(KBD_ACTION_ARGS); // forward declaration

KBD_ACTION KbdAction_Level3_Latch_HookAction(KBD_ACTION_ARGS) {
    if (!down) {
        SC sc = ev->scanCode;
    	UCHAR level = Layout_GetLevel();
		Layout_Key *lk = &(layout[level][sc]);

        if (sc == KbdAction_Level3_Latch_scanCode) {
            //printf("& latch3\n");
            // released any non-mod key while pressing the latch?
            if (KbdAction_Level3_Latch_OtherKey) {
                KbdAction_Level3_Latch_Release();
            } else {
                KbdAction_Level3_Latch_State = 2;
            }
        } else {
            if (!lk->modifier) {
                //printf("= latch3 on %d\n", Layout_GetLevel()+1);
                KbdAction_Level3_Latch_OtherKey = true;
                if (KbdAction_Level3_Latch_State >= 2 && !KbdAction_Level2_Latch_State) {
                    KbdAction_Level3_Latch_Release();
                }
            }
        }
    }
    return true;
}

KBD_ACTION KbdAction_Level3_Latch(KBD_ACTION_ARGS) {
    if (down && !KbdAction_Level3_Latch_State) {
        KbdAction_Level3_Latch_State = 1;
        KbdAction_Level3_Latch_OtherKey = false;
        layout_level3 = 2;
        KbdHook_Actions_Install(KbdAction_Level3_Latch_HookAction);
        KbdAction_Level3_Latch_scanCode = ev->scanCode;
        keybd_event(VK_LCONTROL, 0x01D, 0, KEYBD_EXTRA_INFO);
        keybd_event(VK_RMENU, 0x038, KEYEVENTF_EXTENDEDKEY, KEYBD_EXTRA_INFO);
        //printf("_ latch3\n");
    }
    return false;
}

#if FEAT_LANGS

// -- Layout switchers

HKL KbdLayout_SwitchZero = 0;
void KbdLayout_SwitchNext(int distance) {
    int ln = GetKeyboardLayoutList(0, nil);
    HKL ll[OS_LAYOUT_MAX];
    HWND wf = GetForegroundWindow();
    DWORD wt_id = GetWindowThreadProcessId(wf, nil);
    HKL lc_id = GetKeyboardLayout(wt_id);
    bool no_lc_id;
    int lc;
    #define BUFMAX 256
    char wf_class[BUFMAX];

    GetClassName(wf, wf_class, BUFMAX);
    //printf("class: %s;\n", wf_class);
    #undef BUFMAX
    no_lc_id = !lc_id || !strcmp(wf_class, "ConsoleWindowClass\0");

    GetKeyboardLayoutList(OS_LAYOUT_MAX, ll);
    if (no_lc_id) {
        //printf("!lc_id %lx\n", (ULONG)KbdLayout_SwitchZero);
        if (!KbdLayout_SwitchZero)
            KbdLayout_SwitchZero = ll[0];
        lc_id = KbdLayout_SwitchZero;
        PostMessage(wf, WM_INPUTLANGCHANGEREQUEST, 0, (LPARAM)KbdLayout_SwitchZero);
    }
    {
        fori (lc, 0, ln) {
            if (ll[lc] == lc_id) {
                break;
            }
        }
    }
    //printf("hkl %lx (%lx): %lx %lx %lx\n", (ULONG)lc_id, (ULONG)wt_id, (ULONG)ll[0], (ULONG)ll[1],  (ULONG)ll[2]);
    {
        UCHAR li = ((int)(lc + distance)) % ln;
        HKL lv = ll[li];

        //printf("kbdl_sw: (%d) %d -> %d = %lx; %lx %lx\n", ln, lc, li, (ULONG)lv, (ULONG)wf, wt_id);
        PostMessage(wf, WM_INPUTLANGCHANGEREQUEST, 0, (LPARAM)lv);
        if (no_lc_id) {
            KbdLayout_SwitchZero = lv;
        }
    }
}

KBD_ACTION KbdAction_NextLayout(KBD_ACTION_ARGS) {
    //printf("ka.nl\n");
    if (down)
        KbdLayout_SwitchNext(1);
    return false;
}

KBD_ACTION KbdAction_PrevLayout(KBD_ACTION_ARGS) {
    if (down)
        KbdLayout_SwitchNext(-1);
    return false;
}

#endif

#if FEAT_ACTIONS_UNICODE

// -- UTF-16 input

UCHAR Hotkey_Unicode_input;
UINT16 Hotkey_Unicode_char;

HICON GUI_tray_icon, GUI_tray_utf16_hicon; // forward declaration
bool GUI_tray_update(); // forward declaration

KBD_ACTION Hotkey_Unicode_Action(KBD_ACTION_ARGS) {
    if (!down) {
        return true;
    }
    Hotkey_Unicode_input++;
    {
        SC sc = ev->scanCode;
        UCHAR ch = 0;

        //printf("UNICODE %d %x\n", Hotkey_Unicode_input, sc);
        if (sc == SC_SPACE || sc == SC_N) {
            ch = 0;
        } else if (sc >= SC_J && sc <= SC_L) {
            ch = sc - SC_J + 1;
        } else if (sc >= SC_U && sc <= SC_O) {
            ch = sc - SC_U + 4;
        } else if (sc >= SC_8 && sc <= SC_0) {
            ch = sc - SC_8 + 7;
        } else if (sc >= SC_S && sc <= SC_F) {
            ch = sc - SC_S + 0xA;
        } else if (sc >= SC_W && sc <= SC_R) {
            ch = sc - SC_W + 0xD;
        } else if (sc == SC_H) {
            goto endchar;
        } else if (sc == SC_G || sc == SC_M) {
            goto sendchar;
        } else {
            KbdHook_Actions_Uninstall(Hotkey_Unicode_Action);
            return true;
        }
        //printf("UNICODE (U+%x) += %d\n", Hotkey_Unicode_char, ch);
        Hotkey_Unicode_char *= 0x10;
        Hotkey_Unicode_char += ch;
    }
    if (Hotkey_Unicode_input >= 4) {
        goto sendchar;
    } else {
        return false;
    }

    sendchar:
    //printf("UNICODE U+%x\n", Hotkey_Unicode_char);
    {
        SendInput_UTF16Chr(Hotkey_Unicode_char);
    }
    KbdHook_Actions_Uninstall(Hotkey_Unicode_Action);
    GUI_tray_icon = nil;
    GUI_tray_update();

    endchar:
    return false;
}

KBD_ACTION Hotkey_Unicode(KBD_ACTION_ARGS) {
    if (!down)
        return false;
    Hotkey_Unicode_input = 0;
    Hotkey_Unicode_char = 0;
    KbdHook_Actions_Install(Hotkey_Unicode_Action);
    GUI_tray_icon = GUI_tray_utf16_hicon;
    GUI_tray_update();
    return false;
}

#endif

#if FEAT_ACTIONS_MOUSE

// Mouse moving
//  supports double-press resulting in diagonal motion,
//  but not simultaneous same-direction keys (as Hotkey_Mouse_Pressed has no KbdMod)

bool Hotkey_Mouse_Runall_called = false; // prevent infinite recursion
typedef struct { KBD_ACTION_FUNC action; bool pressed; } Hotkey_Mouse_Pressed;
void Hotkey_Mouse_Pressed_set(Hotkey_Mouse_Pressed *state, UCHAR n, KBD_ACTION_FUNC f, bool down) {
    UCHAR i;
    Hotkey_Mouse_Pressed *p = &(state[0]);
    fori (i, 0, n) {
        if (p->action == f) {
            p->pressed = down;
        }
        p++;
    }
}
KBD_ACTION Hotkey_Mouse_Left(KBD_ACTION_ARGS);
KBD_ACTION Hotkey_Mouse_Right(KBD_ACTION_ARGS);
KBD_ACTION Hotkey_Mouse_Down(KBD_ACTION_ARGS);
KBD_ACTION Hotkey_Mouse_Up(KBD_ACTION_ARGS);
Hotkey_Mouse_Pressed Hotkey_Mouse_State1[] = {
    { Hotkey_Mouse_Left, false }, { Hotkey_Mouse_Right, false },
    { Hotkey_Mouse_Down, false }, { Hotkey_Mouse_Up, false }
};
void Hotkey_Mouse_Runall(Hotkey_Mouse_Pressed *state, UCHAR n, KBD_ACTION_ARGS) {
    if (Hotkey_Mouse_Runall_called)
        return;

    Hotkey_Mouse_Runall_called = true;
    {
        UCHAR i;
        Hotkey_Mouse_Pressed *p = &(state[0]);

        fori (i, 0, n) {
            //printf("p->pressed:%d\n", p->pressed);
            if (p->pressed) {
                p->action(ev, down);
            }
            p++;
        }
    }
    Hotkey_Mouse_Runall_called = false;
}
void Hotkey_Mouse_Move(KBD_ACTION_FUNC f, DWORD x, DWORD y,
                       KBD_ACTION_ARGS,
                       Hotkey_Mouse_Pressed *state, UCHAR state_len) {
    if (down) {
        if (Hotkey_Mouse_Runall_called)
            mouse_event(MOUSEEVENTF_MOVE, x, y, 0, 0);
        Hotkey_Mouse_Pressed_set(state, state_len, f, true);
        Hotkey_Mouse_Runall(state, state_len, ev, down);
    } else {
        Hotkey_Mouse_Pressed_set(state, state_len, f, false);
    }
}

# define x 5
# define y 5
KBD_ACTION Hotkey_Mouse_Left(KBD_ACTION_ARGS) {
    Hotkey_Mouse_Move(Hotkey_Mouse_Left, -x, 0, ev, down, Hotkey_Mouse_State1, 4);
    return false;
}
KBD_ACTION Hotkey_Mouse_Right(KBD_ACTION_ARGS) {
    Hotkey_Mouse_Move(Hotkey_Mouse_Right, x, 0, ev, down, Hotkey_Mouse_State1, 4);
    return false;
}
KBD_ACTION Hotkey_Mouse_Up(KBD_ACTION_ARGS) {
    Hotkey_Mouse_Move(Hotkey_Mouse_Up, 0, -y, ev, down, Hotkey_Mouse_State1, 4);
    return false;
}
KBD_ACTION Hotkey_Mouse_Down(KBD_ACTION_ARGS) {
    Hotkey_Mouse_Move(Hotkey_Mouse_Down, 0, y, ev, down, Hotkey_Mouse_State1, 4);
    return false;
}
# undef x
# undef y

#endif // FEAT_ACTIONS_MOUSE

// --

void KbdAction_Init(){
    KbdMod_Init(&layout_mod_level2);
    KbdMod_Init(&layout_mod_level5);
}

#if FEAT_LANGS

/* ---------------------------
   KbdLang

   Built-in keyboard layouts.
   Each is analogous to one in Layout section:
   consists of Layout_Level-s consisting of Layout_Key-s:
   - 4 levels only: normal, normal+shift, altgr, altgr+shift
   - keys map to (UTF-16) characters only: no to-VK, to-SC, to-KBD_ACTION_CALLBACK remappings
   Keys are SC_TILDE..SC_EQUALS, SC_Q..SC_RBRACE, SC_A..SC_QUOTE, SC_Z..SC_SLASH, SC_BACKSLASH, SC_SPACE
   Number of keys is: 13+12+11+10+1+1 = 48
   Scancodes of keys range from 0x002(SC_1) to 0x039(SC_SPACE) - 64 scancodes is needed
   Size of a layout is 4 [levels] * 64 [keys] * 2 [UTF-16 char size] = 0.5 kilobytes.
*/

// how many layouts user might use
// Note: X11 Xkb has limit of 4
#ifndef LANG_MAX
# define LANG_MAX 8
#endif

// how many key levels to have in a KbdLang layout
#ifndef LANG_LEVELS
# define LANG_LEVELS 4
#endif

#define LANG_KEYS 64

typedef struct {
    SC ch;
} KbdLang_Key;

typedef KbdLang_Key KbdLang_Level[LANG_KEYS];

typedef KbdLang_Level KbdLang_Layout[LANG_LEVELS];

// forward declaration
// Could be redefined in layout.h
KbdLang_Layout KbdLang_layouts[LANG_MAX];

void KbdLang_layouts_Init() {
    UCHAR i, j;
    USHORT k;

    fori (i, 0, LANG_MAX) {
        fori (j, 0, LANG_LEVELS) {
            fori (k, 0, KEYS) {
                KbdLang_Key *lk;

                lk = &(KbdLang_layouts[i][j][k]);
                lk->ch = 0;
            }
        }
    }
}

KbdLang_Layout *KbdLang_Current;
UCHAR KbdLang_CurrentIdx, KbdLang_MaxIdx;

void KbdLang_Next() {
    if ((KbdLang_CurrentIdx += 1) > KbdLang_MaxIdx) {
        KbdLang_CurrentIdx = 0;
    }
    //printf("kl.curidx %d\n", KbdLang_CurrentIdx);
    KbdLang_Current = &(KbdLang_layouts[KbdLang_CurrentIdx]);
}

void KbdLang_Prev() {
    if ((KbdLang_CurrentIdx += 1) > (LANG_MAX - 1)) {
        KbdLang_CurrentIdx = LANG_MAX - 1;
    }
    KbdLang_Current = &(KbdLang_layouts[KbdLang_CurrentIdx]);
}

KBD_ACTION KbdAction_KbdLang_Next(KBD_ACTION_ARGS) {
    //printf("kl.next %d.%d\n", KbdLang_CurrentIdx, KbdLang_MaxIdx);
    if (down) {
        KbdLang_Next();
    }
    return false;
}

KBD_ACTION KbdAction_KbdLang_Prev(KBD_ACTION_ARGS) {
    if (down) {
        KbdLang_Prev();
    }
    return false;
}

typedef struct {
    SC sc;
    SC ch[LANG_LEVELS];
} KbdLang_LayoutDef;

void KbdLang_Install(UCHAR idx, KbdLang_LayoutDef *layout_def) {
    KbdLang_LayoutDef *key = layout_def;
    KbdLang_Layout *layout = &(KbdLang_layouts[idx]);
    USHORT j;

    if (idx > LANG_MAX) {
        return;
    }
    fori (j, 0, LANG_KEYS) {
        SC sc = key->sc;

        if (!sc)
            return;
        if (sc <= LANG_KEYS) {
            UCHAR i;

            fori (i, 0, LANG_LEVELS) {
                SC ch = key->ch[i];

                //printf("i %d.%d %x.%d U+%04x\n", idx, sc, key->sc, i, ch);
                (*layout)[i][sc].ch = ch; // layout[i][sc]->ch would result in either segfault or hook_error
            }
        }
        key++;
    }
    //printf("i end;\n");
}

KbdLang_LayoutDef layout_def_lang_common[]; // forward declaration

bool KbdLang_Push(KbdLang_LayoutDef *layout_def) {
    if ((UCHAR)(KbdLang_MaxIdx+1) == LANG_MAX)
        return false;
    KbdLang_MaxIdx += 1;
    //printf("kl.i %d\n", KbdLang_MaxIdx);
    KbdLang_Install(KbdLang_MaxIdx, layout_def_lang_common);
    KbdLang_Install(KbdLang_MaxIdx, layout_def);
    return true;
}

void KbdLang_InstallLayouts(KbdLang_LayoutDef *layout_def[]) {
    UCHAR i;

    fori (i, 0, LANG_MAX) {
        if (layout_def[i] == nil)
            break;
        KbdLang_Push(layout_def[i]);
    }
}

void KbdLang_Clear() {
    KbdLang_layouts_Init();
    KbdLang_CurrentIdx = 0;
    KbdLang_MaxIdx = -1;
    KbdLang_Current = &(KbdLang_layouts[0]);
}

void KbdLang_Init() {
    KbdLang_Clear();
}

/* ---------------------------
   KbdLang - vk workaround

    Is required in order to get Ctrl+, Alt+, Win+
    keyboard accelerators (undo, cut, copy, etc.)
    while using Unicode character codes independent
    from OS layout, since accelerators are working
    with virtual keycodes (despite that the ACCEL
    resource defaults to be character-based).
*/

typedef struct {
    UCHAR sc;
    VK vk;
} KbdLang_VK_Def;

VK KbdLang_vks[LANG_KEYS];

void KbdLang_VK_Install(KbdLang_VK_Def *v) {
    while (v->vk || v->sc) {
        UCHAR sc = v->sc % LANG_KEYS;
        KbdLang_vks[sc] = v->vk;
        v++;
    }
}

void KbdLang_VK_Init() {
    UCHAR i;
    fori (i, 0, LANG_KEYS) {
        KbdLang_vks[i] = 0;
    }
}

#endif // FEAT_LANGS

/* ---------------------------
   Hook callback

   Is called by OS on each key event.
   It then checks whether the event is real (generated by OS)
   or generated by any program (including itself).
   If event is not real, it is passed through.
   Othewise, KbdHook_Actions_RunAll() is called to call all the in-program hooks
    (for latches and anything else).
   Then, a Layout_Key key definition is looked up for the key.
   If the definition specifies an action (a function), it is called;
   return value determines whether to let event through.
   Otherwise, scancode and virtual keycode are substituted from definition (if non-zero),
   new event is generated, and the original is "stopped" (rejected).
*/

#define RawThisEvent() 0
#define StopThisEvent() 1
#define PassThisEvent() CallNextHookEx(NULL, aCode, wParam, lParam)

bool HotkeysSC_Run(SC sc, KBD_ACTION_ARGS); // forward declaration
bool HotkeysVK_Run(VK vk, KBD_ACTION_ARGS); // forward declaration

bool KbdHook_pass = false;

LRESULT CALLBACK KbdHook_Callback(int aCode, WPARAM wParam, LPARAM lParam) {
    PKBDLLHOOKSTRUCT ev = (PKBDLLHOOKSTRUCT) lParam;
    if (aCode != HC_ACTION)
        return PassThisEvent(); // MSDN
    {
        DWORD flags = ev->flags;
        SC sc = (SC) ev->scanCode;
        VK vk = (VK) ev->vkCode;
        bool down = (wParam != WM_KEYUP && wParam != WM_SYSKEYUP);
        // non-physical key events:
        //   injected key presses (generated by programs - keybd_event(), SendInput()),
        //   their non-injected release counterparts,
        //   fake shift presses and releases by driver accompanying numpad keys
        //     (so that numlock'ed keys are independent of shift state yet have the same 2 levels),
        //   LControl press/release by OS (the window system) triggered by AltGr RAlt event
        // Only check here for injected presses and corresponding releases.
        bool fake;
        bool non_shift_mods = keyboard_mods & (MOD_ALL - MOD_SHIFT);

        // init sc, fake
        if (flags & LLKHF_EXTENDED)
            sc |= 0x100;
        fake = (flags & LLKHF_INJECTED || (!(keystate[sc]) && !down));

        if (fake)
            return PassThisEvent();

        keystate[sc] = down;
        //printf("%c %c %x %d f:%lx i:%lx\n", updn(down), vk, sc, Layout_GetLevel()+1, ev->flags, ev->dwExtraInfo);

        if (!KbdHook_Actions_RunAll(ev, down)) {
            return StopThisEvent();
        }
        //printf(" actions;\n");

        #if (FEAT_HOTKEYS_SC || FEAT_HOTKEYS_VK)
        if (non_shift_mods && !(layout_level3|layout_level5)) {
            #if FEAT_HOTKEYS_SC
            if (HotkeysSC_Run(sc, ev, down)) {
                return StopThisEvent();
            }
            #endif // FEAT_HOTKEYS_SC
            #if FEAT_HOTKEYS_VK
            if (HotkeysVK_Run(vk, ev, down)) {
                return StopThisEvent();
            }
            #endif // FEAT_HOTKEYS_VK
        }
        #endif // (FEAT_HOTKEYS_SC || FEAT_HOTKEYS_VK)
        //printf(" hotkeys;\n");

        {
            UCHAR level = Layout_GetLevel();
            #if FEAT_LANGS
            SC nk;
            #endif // FEAT_LANGS

            #if FEAT_LANGS
            if ((KbdLang_MaxIdx && level < LANG_LEVELS && sc < LANG_KEYS)
                && (nk = (*KbdLang_Current)[level][sc].ch)) {
                if (KbdHook_pass) {
                    return PassThisEvent();
                }
                if (down) {
                    //printf("nk (%d.%d.%d) %03x %04x\n", KbdLang_CurrentIdx, KbdLang_MaxIdx, LAYOUT_KEYS, sc, nk);
                    if (non_shift_mods && !layout_level5) {
                        VK vk1 = KbdLang_vks[sc];
                        //printf("%c nk->vk1 %x %c %x\n", updn(down), vk1, vk1, sc);
                        keybd_event(vk1, sc, updnf(0), KEYBD_EXTRA_INFO);
                    } else {
                        SendInput_UTF16Chr(nk);
                    }
                }
                return StopThisEvent();
            } else
            #endif // FEAT_LANGS
            {
                Layout_Key *lk = &layout[level][sc];

                if (lk->action) {
                    //printf("LK.ACT\n");
                    if (lk->action(ev, down)) {
                        return PassThisEvent();
                    } else {
                        return StopThisEvent();
                    }
                } else {
                    VK vk1 = lk->vk;
                    SC sc1 = lk->sc;
                    DWORD flags = lk->flags;

                    //printf("LK.ELSE\n");
                    if (vk1 || sc1) {
                        sc1 = (sc1 ? sc1 : sc);
                        flags = updnf(flags);
                        //printf("vksc %d %x %x -> %x %x %c %lx\n", (Layout_GetLevel() + 1), vk, sc, vk1, sc1, sc1, flags);
                        if (flags & KEYEVENTF_UNICODE) {
                            if (down) {
                                SendInput_UTF16Chr(sc1);
                            }
                        } else {
                            keybd_event(vk1, sc1, flags, KEYBD_EXTRA_INFO);
                        }
                        return StopThisEvent();
                    } else {
                        return PassThisEvent();
                    }
                }
            }
        }
    }
}

/* ---------------------------
   Hook thread management

   Starts, runs, and stops the hook thread.
*/

DWORD main_threadID = 0;
HINSTANCE main_threadHandle = nil;

HHOOK KbdHook_HookHandle = nil;
#define KbdHook_suspended ((bool) !KbdHook_HookHandle)
enum KbdHook_ThreadMessages {
    KBDHOOK_CHANGE_HOOK_STATE
};

#define KBDHOOK_INSTALL 0x01
DWORD WINAPI KbdHook_ThreadProc(LPVOID arg_unused) {
    MSG msg;
    bool problem = false;
    for (;;) {
        if (GetMessage(&msg, nil, 0, 0)==-1) continue; // -1 is an error, 0 is WM_QUIT.
        switch (msg.message) {
            case WM_QUIT: msg.wParam = 0; // fall to next case
            case KBDHOOK_CHANGE_HOOK_STATE:
                if (msg.wParam & KBDHOOK_INSTALL) {
                    KbdHook_HookHandle =
                        SetWindowsHookEx(WH_KEYBOARD_LL,
                                         KbdHook_Callback,
                                         nil, // MSDN: not main_threadHandle
                                         0);
                    problem = !KbdHook_HookHandle;
                } else {
                    if (KbdHook_HookHandle) {
                        UnhookWindowsHookEx(KbdHook_HookHandle);
                        KbdHook_HookHandle = nil;
                    }
                }
                if (msg.wParam) PostThreadMessage(main_threadID, KBDHOOK_CHANGE_HOOK_STATE, problem, 0);
                if (!KbdHook_HookHandle) return 0; // terminate if running no longer
            break;
        }
    }
}

HANDLE KbdHook_threadHandle;
DWORD KbdHook_threadID;

bool KbdHook_Run() {
    MSG msg;
    int i;

    printf("starting kbdhook...\n");
    // Win9x: Last parameter cannot be nil.
    KbdHook_threadHandle = CreateThread(nil, 8*1024, KbdHook_ThreadProc, nil, 0, &KbdHook_threadID);
    if (KbdHook_threadHandle) {
        // avoid lag
        SetThreadPriority(KbdHook_threadHandle, THREAD_PRIORITY_TIME_CRITICAL);
    }

    fori (i, 0, 50) {
        Sleep(10);
        if (PostThreadMessage(KbdHook_threadID, KBDHOOK_CHANGE_HOOK_STATE, (KBDHOOK_INSTALL), 0))
            break;
    }

    {
        DWORD start_time = GetTickCount();
        bool problem = true;
        while (start_time > (GetTickCount() - 500)) {
            if (PeekMessage(&msg, nil, KBDHOOK_CHANGE_HOOK_STATE, KBDHOOK_CHANGE_HOOK_STATE, PM_REMOVE)) {
                problem = msg.wParam;
                break;
            }
        }
        return problem;
    }
}

bool KbdHook_Suspend() {
    MSG msg;
    int i;

    printf("suspending kbdhook...\n");
    fori (i, 0, 50) {
       if (PostThreadMessage(KbdHook_threadID, KBDHOOK_CHANGE_HOOK_STATE, 0, 0)) {
            break;
        }
        Sleep(10);
    }

    {
        DWORD start_time = GetTickCount();
        bool problem = true;
        while (start_time > (GetTickCount() - 500)) {
            if (PeekMessage(&msg, nil, KBDHOOK_CHANGE_HOOK_STATE, KBDHOOK_CHANGE_HOOK_STATE, PM_REMOVE)) {
                problem = msg.wParam;
                break;
            }
        }
        return problem;
    }
}

bool GUI_tray_update(); // forward declaration

void KbdHook_Toggle() {
    if (KbdHook_suspended) {
        KbdHook_Run();
    } else {
        KbdHook_Suspend();
    }
    #if FEAT_GUI
    GUI_tray_update();
    #endif // FEAT_GUI
}

// ---------------------------
// GUI

#if FEAT_GUI

#define GUI_TRAY_MSG (WM_APP + 1)

HANDLE main_moduleHandle;

HWND GUI_main_window;

char GUI_main_window_title[] = "l5\0";
char GUI_main_window_class[] = "l5\0";
#define GUI_tray_title GUI_main_window_title

int GUI_trayID = 1;

void GUI_quit(int); // forward declaration

/*
   Tray icon ("notify icon") procedure.
   Creates, modifies, and removes the tray notification icon
   based on action argument (NIM_ADD, NIM_MODIFY, NIM_DESTROY).
*/

HICON
    GUI_tray_icon = 0,
    GUI_tray_hicon,
    GUI_tray_pause_hicon,
    GUI_tray_pass_hicon,
    GUI_tray_utf16_hicon;

bool GUI_tray_ctl(DWORD action, char *tooltip_str) {
    NOTIFYICONDATA nid;

    nid.cbSize = sizeof nid;
    nid.hWnd = GUI_main_window;
	nid.uID = GUI_trayID;
	nid.uFlags = NIF_ICON | NIF_MESSAGE;
	nid.uCallbackMessage = GUI_TRAY_MSG;
	nid.hIcon = (KbdHook_suspended ? GUI_tray_pause_hicon : GUI_tray_icon ? GUI_tray_icon : GUI_tray_hicon);

	if(tooltip_str) {
		nid.uFlags |= NIF_TIP;
		lstrcpyn(nid.szTip, tooltip_str, (sizeof nid.szTip/sizeof nid.szTip[0]));
	}

	return (bool) Shell_NotifyIcon(action, &nid);
}

bool GUI_tray_update() {
    return GUI_tray_ctl(NIM_MODIFY, nil);
}

#define GUI_STR_LEN 128
typedef char GUI_STR_BUF[GUI_STR_LEN];

GUI_STR_BUF
    GUI_STR_suspend = "Suspend",
    GUI_STR_pass = "No remaps",
    GUI_STR_quit = "Exit";

void GUI_main_window_show_tray_menu() {

	SetForegroundWindow(GUI_main_window);

	// construct and show the menu
	{
	    POINT mpt;
        HMENU m = CreatePopupMenu();
	    // menu commands
	    int
          id_suspend,
          id_pass,
          id_exit;

        // fill the menu
        {
            int id = 16;

            AppendMenu(m, (KbdHook_suspended ? MF_CHECKED : 0), (id_suspend = id++), GUI_STR_suspend);
            AppendMenu(m, (KbdHook_pass ? MF_CHECKED : 0), (id_pass = id++), GUI_STR_pass);
            AppendMenu(m, MF_SEPARATOR, 0, nil);
            AppendMenu(m, 0, (id_exit = id++), GUI_STR_quit);
        }

        // Popup
        GetCursorPos(&mpt);
        {
            // Show menu
            BOOL cmd = TrackPopupMenuEx(m, TPM_RETURNCMD, mpt.x, mpt.y, GUI_main_window, 0);

            // Run the action
            if (cmd == id_exit) {
                GUI_quit(0);
            } else if (cmd == id_suspend) {
                KbdHook_Toggle();
            } else if (cmd == id_pass) {
                KbdHook_pass = !KbdHook_pass;
                GUI_tray_icon = (KbdHook_pass ? GUI_tray_pass_hicon : nil);
                GUI_tray_update();
            }
        }

	}
}

#define GUI_HOTKEY_PROC void CALLBACK
typedef GUI_HOTKEY_PROC (*GUI_HOTKEY_PROC_FUNC)(WPARAM, LPARAM);
GUI_HOTKEY_PROC_FUNC GUI_hotkey_proc;

bool HotkeysOS_Run(HWND, UINT, WPARAM, LPARAM); // forward declaration

LRESULT CALLBACK GUI_main_window_proc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case GUI_TRAY_MSG:
            //printf("GUI_TRAY_MSG\n");
            switch (lParam) {
                case WM_RBUTTONDOWN:
                    GUI_main_window_show_tray_menu();
                    break;
            }
            break;
        #if FEAT_HOTKEYS_OS
        case WM_HOTKEY:
            printf("WM_HOTKEY\n");
            HotkeysOS_Run(hWnd, uMsg, wParam, lParam);
            break;
        #endif
        case WM_CLOSE:
            GUI_quit(0);
            return 0;
    }
    return DefWindowProc(hWnd, uMsg ,wParam, lParam);
}

void GUI_cleanup() {
    GUI_tray_ctl(NIM_DELETE, nil);
}

void GUI_quit(int status) {
    GUI_cleanup();
    PostQuitMessage(status);
}

bool GUI_Init() {
    // class for GUI_main_window
    {
        WNDCLASSEX w;

        w.cbClsExtra = 0;
        w.cbSize = sizeof w;
        w.cbWndExtra = 0;
        w.hbrBackground = 0;
        w.hCursor = 0;
        w.hIcon = 0; //LoadIcon(main_threadHandle, MAKEINTRESOURCE(IDI_APPLICATION));
        w.hIconSm = w.hIcon;
        w.hInstance = main_moduleHandle;
        w.lpfnWndProc = GUI_main_window_proc;
        w.lpszClassName = GUI_main_window_class;
        w.lpszMenuName = 0;
        w.style = CS_HREDRAW | CS_VREDRAW;

        if(!RegisterClassEx(&w))
            return false;
    }

    // display GUI_main_window
	{
	    GUI_main_window = CreateWindowEx(0,
                                         GUI_main_window_class,
                                         GUI_main_window_title,
                                         WS_OVERLAPPEDWINDOW,
                                         CW_USEDEFAULT,
                                         CW_USEDEFAULT,
                                         CW_USEDEFAULT,
                                         CW_USEDEFAULT,
                                         0,
                                         0,
                                         main_moduleHandle,
                                        0);
        if (!GUI_main_window) {
            return false;
        }

        ShowWindow(GUI_main_window, SW_HIDE);
	}


    // load tray data, display the icon
    {
        #define s(id, var) LoadString(main_moduleHandle, id, var, GUI_STR_LEN)
        s(IDS_SUSPEND, GUI_STR_suspend);
        s(IDS_PASS, GUI_STR_pass);
        s(IDS_QUIT, GUI_STR_quit);
        #undef s

        #define icon(var, resource) var = LoadIcon(main_moduleHandle, MAKEINTRESOURCE(resource))
        icon(GUI_tray_hicon, IDI_ICON_TRAY);
        icon(GUI_tray_pause_hicon, IDI_ICON_TRAY_PAUSE);
        icon(GUI_tray_pass_hicon,  IDI_ICON_TRAY_PASS);
        icon(GUI_tray_utf16_hicon, IDI_ICON_TRAY_UTF16);
        #undef icon

        GUI_tray_ctl(NIM_ADD, GUI_tray_title);
    }

    return true;
}

void GUI_Loop() {
    for (;;) {
        int r;
        MSG msg;

        r = GetMessage(&msg, GUI_main_window, 0, 0);
        if (r <= 0)
            break;

        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

#endif // FEAT_GUI

#if (FEAT_HOTKEYS_OS || FEAT_HOTKEYS_SC || FEAT_HOTKEYS_VK)

/*  ---------------------------
    Common for all the hotkeys.

    The hotkeys are managed as Hotkeys struct.

    To check for and run a hotkey,
    Hotkeys.keys is accessed (by SC, VK, etc.).
    A key is an index into Hotkeys.all_hotkeys array.

    A hotkey contains modifiers to check for, an action to call if they match.
    The action is KBD_ACTION_CALLBACK and is called on both key press(es) and release.
    For the the first of keys, .count is valid (of hotkeys for this key).
    Each of count hotkeys (count-1 following) is examined until any one matches.

    The index is used only during the adding or
    removing of hotkeys, to simplify the search
    [in Hotkeys.keys] for owner of
    preceding/following all_hotkeys slice
    (the slice is an hotkey array).

*/

#define HOTKEYS_MAX 64
#define HOTKEYS_POS UCHAR

// size: 3 + arch bytes
typedef struct {
    KBD_ACTION_FUNC action; // an action to call if .mods match modifiers in effect
    UCHAR mods; // keyboard modifiers for this hotkey
    // for Hotkey_Key's first hotkey only
    UCHAR count;
} Hotkeys_Hotkey;

typedef struct {
    HOTKEYS_POS all_hotkeys_count;
    Hotkeys_Hotkey all_hotkeys[HOTKEYS_MAX];
    /*
    VK-based hotkeys actually need 256 only.
    To index into keys[], SC type is used.
    */
    HOTKEYS_POS keys[KEYS];
    /*
    The mapping from all_hotkeys[] index to keys[] index
    Only used during adding or removing of hotkeys.
    */
    SC all_hotkeys_keys[HOTKEYS_MAX];
} Hotkeys;

Hotkeys_Hotkey* Hotkeys_sc(Hotkeys *hotkeys, SC sc) {
    HOTKEYS_POS p = hotkeys->keys[sc];
    printf("found hks_sc %d for sc%03x\n", p, sc);
    return (p ? &(hotkeys->all_hotkeys[p]) : nil);
}

void Hotkeys_Hotkey_Init(Hotkeys_Hotkey *hk) {
    hk->action = nil;
    hk->mods = 0;
    hk->count = 0;
}

void Hotkeys_Init(Hotkeys *hks) {
    int i;
    hks->all_hotkeys_count = 0;
    {
        Hotkeys_Hotkey *ah = &(hks->all_hotkeys[0]);
        fori (i, 0, HOTKEYS_MAX) {
            Hotkeys_Hotkey_Init(ah);
            ah++;
        }
    }
    fori (i, 0, KEYS) {
        hks->keys[i] = 0;
    }
    fori (i, 0, HOTKEYS_MAX) {
        hks->all_hotkeys_keys[i] = 0;
    }
}

void Hotkeys_AllHotkeys_MoveToEnd(Hotkeys *hotkeys, SC sc) {
    Hotkeys_Hotkey *h0, *ha;
    HOTKEYS_POS p0, l0, hl, *hp;
    SC *hk;

    ha = hotkeys->all_hotkeys;
    hk = hotkeys->all_hotkeys_keys;
    hl = hotkeys->all_hotkeys_count;
    hp = hotkeys->keys;

    p0 = hp[sc];
    h0 = &(ha[p0]);
    l0 = h0->count;

    printf("moving sc%03x-%d from %d to end-%d... ", sc, l0, p0, hl);

    if (!l0) {
        return;
    }

    // move the Hotkeys_Hotkeys slice belonging to Hotkeys_Keys[keyval] to the end

    // for each Key that follows, swap the HotKey slices:
    //  if following is larger, swap with its tail
    //  if same or shorter, swap it with current head
    while (p0 + l0 < hl) {
        Hotkeys_Hotkey *h1 = h0 + l0;
        HOTKEYS_POS l1 = h1->count;
        SC sc1 = hk[p0 + l0];

        l1 = h1->count;
        {
            Hotkeys_Hotkey h2;
            HOTKEYS_POS hi;
            if (l0 < l1) {
                HOTKEYS_POS ld = l1 - l0;
                fori (hi, 0, l0) { h2=h0[hi]; h0[hi]=h1[ld+hi]; h1[ld+hi]=h2; hk[p0+hi]=sc1; }
                h0->count = l1;
                hp[sc1] -= ld;
                p0 += ld;
                h0 += ld;
            } else {
                fori (hi, 0, l1) { h2=h0[hi]; h0[hi]=h1[hi]; h1[hi]=h2; hk[p0+hi]=sc1; }
                h0->count = l1;
                hp[sc1] -= l0;
                p0 += l1;
                h0 += l1;
            }
        }
    }

    hp[sc] = p0;
    hk[p0] = sc;

    printf("moved to %d\n", p0);

    return;
}

// return value indicates whether the hotkey was registered
bool Hotkeys_Register(Hotkeys *hotkeys, SC sc, UCHAR mods, KBD_ACTION_FUNC action) {
    Hotkeys_Hotkey *h;
    HOTKEYS_POS l = 0, la;

    la = hotkeys->all_hotkeys_count;
    h = Hotkeys_sc(hotkeys, sc);
    if (h)
        l = h->count;

    // replace existing hotkey if any
    {
        HOTKEYS_POS i;

        fori (i, 0, l) {
            if (h->mods == mods) {
                h->action = action;
                return true;
            }
            h++;
        }
        h-=l;
    }

    // insert { mods, action } to the end

    if (la >= HOTKEYS_MAX)
        return false;

    h = &(hotkeys->all_hotkeys[la]);
    hotkeys->all_hotkeys_keys[la] = sc;

    if (l) {
        Hotkeys_AllHotkeys_MoveToEnd(hotkeys, sc);
    } else {
        h->count = 0;
        hotkeys->keys[sc] = la;
    }

    h->mods = mods;
    h->action = action;

    (h - l)->count++;
    hotkeys->all_hotkeys_count++;

    return true;
}

// return value indicates whether a hotkey was executed
bool Hotkeys_Run(Hotkeys *hotkeys, SC sc, KBD_ACTION_ARGS) {
    UCHAR i, l;
    Hotkeys_Hotkey *h;

    h = Hotkeys_sc(hotkeys, sc);
    if (h == nil)
        return false;
    l = h->count;
    fori (i, 0, l) {
        printf("hks.sc%03x mods=%x kbdm=%x\n", sc, h->mods, keyboard_mods);
        if (h->mods == keyboard_mods) {
            h->action(ev, down);
            return true;
        }
        h++;
    }
    return false;
}

typedef struct {
    SC sc;
    UCHAR mods;
    KBD_ACTION_FUNC action;
} Hotkeys_KeyDef;

void Hotkeys_Bind_Keys(Hotkeys *hotkeys, Hotkeys_KeyDef *kd) {
    while (kd->sc || kd->mods || kd->action) {
        Hotkeys_Register(hotkeys, kd->sc, kd->mods, kd->action);
        kd++;
    }
}
#endif // (FEAT_HOTKEYS_OS || FEAT_HOTKEYS_SC || FEAT_HOTKEYS_VK)


#if FEAT_HOTKEYS_OS

/*  --------------------------
    OS-managed hotkeys

    These hotkeys are VK-based and rely on RegisterHotKey().
    They could not receive the key release event,
    but they do not depend on keyboard hook.
*/

Hotkeys HotkeysOS;

#define HOTKEY_OS void
#define HOTKEY_OS_ARGS void
typedef HOTKEY_OS (*HOTKEY_OS_FUNC)(HOTKEY_OS_ARGS);

#define HOTKEYS_OS_ID int

# if FEAT_GUI
#  define HOTKEYS_OS_WINDOW GUI_main_window
HWND HOTKEYS_OS_WINDOW; // forward declaration
# else
#  define HOTKEYS_OS_WINDOW 0
# endif

HOTKEYS_OS_ID HotkeysOS_Count;

// returns 0 on fail
HOTKEYS_OS_ID HotkeysOS_Register(Hotkeys_KeyDef *key) {
    bool success;
    UCHAR id;

    Hotkeys_Register(&HotkeysOS, (VK)key->sc, key->mods, key->action);
    id = (HotkeysOS_Count += 1);
    success = RegisterHotKey(HOTKEYS_OS_WINDOW, id, key->mods, key->sc);
    printf("%s to add OS hotkey %x_%x(%c)\n", (success ? "ok" : "fail"), key->mods, key->sc, key->sc);
    return (success ? id : 0);
}

void HotkeysOS_Register_Keys(Hotkeys_KeyDef *keys) {
    while (keys->sc) {
        HotkeysOS_Register(keys);
        keys++;
    }
}

void HotkeysOS_Unregister(HOTKEYS_OS_ID id) {
    UnregisterHotKey(HOTKEYS_OS_WINDOW, id);
}

bool HotkeysOS_Run(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    VK vk = lParam>>16;
    bool r;
    r = Hotkeys_Run(&HotkeysOS, vk, nil, 0);
    printf("hk_os.run ak%03x %d\n", vk, r);
    return r;
}

void HotkeysOS_Init(Hotkeys_KeyDef *keys) {
    HotkeysOS_Count = 0;
    HotkeysOS_Register_Keys(keys);
}

#endif // FEAT_HOTKEYS_OS

#if FEAT_HOTKEYS_SC

/* ---------------------------
   Scancode Hotkeys
*/

Hotkeys HotkeysSC;

bool HotkeysSC_Run(SC sc, KBD_ACTION_ARGS) {
    return Hotkeys_Run(&HotkeysSC, sc, ev, down);
}

void HotkeysSC_Init(Hotkeys_KeyDef *kd) {
    Hotkeys_Init(&HotkeysSC);
    Hotkeys_Bind_Keys(&HotkeysSC, kd);
}

#endif // FEAT_HOTKEYS_SC

#if FEAT_HOTKEYS_VK

/* ---------------------------
   Virtual Keycode Hotkeys
*/

Hotkeys HotkeysVK;

bool HotkeysVK_Run(VK vk, KBD_ACTION_ARGS) {
    return Hotkeys_Run(&HotkeysVK, vk, ev, down);
}

void HotkeysVK_Init(Hotkeys_KeyDef *kd) {
    Hotkeys_Init(&HotkeysVK);
    Hotkeys_Bind_Keys(&HotkeysVK, kd);
}

#endif // FEAT_HOTKEYS_VK

/*  ---------------------------
    On-no-key-pressed hotkeys

    This is utilites for hotkeys which hold their action until all keys are up.
*/

/*  On-no-key-pressed hotkey->keyaction->call macro
    Defines:
        KBD_ACTION_CALLBACK keyaction()
            Checks whether all the keys are up, and if,
            uninstalls itself, and executes func.
        KBD_ACTION_CALLBACK hotkey()
            Installs the keyaction [to be executed on each key event].
            Could be used as an action for a VK- or SC- based hotkey.
        HOTKEYS_OS hotkeyos()
            Same as hotkey, but could be used as an action for OS-based hotkey.
*/
#define HOTKEY_ONNOKEY_1(hotkey, keyaction, hotkeyos, func) \
    KBD_ACTION keyaction(KBD_ACTION_ARGS);     \
    KBD_ACTION hotkey(KBD_ACTION_ARGS) {       \
        printf("called hk "#hotkey"\n"); \
        if (down) {                                       \
            Hotkeys_SuppressWinMaybe();                   \
        } else {                                          \
            KbdHook_Actions_Install(keyaction);           \
        }                                                 \
        return false;                                     \
    }                                                     \
    KBD_ACTION keyaction(KBD_ACTION_ARGS) {      \
        printf("called ka "#keyaction"\n"); \
        if (!down) {                                      \
            if (keystate_none()) {                        \
                KbdHook_Actions_Uninstall(keyaction);     \
                func();                                   \
                return false;                             \
            }                                             \
        }                                                 \
        return false;                                     \
    }
#define HOTKEY_ONNOKEY(hotkey, func) \
    HOTKEY_ONNOKEY_1(hotkey, hotkey##_keyaction, hotkey##_os, func)

/*  Suppressor for opening up the Start menu when Windows key has been pressed,
    but no OS-managed hotkey fired.
    Returns whether a win key is down, and thus was suppressed.

    This function works by sending VK_0 if a win key is down.
*/
bool Hotkeys_SuppressWinMaybe() {
    if (keyboard_mods & MOD_WIN) {
        SendInput_DownUp('0', 0x00B);
        return true;
    }
    return false;
}

/*  -----------------------------
    OS hotkeys definitions

*/

#if FEAT_ACTIONS_ONOFF

// On-Off toggling
HOTKEY_OS HotkeyOS_Toggle(HOTKEY_OS_ARGS) {
    KbdHook_Toggle();
}

// On-Pass-through toggling
HOTKEY_OS HotkeyOS_PassthroughToggle(HOTKEY_OS_ARGS) {
    printf("PT toggle...\n");
    if (KbdHook_suspended) {
        KbdHook_pass = false;
        KbdHook_Run();
    }
    KbdHook_pass = !KbdHook_pass;
    #if FEAT_GUI
    GUI_tray_icon = (KbdHook_pass ? GUI_tray_pass_hicon : nil);
    GUI_tray_update();
    #endif
    return;
}

#endif // FEAT_ACTIONS_ONOFF

#if FEAT_ACTIONS_POWER

// Screen dimming
HOTKEY_OS HotkeyOS_DimScreen() {
    printf("dimming\n");
    SendMessage(HWND_BROADCAST, WM_SYSCOMMAND, SC_MONITORPOWER, (LPARAM) 2);
}
HOTKEY_ONNOKEY(HotkeyNK_DimScreen, HotkeyOS_DimScreen);

// Sleeping
HOTKEY_OS HotkeyOS_Suspend() {
    SetSuspendState(false, false, false);
}
HOTKEY_ONNOKEY(HotkeyNK_Suspend, HotkeyOS_Suspend);

// Hibernation
HOTKEY_OS HotkeyOS_Hibernate() {
    SetSuspendState(true, false, false);
}
HOTKEY_ONNOKEY(HotkeyNK_Hibernate, HotkeyOS_Hibernate);

#endif // FEAT_ACTIONS_POWER

/* ---------------------------
   main function
*/

#include "layout.h"

//int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
int main(int argc, char *argv[])
{
    main_threadID = GetCurrentThreadId();
    main_threadHandle = GetCurrentThread();
    main_moduleHandle = GetModuleHandle(0);

    keyboard_mods_init();
    Keystate_Init();
    KbdAction_Init();
    SendInput_Init();

    Layout_BindKeys(layout_def_keys);
    Layout_BindChrs(layout_def_chrs);
    Layout_BindActs(layout_def_acts);
    Layout_BindMods(layout_def_mods);

    KbdLang_Init();
    KbdLang_InstallLayouts(layout_def_layouts);
    KbdLang_VK_Install(layout_def_vks);

#if FEAT_HOTKEYS_OS
    HotkeysOS_Init(layout_def_os_hotkeys);
#endif // FEAT_HOTKEYS_OS
#if FEAT_HOTKEYS_SC
    Hotkeys_SC_Init(layout_def_sc_hotkeys);
#endif // FEAT_HOTKEYS_SC
#if FEAT_HOTKEYS_VK
    HotkeysVK_Init(layout_def_vk_hotkeys);
#endif // FEAT_HOTKEYS_VK
#if FEAT_DLL
    {
        int i;
        fori(i, 1, argc) {
            LoadLibrary(argv[i]);
        }
    }
#endif // FEAT_DLL
    {
        bool hook_error = KbdHook_Run();
        if (hook_error)
            return 1;
    }
#if FEAT_GUI
    if (!GUI_Init())
        return 2;
    GUI_Loop();
#else // FEAT_GUI
    SuspendThread(main_threadHandle);
#endif // FEAT_GUI
    return 0;
}
