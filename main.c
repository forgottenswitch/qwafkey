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

*/

#include <windows.h>
//#include <winnt.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// ---------------------------
// Constants

#define KEYS 256
#define LEVELS 8
#define SEND_ID 0x85

// ---------------------------
// Keycode constants

#define LK_LEVEL2_SHIFT VK_LSHIFT

// ---------------------------
// General utilites

#define nil NULL
#define fori(counter, min, max) for(counter = (min); counter < (max); counter++)
#define forkeys(counter) fori(counter, 0, KEYS)
#define forlevels(counter) fori(counter, 0, LEVELS)

// ---------------------------
// winnt.h

#ifndef KEYEVENTF_UNICODE
#define KEYEVENTF_UNICODE 0x00000004
#define KEYEVENTF_SCANCODE 0x00000008
#endif

// ---------------------------
// Keyboard physical state

bool keystate[KEYS];

void Keystate_Init(void) {
    int i;
    forkeys(i) {
        keystate[i] = false;
    }
}

// ---------------------------
// Modifiers state

// Level2/3/5/7 state.
// Win/Control/Alt state.
// Is modified by Layout level key actions.
UCHAR // layout_levelN must be either 0 or N-1
  layout_level2 = 0, // shift state
  layout_level3 = 0, // altgr state
  layout_level5 = 0, // level5 state
  layout_level7 = 0; // numlock state
bool layout_win = false; // win state

UCHAR Layout_GetLevel() {
    UCHAR level = layout_level2 + layout_level3 + layout_level5 + layout_level7;
    level = min(LEVELS - 1, level);
    return level;
}

// ---------------------------
// Layout

#define KBD_ACTION_PARAMS KBDLLHOOKSTRUCT *ev, bool down
typedef bool (CALLBACK *KBD_ACTION_CALLBACK_FUNC)(KBD_ACTION_PARAMS);
#define KBD_ACTION_CALLBACK bool CALLBACK

#define SC USHORT
#define VK UCHAR

typedef struct {
    SC sc; // 2 bytes
    VK vk; // 1 byte
    UCHAR modifier; // 1 byte
    DWORD flags; // 4 bytes
    // is fired on physical event if non-nil
    KBD_ACTION_CALLBACK_FUNC action; // platform bits
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
    forlevels(i) {
        forkeys(j) {
            Layout_Key_Init(&layout[i][j]);
        }
    }
}

// ---------------------------
// Layout defining

typedef struct {
    UCHAR key;
    UCHAR level;
} Layout_BindMod_Key;

typedef struct {
    UCHAR key;
    UCHAR level;
    KBD_ACTION_CALLBACK_FUNC action;
} Layout_BindAct_Key;

typedef struct {
    UCHAR key;
    UCHAR level;
    UCHAR vk;
} Layout_BindKey_Key;

typedef struct {
    UCHAR key;
    UCHAR level;
    USHORT unicode_chr;
} Layout_BindChr_Key;

void Layout_BindMod(VK key, UCHAR level) {
    layout[level - 1][key].modifier = true;
}

void Layout_BindMods(Layout_BindMod_Key *keys) {
    for(; keys->key && keys->level; keys++) {
        Layout_BindMod(keys->key, keys->level);
    }
}

void Layout_BindAct(VK key, UCHAR level, KBD_ACTION_CALLBACK_FUNC action) {
    Layout_Key *lk = &layout[level - 1][key];
    Layout_Key_Init(lk);
    lk->action = action;
}

void Layout_BindActs(Layout_BindAct_Key *keys) {
    for (; keys->key && keys->level && (keys->action != nil); keys++) {
        Layout_BindAct(keys->key, keys->level, keys->action);
    }
}

void Layout_BindKey(VK key, UCHAR level, VK vk) {
    Layout_Key *lk = &layout[level - 1][key];
    Layout_Key_Init(lk);
    lk->vk = vk;
}

void Layout_BindKeys(Layout_BindKey_Key *keys) {
    for (; keys->key && keys->level && keys->vk; keys++) {
        Layout_BindKey(keys->key, keys->level, keys->vk);
    }
}

void Layout_BindChr(VK key, UCHAR level, USHORT unicode_chr) {
    Layout_Key *lk = &layout[level - 1][key];
    Layout_Key_Init(lk);
    lk->sc = unicode_chr;
    lk->flags = KEYEVENTF_UNICODE;
}

void Layout_BindChrs(Layout_BindChr_Key *keys) {
    for (; keys->key || keys->level || keys->unicode_chr; keys++) {
        Layout_BindChr(keys->key, keys->level, keys->unicode_chr);
    }
}

// ---------------------------
// Key actions
//
// Each action must be a KBD_ACTION_CALLBACK function taking KBD_ACTION_PARAMS.
// Return value (a bool) indicates whether to let physical key event through.

#define MOD_CAPACITY_MAX 4

typedef struct {
    UCHAR count, capacity;
    VK keys[MOD_CAPACITY_MAX];
} KbdAction_Mod;

void KbdAction_Mod_Init(KbdAction_Mod *mod) {
    mod->capacity = MOD_CAPACITY_MAX;
    mod->count = 0;
    UCHAR i;
    VK *key = &(mod->keys[0]);
    fori(i, 0, MOD_CAPACITY_MAX) {
        *key = 0;
        key++;
    }
}

void KbdAction_Mod_Press(KbdAction_Mod *mod, VK vk) {
    UCHAR i, len = mod->count;
    VK *keys = &(mod->keys[0]) - 1;
    fori(i, 0, len) {
        if (*(keys++) == vk) {
            return;
        }
    }
    if (len >= mod->capacity) {
    } else {
        *(keys++) = vk;
        mod->count += 1;
    }
}

void KbdAction_Mod_Release(KbdAction_Mod *mod, VK vk) {
    UCHAR i, len = mod->count;
    VK *keys = &(mod->keys[0]) - 1;
    fori(i, 0, len) {
        if (*(keys++) == vk) {
            UCHAR j, len1 = (len - 1);
            VK *key1 = keys + 1;
            fori(j, i, len1) {
                *keys = *key1;
                keys++;
                key1++;
            }
            mod->count -= 1;
            break;
        }
    }
}

void KbdAction_Mod_Switch(KbdAction_Mod *mod, VK vk, bool down) {
    if (down) {
        KbdAction_Mod_Press(mod, vk);
    } else {
        KbdAction_Mod_Release(mod, vk);
    }
}

// ---------------------------
// Hook actions

#define ACTIONS_MAX 15
typedef void (CALLBACK *KbdHook_Action)(KBD_ACTION_PARAMS);

UCHAR KbdHook_Actions_Count = 0;
KbdHook_Action KbdHook_Actions[ACTIONS_MAX + 1];

bool KbdHook_Actions_Install(void (CALLBACK *kbd_action)(KBD_ACTION_PARAMS)) {
    if (KbdHook_Actions_Count >= ACTIONS_MAX)
        return false;
    KbdHook_Actions[KbdHook_Actions_Count] = kbd_action;
    KbdHook_Actions_Count += 1;
    KbdHook_Actions[KbdHook_Actions_Count] = nil;
    return true;
}

bool KbdHook_Actions_Uninstall(void (CALLBACK *kbd_action)(KBD_ACTION_PARAMS)) {
    void (CALLBACK *f)(KBD_ACTION_PARAMS);
    int i;
    fori(i, 0, KbdHook_Actions_Count) {
        f = KbdHook_Actions[i];
        if (f == kbd_action) {
            int j;
            fori(j, i, KbdHook_Actions_Count) {
                KbdHook_Actions[j] = KbdHook_Actions[j + 1];
            }
            KbdHook_Actions[(KbdHook_Actions_Count -= 1)] = nil;
            return true;
        }
    }
    return false;
}

void KbdHook_Actions_RunAll(KBD_ACTION_PARAMS) {
    int i;
    fori (i, 0, KbdHook_Actions_Count) {
        // printf(" .runall.f() [%d/%d]\n", i + 1, KbdHook_Actions_Count);
    	KbdHook_Actions[i](ev, down);
    }
}

// ---------------------------
// Layout level key actions

// An action for Win keys
KbdAction_Mod layout_mod_win;
KBD_ACTION_CALLBACK KbdAction_Win(KBD_ACTION_PARAMS) {
    KbdAction_Mod_Switch(&layout_mod_win, ev->vkCode, down);
    layout_win = layout_mod_win.count;
    return true;
}

void KbdAction_Level2_Latch_ReleaseMaybe(void); // forward declaration

// An action for LShift and RShift
KbdAction_Mod layout_mod_level2;
KBD_ACTION_CALLBACK KbdAction_Level2_Shift(KBD_ACTION_PARAMS) {
    KbdAction_Level2_Latch_ReleaseMaybe();
    KbdAction_Mod_Switch(&layout_mod_level2, ev->vkCode, down);
    layout_level2 = (layout_mod_level2.count ? 1 : 0);
    return true;
}

// An action for level5 shift
KbdAction_Mod layout_mod_level5;
KBD_ACTION_CALLBACK KbdAction_Level5_Shift(KBD_ACTION_PARAMS) {
    KbdAction_Mod_Switch(&layout_mod_level5, ev->vkCode, down);
    layout_level5 = (layout_mod_level5.count ? 4 : 0);
    return false;
}

// An action for level2 latch
//  holds level2 shift (LK_LEVEL2_SHIFT) until either:
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

void CALLBACK KbdAction_Level2_Latch_HookAction(KBD_ACTION_PARAMS); // forward declaration

void KbdAction_Level2_Latch_Release(void) {
    //printf("  .release_l2_latch() %d\n", KbdHook_Actions_Count);
    KbdHook_Actions_Uninstall(KbdAction_Level2_Latch_HookAction);
    //printf("  .release_l2_latch() => %d\n", KbdHook_Actions_Count);
    KbdAction_Level2_Latch_State = 0;
    KbdAction_Level2_Latch_OtherKey = false;
    layout_level2 = 0;
    keybd_event(LK_LEVEL2_SHIFT, KbdAction_Level2_Latch_scanCode, KEYEVENTF_KEYUP, SEND_ID);
}

void KbdAction_Level2_Latch_ReleaseMaybe(void) {
    if (KbdAction_Level2_Latch_State > 1) {
        KbdAction_Level2_Latch_Release();
    }
}

KBD_ACTION_CALLBACK KbdAction_Level2_Latch(KBD_ACTION_PARAMS); // forward declaration

void CALLBACK KbdAction_Level2_Latch_HookAction(KBD_ACTION_PARAMS) {
    if (!down) {
        //printf(" .runall.l2_latch()\n");
        Layout_Key *lk = &(layout[Layout_GetLevel()][ev->vkCode]);
        if (ev->scanCode == KbdAction_Level2_Latch_scanCode) { // if (lk->action == KbdAction_Level2_Latch)
            //printf(" .runall.l2_latch().latch %d\n", KbdAction_Level2_Latch_State);
            // released any non-mod key while pressing the latch?
            if (KbdAction_Level2_Latch_OtherKey) {
                KbdAction_Level2_Latch_Release();
            } else {
                KbdAction_Level2_Latch_State = 2;
            }
        } else {
            //printf(" .runall.l2_latch().else %d\n", lk->modifier);
            if (!lk->modifier) {
                KbdAction_Level2_Latch_OtherKey = true;
                if (KbdAction_Level2_Latch_State >= 2) {
                    KbdAction_Level2_Latch_Release();
                }
            }
        }
    }
    return;
}

KBD_ACTION_CALLBACK KbdAction_Level2_Latch(KBD_ACTION_PARAMS) {
    if (down && !KbdAction_Level2_Latch_State) {
        KbdAction_Level2_Latch_State = 1;
        KbdAction_Level2_Latch_OtherKey = false;
        layout_level2 = 1;
        KbdHook_Actions_Install(KbdAction_Level2_Latch_HookAction);
        keybd_event(LK_LEVEL2_SHIFT, (KbdAction_Level2_Latch_scanCode = ev->scanCode), 0, SEND_ID);
    }
    return false;
}

// An action for AltGr
//
// AltGr is either just RAlt (if system layout does not have AltGr),
// or LControl followed by RAlt, which produces characters from layout's AltGr keys level.
// Be LControl physically released while still holding the AltGr-enabled RAlt,
// AltGr would become just RAlt until its release.

void KbdAction_Level3_Latch_ReleaseMaybe(void); // forward declaration

KBD_ACTION_CALLBACK KbdAction_AltGr(KBD_ACTION_PARAMS) {
    printf("AltGr action.\n");
    KbdAction_Level3_Latch_ReleaseMaybe();
    layout_level3 = (down ? 2 : 0);
    return true;
}

// An action for level3 latch (AltGr latch)

UCHAR KbdAction_Level3_Latch_State = 0;
bool KbdAction_Level3_Latch_OtherKey = 0;
SC KbdAction_Level3_Latch_scanCode = 0;

void CALLBACK KbdAction_Level3_Latch_HookAction(KBD_ACTION_PARAMS); // forward declaration

void KbdAction_Level3_Latch_Release(void) {
    printf("  .release_l3_latch() %d\n", KbdHook_Actions_Count);
    KbdHook_Actions_Uninstall(KbdAction_Level3_Latch_HookAction);
    printf("  .release_l3_latch() => %d\n", KbdHook_Actions_Count);
    KbdAction_Level3_Latch_State = 0;
    KbdAction_Level3_Latch_OtherKey = false;
    layout_level3 = 0;
    keybd_event(VK_LCONTROL, KbdAction_Level3_Latch_scanCode, KEYEVENTF_KEYUP, SEND_ID);
    keybd_event(VK_RMENU, KbdAction_Level3_Latch_scanCode, KEYEVENTF_KEYUP, SEND_ID);
}

void KbdAction_Level3_Latch_ReleaseMaybe(void) {
    if (KbdAction_Level3_Latch_State > 1) {
        KbdAction_Level3_Latch_Release();
    }
}

KBD_ACTION_CALLBACK KbdAction_Level3_Latch(KBD_ACTION_PARAMS); // forward declaration

void CALLBACK KbdAction_Level3_Latch_HookAction(KBD_ACTION_PARAMS) {
    if (!down) {
        //printf(" .runall.l3_latch()\n");
        Layout_Key *lk = &(layout[Layout_GetLevel()][ev->vkCode]);
        if (ev->scanCode == KbdAction_Level3_Latch_scanCode) {
            printf(" .runall.l3_latch().latch %d\n", KbdAction_Level3_Latch_State);
            // released any non-mod key while pressing the latch?
            if (KbdAction_Level3_Latch_OtherKey) {
                KbdAction_Level3_Latch_Release();
            } else {
                KbdAction_Level3_Latch_State = 2;
            }
        } else {
            printf(" .runall.l3_latch().else %d %d\n", lk->modifier, KbdAction_Level3_Latch_State);
            if (!lk->modifier) {
                KbdAction_Level3_Latch_OtherKey = true;
                if (KbdAction_Level3_Latch_State >= 2) {
                    KbdAction_Level3_Latch_Release();
                }
            }
        }
    }
    return;
}

KBD_ACTION_CALLBACK KbdAction_Level3_Latch(KBD_ACTION_PARAMS) {
    if (down && !KbdAction_Level3_Latch_State) {
        KbdAction_Level3_Latch_State = 1;
        KbdAction_Level3_Latch_OtherKey = false;
        layout_level3 = 2;
        KbdHook_Actions_Install(KbdAction_Level3_Latch_HookAction);
        KbdAction_Level3_Latch_scanCode = ev->scanCode;
        keybd_event(VK_LCONTROL, KbdAction_Level3_Latch_scanCode, 0, SEND_ID);
        printf("vk_rmenu...\n");
        keybd_event(VK_RMENU, KbdAction_Level3_Latch_scanCode, 0, SEND_ID);
    }
    return false;
}

//

void KbdAction_Init(){
    KbdAction_Mod_Init(&layout_mod_win);
    KbdAction_Mod_Init(&layout_mod_level2);
    KbdAction_Mod_Init(&layout_mod_level5);
}

// ---------------------------
// Hook callback

#define RawThisEvent() 0
#define StopThisEvent() 1
#define PassThisEvent() CallNextHookEx(NULL, aCode, wParam, lParam)

LRESULT CALLBACK KbdHook_Callback(int aCode, WPARAM wParam, LPARAM lParam) {
    if (aCode != HC_ACTION)
        return PassThisEvent(); // MSDN
    PKBDLLHOOKSTRUCT ev = (PKBDLLHOOKSTRUCT) lParam;
    SC sc = (SC) ev->scanCode;
    VK vk = (VK) ev->vkCode;
    bool down = (wParam != WM_KEYUP && wParam != WM_SYSKEYUP);
    // non-physical key events (without numpad shifts and altgr control):
    //   injected key presses,
    //   their non-injected release counterparts,
    //   key events with non-nil dwExtraInfo (ones sent by programs, not OS)
    bool fake = (ev->flags == LLKHF_INJECTED || (!(keystate[vk]) && !down) || ev->dwExtraInfo);

    if (fake)
        return PassThisEvent();
    keystate[vk] = down;

    UCHAR level = Layout_GetLevel();
    Layout_Key *lk = &layout[level][vk];

    printf("%c %c %d %d \n",
           (down ? 'p' : 'r'), vk, vk,
           level+1);

    KbdHook_Actions_RunAll(ev, down);
    //printf(" .runall()\n");

    if (lk->action) {
        bool pass = lk->action(ev, down);
        if (pass) {
            return PassThisEvent();
        } else {
            return StopThisEvent();
        }
    } else {
        VK vk1 = lk->vk;
        SC sc1 = lk->sc;
        DWORD flags = lk->flags;

        if (vk1 || sc1) {
            sc1 = (sc1 ? sc1 : sc);
            flags = (down ? flags : flags | KEYEVENTF_KEYUP);
            // printf("keybd_event %c %d %d [%c %d %ld] %ld\n", vk1, vk1, sc1, lk->vk, lk->vk, lk->sc, flags);
            keybd_event(vk1, sc1, flags, SEND_ID);
            return StopThisEvent();
        } else {
            return PassThisEvent();
        }
    }
}

// ---------------------------
// Hook thread management

DWORD main_threadID = 0;
HINSTANCE main_threadHandle = nil;

HHOOK KbdHook_HookHandle = nil;
enum KbdHook_ThreadMessages {
    KBDHOOK_CHANGE_HOOK_STATE
};

#define KbdHook_ThreadMessageID 0x01
DWORD WINAPI KbdHook_ThreadProc(LPVOID arg_unused) {
    MSG msg;
    bool problem = false;
    for (;;) {
        if (GetMessage(&msg, nil, 0, 0)==-1) continue; // -1 is an error, 0 is WM_QUIT.
        switch (msg.message) {
            case WM_QUIT: msg.wParam = 0; // fall to next case
            case KBDHOOK_CHANGE_HOOK_STATE:
                if (msg.wParam & KbdHook_ThreadMessageID) {
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
    // Win9x: Last parameter cannot be nil.
    KbdHook_threadHandle = CreateThread(nil, 8*1024, KbdHook_ThreadProc, nil, 0, &KbdHook_threadID);
    if (KbdHook_threadHandle) {
        SetThreadPriority(KbdHook_threadHandle, THREAD_PRIORITY_TIME_CRITICAL); // avoid lag
    }
    int i;
    fori (i, 0, 50) {
        Sleep(10);
        if (PostThreadMessage(KbdHook_threadID, KBDHOOK_CHANGE_HOOK_STATE, (KbdHook_ThreadMessageID), 0))
            break;
    }

    DWORD start_time = GetTickCount();
    bool problem = true;
    while (start_time > (GetTickCount() - 500)) {
        if (PeekMessage(&msg, nil, KBDHOOK_CHANGE_HOOK_STATE, KBDHOOK_CHANGE_HOOK_STATE, PM_REMOVE)) {
            problem = msg.wParam;
        }
    }
    return problem;
}

// ---------------------------
// main function

#include "layout.h"

int main()
{
    main_threadID = GetCurrentThreadId();
    main_threadHandle = GetCurrentThread();
    Keystate_Init();
    KbdAction_Init();
    Layout_BindKeys(layout_def_keys);
    Layout_BindChrs(layout_def_chrs);
    Layout_BindActs(layout_def_acts);
    Layout_BindMods(layout_def_mods);
    bool hook_error = KbdHook_Run();
    if (hook_error)
        return 1;
    // Sleep forever
    SuspendThread(main_threadHandle);
    return 0;
}
