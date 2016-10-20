#include "ui.h"
#include "kl.h"
#include "lm.h"
#include "hk.h"
#include "parse.h"

/* ui.c -- the graphical user interface,
 * that is, the tray icon.
 *
 *  */

/* main window */
HWND UI_MW;

char UI_MW_Title[] = "qwafkey";
char UI_MW_Class[] = "qwafkey";

int UI_TR_ID = 1;

HICON UI_ICO_On;
HICON UI_ICO_Off;

/*
   Tray icon ("notify icon") procedure.
   Creates, modifies, and removes the tray notification icon
   based on action parameter (NIM_ADD, NIM_MODIFY, NIM_DESTROY).
*/
bool UI_TR_proc(DWORD action, char *tooltip_str) {
    NOTIFYICONDATA nid;
    nid.cbSize = sizeof nid;
    nid.hWnd = UI_MW;
    nid.uID = UI_TR_ID;
    nid.uFlags = NIF_ICON | NIF_MESSAGE;
    nid.uCallbackMessage = UI_TRAY_MSG;
    nid.hIcon = (KL_active ? UI_ICO_On : UI_ICO_Off);
    if (tooltip_str) {
        nid.uFlags |= NIF_TIP;
        lstrcpyn(nid.szTip, tooltip_str, lenof(nid.szTip));
    }
    return (bool) Shell_NotifyIcon(action, &nid);
}

bool UI_TR_update(void) {
    return UI_TR_proc(NIM_MODIFY, NULL);
}

void UI_TR_delete(void) {
    UI_TR_proc(NIM_DELETE, NULL);
}

UI_STR UI_STR_Suspend = "Suspend";
UI_STR UI_STR_Restart = "Restart";
UI_STR UI_STR_NextLayout = "Next layout";
UI_STR UI_STR_PrevLayout = "Previous layout";
UI_STR UI_STR_Exit = "Exit";

enum {
    UI_TC_SUSPEND = 17,
    UI_TC_RESTART,
    UI_TC_NEXT,
    UI_TC_PREV,
    UI_TC_EXIT,
};

void UI_TR_menu(void) {
    SetForegroundWindow(UI_MW);
    POINT mpt;
    HMENU m = CreatePopupMenu();
    AppendMenu(m, (KL_active ? 0 : MF_CHECKED), UI_TC_SUSPEND, UI_STR_Suspend);
    AppendMenu(m, 0, UI_TC_RESTART, UI_STR_Restart);
    AppendMenu(m, MF_SEPARATOR, 0, NULL);
    AppendMenu(m, 0, UI_TC_NEXT, UI_STR_NextLayout);
    AppendMenu(m, 0, UI_TC_PREV, UI_STR_PrevLayout);
    AppendMenu(m, MF_SEPARATOR, 0, NULL);
    AppendMenu(m, 0, UI_TC_EXIT, UI_STR_Exit);
    GetCursorPos(&mpt);
    int cmd = TrackPopupMenuEx(m, TPM_RETURNCMD, mpt.x, mpt.y, UI_MW, 0);
    switch (cmd) {
    case UI_TC_SUSPEND:
        if (KL_active) {
            KL_deactivate();
        } else {
            KL_activate();
        }
        break;
    case UI_TC_RESTART:
        restart_the_program();
        break;
    case UI_TC_NEXT:
        LM_activate_next_locale();
        break;
    case UI_TC_PREV:
        LM_activate_prev_locale();
        break;
    case UI_TC_EXIT:
        UI_quit(0);
        break;
    }
}

LRESULT CALLBACK UI_MW_proc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case UI_TRAY_MSG:
            switch (lParam) {
                case WM_RBUTTONDOWN:
                    UI_TR_menu();
                    break;
            }
            break;
        case WM_CLOSE:
            UI_quit(0);
            return 0;
    }
    return DefWindowProc(hWnd, uMsg ,wParam, lParam);
}

void UI_quit(int status) {
    PostQuitMessage(status);
}

bool UI_init(void) {
    WNDCLASSEX w;
    w.cbClsExtra = 0;
    w.cbSize = sizeof(WNDCLASSEX);
    w.cbWndExtra = 0;
    w.hbrBackground = 0;
    w.hCursor = 0;
    w.hIcon = 0;
    w.hIconSm = w.hIcon;
    w.hInstance = 0;
    w.lpfnWndProc = UI_MW_proc;
    w.lpszClassName = UI_MW_Class;
    w.lpszMenuName = 0;
    w.style = CS_HREDRAW | CS_VREDRAW;
    if(!RegisterClassEx(&w))
        return false;

    UI_MW = CreateWindowEx(0, UI_MW_Class, UI_MW_Title,
                           WS_OVERLAPPEDWINDOW,
                           CW_USEDEFAULT, CW_USEDEFAULT,
                           CW_USEDEFAULT, CW_USEDEFAULT,
                           0, 0,
                           0, 0);
    if (!UI_MW) {
        return false;
    }
    ShowWindow(UI_MW, SW_HIDE);

    #define s(id, var) LoadString(0, id, var, UI_STR_MAXLEN)
    s(IDS_SUSPEND, UI_STR_Suspend);
    s(IDS_RESTART, UI_STR_Restart);
    s(IDS_NEXT_LAYOUT, UI_STR_NextLayout);
    s(IDS_PREV_LAYOUT, UI_STR_PrevLayout);
    s(IDS_EXIT, UI_STR_Exit);
    #undef s

    #define icon(var, resource) var = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(resource))
    icon(UI_ICO_On, IDI_ICON_ON);
    icon(UI_ICO_Off, IDI_ICON_OFF);
    #undef icon

    UI_TR_proc(NIM_ADD, UI_MW_Title);
    return true;
}

HANDLE UI_thread;
DWORD UI_thread_id;

void UI_loop(void) {
    for (;;) {
        int r;
        MSG msg;

        r = GetMessage(&msg, UI_MW, 0, 0);
        if (r <= 0)
            break;

        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

DWORD WINAPI UI_thread_proc(LPVOID arg_unused) {
    UI_loop();
    return 0;
}

void UI_spawn(void) {
    UI_thread = CreateThread(NULL, 8*1024, UI_thread_proc, NULL, 0, &UI_thread_id);
}

bool UI_ask_for_creating_config_file(char *path) {
    int x;
    char title[] = ProgramName" configuration file";
    char *message;
    message = str_concat(ProgramName" configuration file ", path, " is missing.\n"
                         "Would you like to create it? (It would open in Notepad)", NULL);
    x = MessageBox(NULL, message, title, MB_YESNO);
    if (x == IDYES) {
        char *config_example_path;
        char *config_example = NULL;
        config_example_path = str_concat_path(ProgramDir, "config", "example.txt", NULL);
        CreateDirectory(ConfigDir, NULL);
        {
            FILE *f = fopen(config_example_path, "r");
            if (f) {
                config_example = fread_to_eof(f, '\n');
                fclose(f);
            }
            free(config_example_path);
        }
        if (config_example) {
            FILE *f = fopen(path, "w");
            fwrite(config_example, 1, strlen(config_example), f);
            fclose(f);
            free(config_example);
        }
        char *cmd;
        cmd = str_concat("notepad \"", path, "\"", NULL);
        OS_run_command(cmd);
        free(cmd);
    }
    free(message);
    return (x == IDYES);
}

void UI_maybe_show_errors_for_config_file(char *path) {
    if (!parse_failed_lines_count) { return; }
    char *message = str_concat("Unrecognized lines in config file ",
                               path, ":\n\n", parse_failed_lines, NULL);
    int x = MessageBox(NULL, message, ProgramName,
                       MB_ICONERROR | MB_ABORTRETRYIGNORE | MB_DEFBUTTON2);
    if (x == IDABORT) { exit(1); }
    else if (x == IDRETRY) { restart_the_program(); }
    free(message);
}
