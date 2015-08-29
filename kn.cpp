#include "kn.h"
#include "scancodes.h"

typedef struct {
    VK vk;
    char *name;
} KN_Pair;

#define kn(vk, name) { vk, name }
#define k1(vk) kn(vk, #vk)
#define k2(vk, name) kn(vk, #vk), kn(vk, name)
KN_Pair KN_pairs[] = {
    k1(VK_SPACE),
    k1(VK_TAB),
    k2(VK_RETURN, "enter"),
    k2(VK_ESCAPE, "esc"),
    kn(VK_BACK, "backspace"),
    kn(VK_BACK, "bs"),
    k2(VK_DELETE, "del"),
    k2(VK_INSERT, "ins"),
    k1(VK_HOME),
    k1(VK_END),
    k1(VK_PRIOR),
    k1(VK_NEXT),
    k1(VK_UP),
    k1(VK_DOWN),
    k1(VK_LEFT),
    k1(VK_RIGHT),
    k1(VK_SCROLL),
    k1(VK_CAPITAL),
    k1(VK_NUMLOCK),
    k1(VK_NUMPAD0),
    k1(VK_NUMPAD1),
    k1(VK_NUMPAD2),
    k1(VK_NUMPAD3),
    k1(VK_NUMPAD4),
    k1(VK_NUMPAD5),
    k1(VK_NUMPAD6),
    k1(VK_NUMPAD7),
    k1(VK_NUMPAD8),
    k1(VK_NUMPAD9),
    kn(VK_DECIMAL, "numpaddot"),
    kn(VK_DELETE, "numpaddel"),
    kn(VK_DIVIDE, "numpaddiv"),
    kn(VK_MULTIPLY, "numpadmult"),
    kn(VK_ADD, "numpadadd"),
    kn(VK_SUBTRACT, "numpadsub"),
    kn(VK_RETURN, "numpadenter"),
    k2(VK_APPS, "appskey"),
    kn(VK_APPS, "menu"),
    k2(VK_PRINT, "printscreen"),
    k2(VK_PAUSE, "break"),
    k1(VK_HELP),
    k1(VK_SLEEP),
    k1(VK_BROWSER_BACK),
    k1(VK_BROWSER_FORWARD),
    k1(VK_BROWSER_REFRESH),
    k1(VK_BROWSER_STOP),
    k1(VK_BROWSER_SEARCH),
    k1(VK_BROWSER_FAVORITES),
    k1(VK_BROWSER_HOME),
    k1(VK_VOLUME_MUTE),
    k1(VK_VOLUME_DOWN),
    k1(VK_VOLUME_UP),
    k1(VK_MEDIA_NEXT_TRACK),
    k1(VK_MEDIA_PREV_TRACK),
    k1(VK_MEDIA_STOP),
    k1(VK_MEDIA_PLAY_PAUSE),
    k1(VK_LAUNCH_MAIL),
    k1(VK_LAUNCH_MEDIA_SELECT),
    k1(VK_LAUNCH_APP1),
    k1(VK_LAUNCH_APP2),
};
#undef kn
#undef k1
#undef k2

VK KN_name_to_vk(char *name) {
    size_t i;
    fori (i, 0, len(KN_pairs)) {
        KN_Pair *p = KN_pairs + i;
        if (!_strcmpi(p->name, name)) {
            return p->vk;
        }
    }
    return 0;
}

void KN_init() {
    size_t i;
    fori (i, 0, len(KN_pairs)) {
        KN_Pair *p = KN_pairs + i;
        char *name = p->name;
        if (name[0] == 'V' && name[1] == 'K' && name[2] == '_') {
            p->name = name + 3;
        }
    }
}

typedef struct {
    char *name;
    SC sc;
} KN_LPair;

#define k(name, sc) { name, sc }
KN_LPair KN_lpairs[] = {
    /* Common */
    k("Escape", SC_ESCAPE),
    k("Esc", SC_ESCAPE),
    k("F1", SC_F1),
    k("F2", SC_F2),
    k("F3", SC_F3),
    k("F4", SC_F4),
    k("F5", SC_F5),
    k("F6", SC_F6),
    k("F7", SC_F7),
    k("F8", SC_F8),
    k("F9", SC_F9),
    k("F10", SC_F10),
    k("F11", SC_F11),
    k("F12", SC_F12),

    k("BS", SC_BACK),
    k("Backspace", SC_BACK),
    k("Return", SC_RETURN),
    k("Enter", SC_RETURN),
    k("Tab", SC_TAB),
    k("Space", SC_SPACE),
    k("AppsKey", SC_APPS),
    k("Apps", SC_APPS),
    k("Menu", SC_APPS),

    k("PrintScreen", SC_PRINT),
    k("Print", SC_PRINT),
    k("SysRq", SC_PRINT),
    k("ScrollLock", SC_SCROLL),
    k("PauseBreak", SC_PAUSE),
    k("Pause", SC_PAUSE),
    k("Break", SC_PAUSE),
    k("CtrlBreak", SC_PAUSE),

    k("Insert", SC_INSERT),
    k("Ins", SC_INSERT),
    k("Home", SC_HOME),
    k("Prior", SC_PRIOR),
    k("PgUp", SC_PRIOR),
    k("PageUp", SC_PRIOR),
    k("Delete", SC_DELETE),
    k("Del", SC_DELETE),
    k("Next", SC_PRIOR),
    k("PgDn", SC_PRIOR),
    k("PgDown", SC_PRIOR),
    k("PageDn", SC_PRIOR),
    k("PageDown", SC_PRIOR),

    k("Up", SC_UP),
    k("Dn", SC_DOWN),
    k("Down", SC_DOWN),
    k("Left", SC_LEFT),
    k("Right", SC_RIGHT),

    /* QWERTY */
    k("qTilde", SC_TILDE),
    k("q1", SC_1),
    k("q2", SC_2),
    k("q3", SC_3),
    k("q4", SC_4),
    k("q5", SC_5),
    k("q6", SC_6),
    k("q7", SC_7),
    k("q8", SC_8),
    k("q9", SC_9),
    k("q0", SC_0),
    k("qMinus", SC_MINUS),
    k("qPlus", SC_EQUAL),
    k("qEquals", SC_EQUAL),
    k("qEqual", SC_EQUAL),
    k("qBackslash", SC_BACKSLASH),

    k("qQ", SC_Q),
    k("qW", SC_W),
    k("qE", SC_E),
    k("qR", SC_R),
    k("qT", SC_T),
    k("qY", SC_Y),
    k("qU", SC_U),
    k("qI", SC_I),
    k("qO", SC_O),
    k("qP", SC_P),
    k("qLBracket", SC_LBRACE),
    k("qLBrace", SC_LBRACE),
    k("qRBracket", SC_RBRACE),
    k("qRBrace", SC_RBRACE),

    k("qA", SC_A),
    k("qS", SC_S),
    k("qD", SC_D),
    k("qF", SC_F),
    k("qG", SC_G),
    k("qH", SC_H),
    k("qI", SC_I),
    k("qJ", SC_J),
    k("qK", SC_K),
    k("qL", SC_L),
    k("qSemicolon", SC_SEMICOLON),
    k("qSemic", SC_SEMICOLON),
    k("qSemi", SC_SEMICOLON),
    k("qQuote", SC_QUOTE),
    k("qApostrophe", SC_QUOTE),

    k("qZ", SC_Z),
    k("qX", SC_X),
    k("qC", SC_C),
    k("qV", SC_V),
    k("qB", SC_B),
    k("qN", SC_N),
    k("qM", SC_M),
    k("qComma", SC_COMMA),
    k("qPeriod", SC_PERIOD),
    k("qDot", SC_PERIOD),
    k("qSlash", SC_SLASH),
};

SC KN_lname_to_sc(char *name) {
    char *colon = strchr(name, ':');
    char *asgn = strchr(name, '=');
    size_t colon_pos = (colon == nil ? strlen(name) : colon - name);
    size_t asgn_pos = (asgn == nil ? strlen(name) : asgn - name);
    size_t maxpos = (colon_pos > asgn_pos ? asgn_pos : colon_pos);
    size_t i;
    fori (i, 0, len(KN_lpairs)) {
        KN_LPair *lp = KN_lpairs + i;
        if (!_strnicmp(lp->name, name, maxpos)) {
            return lp->sc;
        }
    }
    return 0;
}
