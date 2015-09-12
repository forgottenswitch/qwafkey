#include "kr.h"
#include "kl.h"
#include "lm.h"

bool KR_active;
size_t KR_id = 0;

typedef struct {
    USHORT x;
    USHORT y;
} KR_Res;

KR_Res KR_hwnd_to_res(HWND hwnd) {
    RECT r;
    KR_Res res;
    GetClientRect(hwnd, &r);
    res.x = r.right - r.left;
    res.y = r.bottom - r.top;
    return res;
}

size_t KR_res_count = 0;
KR_Res KR_res[32];

void KR_add_res(USHORT x, USHORT y) {
    if (KR_res_count >= len(KR_res)) {
        return;
    }
    KR_res[KR_res_count].x = x;
    KR_res[KR_res_count].y = y;
    KR_res_count += 1;
}

bool KR_match_res(HWND hwnd) {
    KR_Res dim = KR_hwnd_to_res(hwnd), res;
    dput("w{%d,%d} ", dim.x, dim.y);
    size_t i;
    fori (i, 0, KR_res_count) {
        res = KR_res[i];
        dput("r{%d,%d} ", res.x, res.y);
        if (res.x == dim.x && res.y == dim.y) {
            dput("ok ");
            return true;
        }
    }
    return false;
}

typedef struct {
    USHORT sc;
    USHORT binding;
} KR_Bind;

typedef struct {
    char window_title[KR_MAXTITLE];
    size_t window_title_len;
    char window_class[KR_MAXCLASS];
    size_t window_class_len;
    size_t binds_count;
    size_t binds_size;
    KR_Bind *binds;
} KR_App;

size_t KR_apps_count = 0, KR_apps_size = 0;
KR_App *KR_apps = (KR_App*)malloc((KR_apps_size = 20) * sizeof(KR_App));

void KR_add_app() {
    if ((KR_apps_count+=1) > KR_apps_size) {
        KR_apps = (KR_App*)realloc(KR_apps, (KR_apps_size *= 1.5) * sizeof(KR_App));
    }
    KR_App *ka = KR_apps + KR_apps_count;
    ZeroPnt(ka);
}

void KR_apply_app(KR_App *app) {
    if (app->window_class_len) {
        dput("cls|%s| ", app->window_class);
    } else if (app->window_title_len) {
        dput("tit|%s| ", app->window_title);
    }
    size_t i, bc = app->binds_count;
    KR_Bind *binds = app->binds, b;
    fori (i, 0, bc) {
        b = binds[i];
        if (b.sc) {
            KL_temp_sc(b.sc, b.binding);
        }
    }
}

void KR_apply(size_t id) {
    dput("kr_apply %d ", id);
    KR_id = id;
    KR_apply_app(KR_apps + id - 1);
}

size_t KR_hwnd_to_id(HWND hwnd) {
    char buf[256];
    int buflen = GetWindowTextA(hwnd, buf, len(buf));
    if (buflen > 0) {
        dput("title(%d) |%s| ", buflen, buf);
        buflen--;
        size_t i;
        fori (i, 0, KR_apps_count) {
            KR_App *app = KR_apps + i;
            char *title = app->window_title;
            size_t title_len = app->window_title_len;
            dput("t(%d)|%s| ", title_len, title);
            if (!strnicmp(buf, title, title_len)) {
                dput("ok t app%d ", i);
                return i+1;
            }
        }
    }
    return 0;
}

size_t KR_wndcls_to_id(char *wndcls) {
    size_t i;
    fori (i, 0, KR_apps_count) {
        KR_App *app = KR_apps + i;
        char *cls = app->window_class;
        size_t cls_len = app->window_class_len;
        dput("c(%d)|%s| ", cls_len, cls);
        if (cls_len && !strnicmp(wndcls, cls, cls_len)) {
            dput("ok c app%d ", i);
            return i+1;
        }
    }
    return 0;
}

void KR_clear() {
    dput("kr_clear ");
    KR_id = 0;
    LM_activate_selected_locale();
}

void KR_resume() {
    dput("kr_resume ");
    HWND hwnd = GetActiveWindow();
    size_t id = KR_hwnd_to_id(hwnd);
    if (!id) {
        return;
    }
    KR_apply(id);
}

void KR_toggle_clear() {
    if (KR_id) {
        KR_clear();
    } else {
        KR_resume();
    }
}

void KR_on_task_switch(HWND hwnd, char *wndclass) {
    Sleep(500);
    if (!KR_active || !KR_match_res(hwnd)) {
        goto clear;
    }
    size_t id;
    if ((id = KR_hwnd_to_id(hwnd))) {
        KR_apply(id);
        return;
    } else if (KR_id) {
        goto clear;
    } else {
        return;
    }
    clear:
    if ((id = KR_wndcls_to_id(wndclass))) {
        KR_apply(id);
    } else {
        KR_clear();
    }
    return;
}

void KR_activate() {
    KR_active = true;
    KR_resume();
}

void KR_deactivate() {
    KR_active = false;
    KR_clear();
}

void KR_toggle() {
    if (KR_active) {
        KR_activate();
    } else {
        KR_deactivate();
    }
}

KR_App *KR_app = nil;

void KR_set_bind_title(char *title) {
    KR_add_app();
    KR_app = KR_apps + KR_apps_count - 1;
    size_t title_len = strlen(title);
    if (title_len >= KR_MAXTITLE) {
        title_len = KR_MAXTITLE - 1;
    }
    strncpy(KR_app->window_title, title, title_len+1);
    KR_app->window_title[title_len] = '\0';
    KR_app->window_title_len = title_len;
}

void KR_set_bind_class(char *wndclass) {
    KR_add_app();
    KR_app = KR_apps + KR_apps_count - 1;
    size_t cls_len = strlen(wndclass);
    if (cls_len >= KR_MAXCLASS) {
        cls_len = KR_MAXCLASS - 1;
    }
    strncpy(KR_app->window_class, wndclass, cls_len+1);
    KR_app->window_class[cls_len] = '\0';
    KR_app->window_class_len = cls_len;
}

void KR_bind(SC sc, SC binding) {
    if (KR_app == nil) {
        dput("remap sc%03x=>%03x : no title; ", sc, binding);
        return;
    }
    dput("remap sc%03x=>%03x ", sc, binding);
    if (!KR_app->binds) {
        KR_app->binds = (KR_Bind*)malloc((KR_app->binds_size = 8) * sizeof(KR_Bind));
    }
    if (KR_app->binds_count >= KR_app->binds_size) {
        KR_app->binds = (KR_Bind*)realloc(KR_app->binds, (KR_app->binds_size *= 1.5) * sizeof(KR_Bind));
    }
    KR_Bind b = { sc, binding };
    KR_app->binds[KR_app->binds_count] = b;
    KR_app->binds_count += 1;
}
