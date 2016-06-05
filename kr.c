#include "kr.h"
#include "kl.h"
#include "lm.h"
#include "ka.h"

/* kr.c -- key remapper
 *
 * Allows for keybindings to be assigned per-application.
 *
 * */

bool KR_active;
size_t KR_id = 0;

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

typedef struct {
    int one;
    int two;
} int_pair;

#define KR_hash_table_len 256
int_pair KR_titles_hash_pairs[KR_hash_table_len];
int_pair KR_wndcs_hash_pairs[KR_hash_table_len];

size_t KR_apps_count = 0, KR_apps_size = 0;
KR_App *KR_apps;

size_t KR_titles_count = 0, KR_titles_size = 0;
KR_Title *KR_titles;

size_t KR_wndcs_count = 0, KR_wndcs_size = 0;
KR_Wndcls *KR_wndcs;

KR_App *KR_app = NULL;

#ifndef INT_MAX
# define INT_MAX 2147483647
#endif

int KR_str_checksum(char *s) {
    char c, c0 = 0;
    int sum = 0;
    while ((c = *s++)) {
        sum += c - 2 * c0;
        sum &= INT_MAX;
        c0 = c;
    }
    return sum;
}

int_pair KR_hash_get_idx_pair(int_pair *hash_pairs, char *str) {
    int chsum = KR_str_checksum(str);
    return hash_pairs[chsum % KR_hash_table_len];
}

void KR_add_app(void) {
    if ((KR_apps_count+=1) > KR_apps_size) {
        KR_apps = (KR_App*)realloc(KR_apps, (KR_apps_size *= 1.5) * sizeof(KR_App));
    }
    KR_App *ka = KR_apps + KR_apps_count - 1;
    ZeroPnt(ka);
    KR_app = ka;
}

void KR_add_title(void) {
    if ((KR_titles_count+=1) > KR_titles_size) {
        KR_titles = (KR_Title*)realloc(KR_titles, (KR_titles_size *= 1.5) * sizeof(KR_Title));
    }
    KR_Title *ti = KR_titles + KR_titles_count - 1;
    ZeroPnt(ti);
    ti->app = KR_apps + KR_apps_count - 1;
}

void KR_add_wndcs(void) {
    if ((KR_wndcs_count+=1) > KR_wndcs_size) {
        KR_wndcs = (KR_Wndcls*)realloc(KR_wndcs, (KR_wndcs_size *= 1.5) * sizeof(KR_Wndcls));
    }
    KR_Wndcls *cls = KR_wndcs + KR_wndcs_count - 1;
    ZeroPnt(cls);
    cls->app = KR_apps + KR_apps_count - 1;
}

int KR_compare_sumpairs(const void *x, const void *y) {
    int xc = ((int_pair*)x)->two, yc = ((int_pair*)y)->two;
    if (xc < yc) { return -1; }
    if (xc == yc) { return 0; }
    return 1;
}

void KR_hash_the_titles(void) {
    /* Sort the KR_titles by checksum value */
    int n = KR_titles_count;
    int i;
    int_pair *pairs = malloc((n+1) * sizeof(int_pair));
    fori (i, 0, n) {
        pairs[i].one = i;
        pairs[i].two = KR_str_checksum(KR_titles[i].str) % KR_hash_table_len;
        //printf("Titles [%d]: %.*s\n", i, KR_titles[i].len, KR_titles[i].str);
    }
    qsort(pairs, n, sizeof(int_pair), KR_compare_sumpairs);
    KR_Title *ary = malloc(n * sizeof(KR_Title));
    fori (i, 0, n) {
        int i0 = pairs[i].one;
        ary[i] = KR_titles[i0];
        //printf("Titles [%d]: h%d %.*s\n", i0, pairs[i].two, KR_titles[i0].len, KR_titles[i0].str);
    }
    fori (i, 0, n) { 
        KR_titles[i] = ary[i]; 
        //printf("KR_titles[%d]:= %.*s\n", i, KR_titles[i].len, KR_titles[i].str);
    }
    /* Assign the checksum value ranges to KR_wndcls_hash_pairs */
    int sum0 = 0, i1 = 0, n1 = n + 1;
    fori (i, 0, n1) {
        int sum = pairs[i].two;
        if (!i) { sum0 = sum; }
        else if (sum != sum0) {
            KR_titles_hash_pairs[sum0].one = i1 + 1;
            KR_titles_hash_pairs[sum0].two = i - 1;
            //printf("sum!=; i=%d; [%d..%d] \n", n, i, i1+1, i-1);
            i1 = i;
        } //else { printf("sum[%d]:%d; ", i, sum); }
        sum0 = sum;
    }
    /* Clean up */
    free(ary);
    free(pairs);
    /* Print the result */
    fori (i, 0, KR_hash_table_len) {
        int_pair i12 = KR_titles_hash_pairs[i];
        if (i12.one) {
            printf("Titles h%d:\n", i);
            if (i12.one) for (i1=i12.one-1; i1<=i12.two; i1++) {
                printf(" [%d]: %.*s\n", i1, KR_titles[i1].len, KR_titles[i1].str);
            }
        }
    }
}

void KR_hash_the_wndcls(void) {
    /* Sort the KR_wndcs by checksum value */
    int n = KR_wndcs_count;
    int i;
    int_pair *pairs = malloc((n+1) * sizeof(int_pair));
    fori (i, 0, n) {
        pairs[i].one = i;
        pairs[i].two = KR_str_checksum(KR_wndcs[i].str) % KR_hash_table_len;
        //printf("Wndcs [%d]: %.*s\n", i, KR_wndcs[i].len, KR_wndcs[i].str);
    }
    qsort(pairs, n, sizeof(int_pair), KR_compare_sumpairs);
    KR_Wndcls *ary = malloc(n * sizeof(KR_Wndcls));
    fori (i, 0, n) {
        int i0 = pairs[i].one;
        ary[i] = KR_wndcs[i0];
        //printf("Wndcs [%d]: h%d %.*s\n", i0, pairs[i].two, KR_wndcs[i0].len, KR_wndcs[i0].str);
    }
    fori (i, 0, n) { 
        KR_wndcs[i] = ary[i]; 
        //printf("KR_wndcs[%d]:= %.*s\n", i, KR_wndcs[i].len, KR_wndcs[i].str);
    }
    /* Assign the checksum value ranges to KR_wndcls_hash_pairs */
    int sum0 = 0, i1 = 0, n1 = n + 1;
    fori (i, 0, n1) {
        int sum = pairs[i].two;
        if (!i) { sum0 = sum; }
        else if (sum != sum0) {
            KR_wndcs_hash_pairs[sum0].one = i1 + 1;
            KR_wndcs_hash_pairs[sum0].two = i - 1;
            //printf("sum!=; i=%d; [%d..%d] \n", n, i, i1+1, i-1);
            i1 = i;
        } //else { printf("sum[%d]:%d; ", i, sum); }
        sum0 = sum;
    }
    /* Clean up */
    free(ary);
    free(pairs);
    /* Print the result */
    fori (i, 0, KR_hash_table_len) {
        int_pair i12 = KR_wndcs_hash_pairs[i];
        if (i12.one) {
            printf("Wndcls h%d:\n", i);
            if (i12.one) for (i1=i12.one-1; i1<=i12.two; i1++) {
                printf(" [%d]: %.*s\n", i1, KR_wndcs[i1].len, KR_wndcs[i1].str);
            }
        }
    }
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
    printf("kr_apply");
    if (!on_pt_only) {
        KR_id = 1;
    }
    KR_apply_app(app, on_pt_only);
}

KR_App *KR_hwnd_to_app(HWND hwnd) {
    printf(" hwnd_to_app... ");
    char buf[256];
    int buflen = GetWindowTextA(hwnd, buf, lenof(buf));
    if (buflen > 0) {
        printf("title(%d) |%s| ", buflen, buf);
        buflen--;
        int i;
        int_pair i12 = KR_hash_get_idx_pair(KR_titles_hash_pairs, buf);
        if (i12.one) {
            for (i=i12.one-1; i<=i12.two; i++) {
                KR_Title *ti = KR_titles + i;
                printf("t(%d)|%s| ", ti->len, ti->str);
                if (ti->len && !strnicmp(buf, ti->str, ti->len)) {
                    printf("ok t app%d ", i);
                    return ti->app;
                }
            }
        }
    }
    return 0;
}

KR_App *KR_wndcls_to_app(char *wndcls) {
    int i;
    int_pair i12 = KR_hash_get_idx_pair(KR_wndcs_hash_pairs, wndcls);
    printf(" wndcls_to_app { %d, %d }... ", i12.one, i12.two);
    if (i12.one) {
        for (i=i12.one-1; i<=i12.two; i++) {
            KR_Wndcls *cls = KR_wndcs + i;
            printf("c(%d)|%s| ", cls->len, cls->str);
            if (cls->len && !strnicmp(wndcls, cls->str, cls->len)) {
                printf("ok c app%d ", i);
                return cls->app;
            }
        }
    }
    return 0;
}

void KR_clear() {
    printf("kr_clear ");
    KR_id = 0;
    LM_activate_selected_locale();
}

void KR_resume(bool on_pt_only) {
    printf("kr_resume(%d) ", on_pt_only);
    HWND hwnd = GetForegroundWindow();
    KR_on_task_switch(hwnd, OS_get_window_class(hwnd), on_pt_only);
}

void KR_on_task_switch(HWND hwnd, char *wndclass, bool on_pt_only) {
    if (!KL_active || !KR_active) {
        if (KR_id) {
            goto clear;
        }
        goto wndcls;
    }
    KR_App *app;
    if ((app = KR_hwnd_to_app(hwnd))) {
        puts("{FOUND APP}");
        KR_apply(app, false);
        return;
    } else if (KR_id) {
        puts("{HAVE ID}");
        goto clear;
    } else {
        goto wndcls;
    }
    clear:
    KR_clear();
    wndcls:
    puts("{AT WNDCLS}");
    if ((app = KR_wndcls_to_app(wndclass))) {
        KR_apply(app, on_pt_only);
    }
    return;
}

void KR_activate() {
    printf("kr_on ");
    KR_active = true;
    KR_resume(false);
}

void KR_deactivate() {
    printf("kr_off ");
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
    if (KR_app == NULL) {
        printf("remap sc%03x=>%03x : no title; ", sc, binding);
        return;
    }
    printf("remap sc%03x=>%03x/x%02x ", sc, binding, mods);
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
