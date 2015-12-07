#include "eh.h"
#include "lm.h"
#include "kl.h"
#include "kr.h"

HWINEVENTHOOK EH_handle;

void EH_CALLBACK EH_proc(
    HWINEVENTHOOK hook, DWORD event, HWND hwnd,
    LONG idObject, LONG idChild,
    DWORD dwEventThread, DWORD dwmsEventTime
) {
    DWORD thread_id = GetWindowThreadProcessId(hwnd, nil);
    WORD langid = LANGID_Primary(GetKeyboardLayout(thread_id));
    LANGID lm_langid = LM_selected_langid();
    char *wincls = OS_get_window_class(hwnd);
    dput("\n%s switch |%s|{ lang:%08x:%x;%x } ", (langid==0 ? "console" : "task"), wincls, lm_langid, langid, KL_active);
    LM_hwnd = hwnd;
    if (!strcmp(wincls, "ConsoleWindowClass")) {
        dput("<console/>");
        LM_activate_nth_locale(0);
        LM_activate_selected_locale();
    } else {
        if (KL_active && lm_langid != langid) {
            LM_activate_selected_locale();
        }
        KR_on_task_switch(hwnd, wincls, false);
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

