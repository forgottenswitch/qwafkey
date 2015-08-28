#include "eh.h"
#include "mh.h"

#ifndef __MINGW32__
# define EH_CALLBACK CALLBACK
#else
# define EH_CALLBACK
#endif // __MINGW32__

HWINEVENTHOOK EH_handle;

void EH_CALLBACK EH_proc(HWINEVENTHOOK hook, DWORD event, HWND hwnd,
    LONG idObject, LONG idChild,
    DWORD dwEventThread, DWORD dwmsEventTime
    ) {
    DWORD tid = GetWindowThreadProcessId(hwnd, nil);
    WORD langid = LANGID_Primary(GetKeyboardLayout(tid));
    printf("%s switch { lang:%08x; }\n", (langid==0 ? "console" : "task"), langid);
    MH_lang = langid;
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

