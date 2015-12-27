#include "kr.h"
#include "kl.h"
#include "lm.h"
#include "ka.h"

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
    USHORT mods;
    USHORT binding;
} KR_Bind;

typedef struct {
    size_t binds_count;
    size_t binds_size;
    KR_Bind *binds;
} KR_App;

typedef struct {
    char str[KR_MAXTITLE];
    size_t len;
    KR_App *app;
} KR_Title;

typedef struct {
    char str[KR_MAXCLASS];
    size_t len;
    KR_App *app;
} KR_Wndcls;

size_t KR_apps_count = 0, KR_apps_size = 0;
KR_App *KR_apps;

size_t KR_titles_count = 0, KR_titles_size = 0;
KR_Title *KR_titles;

size_t KR_wndcs_count = 0, KR_wndcs_size = 0;
KR_Wndcls *KR_wndcs;

KR_App *KR_app = nil;

void KR_add_app() {
    if ((KR_apps_count+=1) > KR_apps_size) {
        KR_apps = (KR_App*)realloc(KR_apps, (KR_apps_size *= 1.5) * sizeof(KR_App));
    }
    KR_App *ka = KR_apps + KR_apps_count - 1;
    ZeroPnt(ka);
    KR_app = ka;
}

void KR_add_title() {
    if ((KR_titles_count+=1) > KR_titles_size) {
        KR_titles = (KR_Title*)realloc(KR_titles, (KR_titles_size *= 1.5) * sizeof(KR_Title));
    }
    KR_Title *ti = KR_titles + KR_titles_count - 1;
    ZeroPnt(ti);
    ti->app = KR_apps + KR_apps_count - 1;
}

void KR_add_wndcs() {
    if ((KR_wndcs_count+=1) > KR_wndcs_size) {
        KR_wndcs = (KR_Wndcls*)realloc(KR_wndcs, (KR_wndcs_size *= 1.5) * sizeof(KR_Wndcls));
    }
    KR_Wndcls *cls = KR_wndcs + KR_wndcs_count - 1;
    ZeroPnt(cls);
    cls->app = KR_apps + KR_apps_count - 1;
}

size_t KR_ka_kr_on_pt;

void KR_apply_app(KR_App *app, bool on_pt_only) {
    size_t i, bc = app->binds_count;
    KR_Bind *binds = app->binds, b;
    fori (i, 0, bc) {
        b = binds[i];
        if (b.sc) {
            if (on_pt_only && b.binding != KR_ka_kr_on_pt) {
                continue;
            }
            KL_temp_sc(b.sc, b.mods, b.binding);
        }
    }
}

void KR_apply(KR_App *app, bool on_pt_only) {
    dput("kr_apply");
    if (!on_pt_only) {
        KR_id = 1;
    }
    KR_apply_app(app, on_pt_only);
}

KR_App *KR_hwnd_to_app(HWND hwnd) {
    char buf[256];
    int buflen = GetWindowTextA(hwnd, buf, len(buf));
    if (buflen > 0) {
        dput("title(%d) |%s| ", buflen, buf);
        buflen--;
        size_t i;
        fori (i, 0, KR_titles_count) {
            KR_Title *ti = KR_titles + i;
            dput("t(%d)|%s| ", ti->len, ti->str);
            if (ti->len && !strnicmp(buf, ti->str, ti->len)) {
                dput("ok t app%d ", i);
                return ti->app;
            }
        }
    }
    return 0;
}

KR_App *KR_wndcls_to_app(char *wndcls) {
    size_t i;
    fori (i, 0, KR_wndcs_count) {
        KR_Wndcls *cls = KR_wndcs + i;
        dput("c(%d)|%s| ", cls->len, cls->str);
        if (cls->len && !strnicmp(wndcls, cls->str, cls->len)) {
            dput("ok c app%d ", i);
            return cls->app;
        }
    }
    return 0;
}

void KR_clear() {
    dput("kr_clear ");
    KR_id = 0;
    LM_activate_selected_locale();
}

void KR_resume(bool on_pt_only) {
    dput("kr_resume(%d) ", on_pt_only);
    HWND hwnd = GetForegroundWindow();
    KR_on_task_switch(hwnd, OS_get_window_class(hwnd), on_pt_only);
}

void KR_on_task_switch(HWND hwnd, char *wndclass, bool on_pt_only) {
    Sleep(500);
    if (!KL_active || !KR_active || !KR_match_res(hwnd)) {
        if (KR_id) {
            goto clear;
        }
        goto wndcls;
    }
    KR_App *app;
    if ((app = KR_hwnd_to_app(hwnd))) {
        KR_apply(app, false);
        return;
    } else if (KR_id) {
        goto clear;
    } else {
        return;
    }
    clear:
    KR_clear();
    wndcls:
    if ((app = KR_wndcls_to_app(wndclass))) {
        KR_apply(app, on_pt_only);
    }
    return;
}

void KR_activate() {
    dput("kr_on ");
    KR_active = true;
    KR_resume(false);
}

void KR_deactivate() {
    dput("kr_off ");
    KR_active = false;
    KR_clear();
}

void KR_toggle() {
    if (KR_active) {
        KR_deactivate();
    } else {
        KR_activate();
    }
}

void KR_set_bind_title(char *title) {
    KR_add_title();
    KR_Title *ti = KR_titles + KR_titles_count - 1;
    size_t title_len = strlen(title);
    if (title_len >= KR_MAXTITLE) {
        title_len = KR_MAXTITLE - 1;
    }
    strncpy(ti->str, title, title_len+1);
    ti->str[title_len] = '\0';
    ti->len = title_len;
}

void KR_set_bind_class(char *wndclass) {
    KR_add_wndcs();
    KR_Wndcls *cls = KR_wndcs + KR_wndcs_count - 1;
    size_t cls_len = strlen(wndclass);
    if (cls_len >= KR_MAXCLASS) {
        cls_len = KR_MAXCLASS - 1;
    }
    strncpy(cls->str, wndclass, cls_len+1);
    cls->str[cls_len] = '\0';
    cls->len = cls_len;
}

void KR_bind(SC sc, SC binding, USHORT mods) {
    if (KR_app == nil) {
        dput("remap sc%03x=>%03x : no title; ", sc, binding);
        return;
    }
    dput("remap sc%03x=>%03x/x%02x ", sc, binding, mods);
    if (!KR_app->binds) {
        KR_app->binds = (KR_Bind*)malloc((KR_app->binds_size = 8) * sizeof(KR_Bind));
    }
    if (KR_app->binds_count >= KR_app->binds_size) {
        KR_app->binds = (KR_Bind*)realloc(KR_app->binds, (KR_app->binds_size *= 1.5) * sizeof(KR_Bind));
    }
    KR_Bind b = { sc, mods, binding };
    KR_app->binds[KR_app->binds_count] = b;
    KR_app->binds_count += 1;
}

void KR_init() {
    KR_apps = (KR_App*)malloc((KR_apps_size = 20) * sizeof(KR_App));
    KR_titles = (KR_Title*)malloc((KR_titles_size = 20) * sizeof(KR_Title));
    KR_wndcs = (KR_Wndcls*)malloc((KR_wndcs_size = 20) * sizeof(KR_Wndcls));
    KR_ka_kr_on_pt = KA_name_to_id("kr_on_pt");
}
