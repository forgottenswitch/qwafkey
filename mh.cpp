#include "mh.h"

HHOOK MH_handle;
WORD MH_lang;

#define RawThisEvent() 0
#define StopThisEvent() 1
#define PassThisEvent() CallNextHookEx(NULL, aCode, wParam, lParam)
LRESULT CALLBACK MH_proc(int aCode, WPARAM wParam, LPARAM lParam) {
    CWPSTRUCT *ev = (CWPSTRUCT*) lParam;
    printf(" e;");
    if (ev->message == WM_INPUTLANGCHANGE) {
        MH_lang = LANGID_Primary(ev->lParam);
        printf("change lang: %04d\n", MH_lang);
    }
    return PassThisEvent();
}
#undef RawThisEvent
#undef StopThisEvent
#undef PassThisEvent

void MH_activate() {
    MH_handle = SetWindowsHookEx(WH_CALLWNDPROC, MH_proc, nil, 0);
    printf("MH_handle: %08x\n", (size_t)MH_handle);
    if (MH_handle == nil)
        OS_print_last_error();
}

void MH_deactivate() {
    UnhookWindowsHookEx(MH_handle);
}
