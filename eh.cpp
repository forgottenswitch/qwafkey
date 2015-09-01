#include "eh.h"
#include "lm.h"
#include "kl.h"

#ifdef __MINGW32__
# define EH_CALLBACK
#else
# define EH_CALLBACK CALLBACK
#endif // __MINGW32__

HWINEVENTHOOK EH_handle;

void EH_CALLBACK EH_proc(
    HWINEVENTHOOK hook, DWORD event, HWND hwnd,
    LONG idObject, LONG idChild,
    DWORD dwEventThread, DWORD dwmsEventTime
) {
    DWORD thread_id = GetWindowThreadProcessId(hwnd, nil);
    WORD langid = LANGID_Primary(GetKeyboardLayout(thread_id));
    LANGID lm_langid = LM_selected_langid();
    dput("%s switch { lang:%08x:%x;:%x }\n", (langid==0 ? "console" : "task"), lm_langid, langid, KL_active);
    LM_hwnd = hwnd;
    if (KL_active && lm_langid != langid) {
        LM_activate_selected_locale();
    }
}

void EH_activate() {
    EH_handle = SetWinEventHook(
            EVENT_SYSTEM_FOREGROUND, EVENT_SYSTEM_FOREGROUND,
            nil, EH_proc,
            0, 0,
            WINEVENT_OUTOFCONTEXT
        );
}

void EH_deactivate() {
    UnhookWinEvent(EH_handle);
}

