#ifndef LAYOUT_H_INCLUDED
#define LAYOUT_H_INCLUDED

#include "scancodes.h"

// Hotkeys

#define h(vk, mods, action) { vk, mods, (KBD_ACTION_FUNC)action }
#if FEAT_HOTKEYS_OS
Hotkeys_KeyDef layout_def_os_hotkeys[] = {
    #if FEAT_ACTIONS_ONOFF
    h(VK_F2, MOD_WIN, HotkeyOS_PassthroughToggle),
    h(VK_F2, MOD_WIN|MOD_SHIFT, HotkeyOS_Toggle),
    #endif
    // end
    { 0, 0, 0 }
};
#endif
#undef h

#define h(sc, mods, action) { sc, mods, action }
#if FEAT_HOTKEYS_SC
Hotkeys_KeyDef layout_def_sc_hotkeys[] = {
    #if FEAT_ACTIONS_MOUSE
    h(SC_7, MOD_WIN, Hotkey_Mouse_Left),
    h(SC_8, MOD_WIN, Hotkey_Mouse_Down),
    h(SC_9, MOD_WIN, Hotkey_Mouse_Up),
    h(SC_0, MOD_WIN, Hotkey_Mouse_Right),
    #endif
    // end
    { 0, 0, 0 }
};
#endif
#undef h

#define h(vk, mods, action) { vk, mods, action }
#if FEAT_HOTKEYS_VK
Hotkeys_KeyDef layout_def_vk_hotkeys[] = {
    #if FEAT_ACTIONS_POWER
    h('Z', MOD_WIN, HotkeyNK_DimScreen),
    //
    h(VK_F2, MOD_CONTROL|MOD_ALT, HotkeyNK_Suspend),
    h(VK_F2, MOD_CONTROL|MOD_ALT|MOD_SHIFT, HotkeyNK_Hibernate),
    #endif
    //
    #if FEAT_ACTIONS_UNICODE
    h('U', MOD_ALT, Hotkey_Unicode),
    #endif
    // end
    { 0, 0, 0}
};
#endif
#undef h

// Utilites

#define l1(sc, remap) { sc, 1, remap }, { sc, 2, remap }
#define l3(sc, remap) { sc, 3, remap }, { sc, 4, remap }
#define l5(sc, remap) { sc, 5, remap }, { sc, 6, remap }

#define l0(sc, remap) l1(sc, remap), l3(sc, remap), l5(sc, remap)
#define l15(sc, remap) l1(sc, remap), l5(sc, remap)
#define l35(sc, remap) l3(sc, remap), l5(sc, remap)

#define l34(sc, remap1, remap2) { sc, 3, remap1 }, { sc, 4, remap2 }
#define l56(sc, remap1, remap2) { sc, 5, remap1 }, { sc, 6, remap2 }

#define m1(sc) { sc, 1 }, { sc, 2 }
#define m3(sc) { sc, 3 }, { sc, 4 }
#define m5(sc) { sc, 5 }, { sc, 6 }

#define m0(sc) m1(sc), m3(sc), m5(sc)
#define m15(sc) m1(sc), m5(sc)
#define m35(sc) m3(sc), m5(sc)

// Layout

Layout_BindMod_Key layout_def_mods[] = {
    m0(SC_LSHIFT), m0(SC_RSHIFT),
    m0(SC_LCONTROL), m0(SC_RCONTROL),
    m0(SC_LMENU), m0(SC_RMENU),
    m0(SC_LWIN), m0(SC_RWIN),
    //
    m0(SC_CAPITAL),
    //
    m0(SC_SEMICOLON),
    m35(SC_S),
    //
    { SC_COMMA, 1 },
    // end
    { 0, 0 }
};

Layout_BindAct_Key layout_def_acts[] = {
    l0(SC_LWIN, KbdAction_Win),
    l0(SC_RWIN, KbdAction_Win),
    l0(SC_LMENU, KbdAction_Alt),
    l0(SC_RMENU, KbdAction_Alt),
    l0(SC_LCONTROL, KbdAction_Ctrl),
    l0(SC_RCONTROL, KbdAction_Ctrl),
    l0(SC_CAPITAL, KbdAction_Ctrl_Rebound),
    //
    l0(SC_LSHIFT, KbdAction_Level2_Shift),
    l0(SC_RSHIFT, KbdAction_Level2_Shift),
    l0(SC_RMENU, KbdAction_AltGr),
    //
    l15(SC_SEMICOLON, KbdAction_Level5_Shift),
    l35(SC_S, KbdAction_Level2_Latch),
    l35(SC_SLASH, KbdAction_Level5_Lock),
    //
    //l34(SC_M, KbdAction_NextLayout, KbdAction_PrevLayout),
    l34(SC_M, KbdAction_KbdLang_Next, KbdAction_KbdLang_Prev),
    //
    { SC_COMMA, 1, KbdAction_Level3_Latch },
    // end
    { 0, 0, nil }
};

Layout_BindKey_Key layout_def_keys[] = {
    l0(SC_CAPITAL, VK_LCONTROL),
    //
    l3(SC_PERIOD, VK_APPS),
    //
    l5(SC_T, VK_TAB),
    l5(SC_H, VK_LEFT), l5(SC_J, VK_DOWN), l5(SC_K, VK_UP), l5(SC_L, VK_RIGHT),
    l5(SC_U, VK_BACK), l5(SC_O, VK_DELETE),
    l5(SC_N, VK_NEXT), l5(SC_Y, VK_PRIOR),
    l5(SC_COMMA, VK_HOME), l5(SC_PERIOD, VK_END),
    l5(SC_M, VK_RETURN), l5(SC_I, VK_ESCAPE),
    l5(SC_QUOTE, VK_INSERT), l5(SC_P, VK_PRINT), l5(SC_SLASH, VK_PAUSE),
    // end
    { 0, 0, 0 }
};

#define c l1
#define c3 l34
#define c5 l56
Layout_BindChr_Key layout_def_chrs[] = {
    c3(SC_J, '_', '_'),
    c3(SC_L, ';', '`'),
    c3(SC_QUOTE, '\'', '"'),
    c3(SC_R, '#', '@'),
    c3(SC_X, '~', '$'),
    c3(SC_C, '\\', '|'),
    //
    c5(SC_1, '&', '&'),
    c5(SC_2, '+', '+'),
    c5(SC_3, '<', '<'),
    c5(SC_4, '>', '>'),
    c5(SC_5, '-', '_'),
    c5(SC_6, '^', '^'),
    c5(SC_7, 0x2018, 0x201C),
    c5(SC_8, 0x201A, 0x201E),
    c5(SC_9, 0x201B, 0x201F),
    c5(SC_0, 0x2019, 0x201D),
    //
    c5(SC_Q, '!', ','),
    c5(SC_W, ':', '`'),
    c5(SC_E, '(', '@'),
    c5(SC_R, '=', '#'),
    //
    c5(SC_A, '/', '?'),
    c5(SC_D, '{', '}'),
    c5(SC_F, '[', ']'),
    c5(SC_G, ')', '*'),
    //
    c5(SC_Z, '%', '%'),
    c5(SC_X, '$', '`'),
    c5(SC_C, '\\', '|'),
    c5(SC_V, '"', '\''),
    c5(SC_B, ';', '/'),
    // end
    { 0, 0, 0 }
};
#undef c

#define c13(sc, ch1, ch2, ch3, ch4) { sc, { ch1, ch2, ch3, ch4 } }
#define c(sc, ch1, ch2) c13(sc, ch1, ch2, 0, 0)

KbdLang_LayoutDef layout_def_lang_none[] = {
    // end
    { 0, { 0, 0, 0, 0 } }
};

KbdLang_LayoutDef layout_def_lang_common[] = {
    c(SC_1, '1', '!'),
    c(SC_2, '2', '@'),
    c(SC_3, '3', '#'),
    c(SC_4, '4', '$'),
    c(SC_5, '5', '%'),
    c(SC_6, '6', '^'),
    c(SC_7, '7', '&'),
    c(SC_8, '8', '*'),
    c(SC_9, '9', '('),
    c(SC_0, '0', ')'),
    //
    c(SC_TILDE, '`', '~'),
    c(SC_MINUS, '-', '_'),
    c(SC_EQUAL, '=', '+'),
    c(SC_BACKSLASH, '\\', '|'),
    //
    c(SC_LBRACE, '[', '{'),
    c(SC_RBRACE, ']', '}'),
    //
    c(SC_QUOTE, ',', ':'),
    //
    c(SC_PERIOD, '.', '>'),
    c(SC_SLASH, '/', '?'),
    // end
    { 0, { 0, 0, 0, 0 } }
};

KbdLang_LayoutDef layout_def_lang_qw[] = {
    c(SC_Q, 'q', 'Q'),
    c(SC_W, 'w', 'W'),
    c(SC_E, 'a', 'A'),
    c(SC_R, 'f', 'F'),
    c(SC_T, 'g', 'G'),
    c(SC_Y, 'y', 'Y'),
    c(SC_U, 'u', 'U'),
    c(SC_I, 'o', 'O'),
    c(SC_O, 'p', 'P'),
    c(SC_P, 'k', 'K'),
    //
    c(SC_A, 'v', 'V'),
    c(SC_S, 's', 'S'),
    c(SC_D, 'e', 'E'),
    c(SC_F, 'r', 'R'),
    c(SC_G, 't', 'T'),
    c(SC_H, 'h', 'H'),
    c(SC_J, 'n', 'N'),
    c(SC_K, 'i', 'I'),
    c(SC_L, 'l', 'L'),
    //
    c(SC_Z, 'z', 'Z'),
    c(SC_X, 'x', 'X'),
    c(SC_C, 'c', 'C'),
    c(SC_V, 'd', 'D'),
    c(SC_B, 'b', 'B'),
    c(SC_N, 'j', 'J'),
    c(SC_M, 'm', 'M'),
    // end
    { 0, { 0, 0, 0, 0 } }
};

KbdLang_LayoutDef layout_def_lang_qw_ru[] = {
    // IO = U+0401
    // A..YA = U+0410..U+042F
    // a..ya = U+0430..U+044F
    // io = U+0451
    c(SC_Q, 0x044b, 0x042b), // yeru
    c13(SC_W, 0x0448, 0x0428, 0x0449, 0x0429), // sha, shcha
    c(SC_E, 0x0430, 0x0410), // a
    c13(SC_R, 0x043a, 0x041a, 0x0444, 0x0424), // ka, ef
    c(SC_T, 0x0433, 0x0413), // ghe
    c(SC_Y, 0x0439, 0x0419), // short i
    c13(SC_U, 0x0443, 0x0423, 0x044e, 0x042e), // u, yu
    c(SC_I, 0x043e, 0x041e), // o
    c(SC_O, 0x043f, 0x041f), // pe
    c13(SC_P, 0x044c, 0x042c, 0x044a, 0x042a), // soft sign, hard sign
    //
    c(SC_A, 0x0447, 0x0427), // che
    c(SC_S, 0x0441, 0x0421), // es
    c13(SC_D, 0x0435, 0x0415, 0x0451, 0x0401), // ie, io
    c(SC_F, 0x0440, 0x0420), // er
    c(SC_G, 0x0442, 0x0422), // te
    c13(SC_H, 0x0432, 0x0412, 0x044d, 0x042d), // ve, e
    c(SC_J, 0x043d, 0x041d), // en
    c(SC_K, 0x0438, 0x0418), // i
    c(SC_L, 0x043b, 0x041b), // el
    //
    c(SC_Z, 0x0437, 0x0417), // ze
    c13(SC_X, 0x0436, 0x0416, 0x0445, 0x0425), // zhe, ha
    c(SC_C, 0x0446, 0x0426), // tse
    c(SC_V, 0x0434, 0x0414), // de
    c(SC_B, 0x0431, 0x0411), // be
    c(SC_N, 0x044f, 0x042f), // ya
    c(SC_M, 0x043c, 0x041c), // em
    // end
    { 0, { 0, 0, 0, 0 } }
};

#undef c
#undef c13

KbdLang_LayoutDef *layout_def_layouts[] = {
    layout_def_lang_qw,
    layout_def_lang_qw_ru,
    // end
    nil
};

// virtual keycodes
// shared by all langs
// are used with ctrl,alt,win modifiers
#define v(sc, vk) { sc, vk }
KbdLang_VK_Def layout_def_vks[] = {
    v(SC_1, '1'),
    v(SC_2, '2'),
    v(SC_3, '3'),
    v(SC_4, '4'),
    v(SC_5, '5'),
    v(SC_6, '6'),
    v(SC_7, '7'),
    v(SC_8, '8'),
    v(SC_9, '9'),
    v(SC_0, '0'),
    //
    v(SC_TILDE, VK_OEM_3),
    v(SC_MINUS, VK_OEM_MINUS),
    v(SC_EQUAL, VK_OEM_PLUS),
    v(SC_BACKSLASH, VK_OEM_5),
    //
    v(SC_Q, 'Q'),
    v(SC_W, 'W'),
    v(SC_E, 'A'),
    v(SC_R, 'F'),
    v(SC_T, 'G'),
    v(SC_Y, 'Y'),
    v(SC_U, 'U'),
    v(SC_I, 'O'),
    v(SC_O, 'P'),
    v(SC_P, 'K'),
    v(SC_LBRACE, VK_OEM_4),
    v(SC_RBRACE, VK_OEM_6),
    //
    v(SC_A, 'V'),
    v(SC_S, 'S'),
    v(SC_D, 'E'),
    v(SC_F, 'R'),
    v(SC_G, 'T'),
    v(SC_H, 'H'),
    v(SC_J, 'N'),
    v(SC_K, 'I'),
    v(SC_L, 'L'),
    v(SC_SEMICOLON, VK_OEM_1),
    v(SC_QUOTE, VK_OEM_7),
    //
    v(SC_Z, 'Z'),
    v(SC_X, 'X'),
    v(SC_C, 'C'),
    v(SC_V, 'D'),
    v(SC_B, 'B'),
    v(SC_N, 'J'),
    v(SC_M, 'M'),
    v(SC_COMMA, VK_OEM_COMMA),
    v(SC_PERIOD, VK_OEM_PERIOD),
    v(SC_SLASH, VK_OEM_2),
    // end
    { 0, 0 }
};
#undef v

#endif // LAYOUT_H_INCLUDED
