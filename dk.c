#include "dk.h"
#include "freadline.h"
#include "ka.h"
#include "stdafx.h"

/*
A starting size of children array for a pivot node.
It is 2 as many nodes have just one descedant.
*/
#define DK_Node_MINSIZE 2
/* A printf shortcut for DK_Node. */
#define dput_node(node) dput(" node {a%d}\n", node->count)

/* A character buffer type definitions. */
#define DK_charbuf_SIZE 256
typedef char DK_charbuf[DK_charbuf_SIZE];

/* Check if two DK_Key structs have the same .type and .code. */
bool DK_Key_eq(DK_Key k1, DK_Key k2) {
    bool ret = (k1.type == k2.type) && (k1.code == k2.code);;
    //dput("DK_eq (%d,%d), (%d,%d) => %d\n", k1.type, k1.code, k2.type, k2.code, ret);
    return ret;
}

#define DK_pivotNode_alloc(node) ((node)->count)
#define DK_pivotNode_length(node) ((node)->data.pivot.nodes[0].count)
#define DK_pivotNode_nodes(node) ((node)->data.pivot.nodes)
#define DK_bindNode_code(node) ((node)->data.bind.code)

/* Check if DK_Node is a pivot, not bind. */
bool DK_is_pivotNode(DK_Node *node) {
    return DK_pivotNode_alloc(node) > 0;
}

/* Convert a node to pivot, allocating children array. */
void DK_bindNode_toPivot(DK_Node *node) {
    DK_Node *nodes = (DK_Node*) calloc(DK_Node_MINSIZE, sizeof(DK_Node));
    DK_pivotNode_nodes(node) = nodes;
    DK_pivotNode_alloc(node) = DK_Node_MINSIZE;
}

/* Free the (pivot) node's children array recursively. */
void DK_Node_free(DK_Node *node) {
    if (DK_is_pivotNode(node)) {
        int i, len = DK_pivotNode_length(node);
        DK_Node *nodes = DK_pivotNode_nodes(node);
        fori (i, 1, len) {
            DK_Node_free(nodes + i);
        }
        free(nodes);
        DK_pivotNode_alloc(node) = 0;
        DK_pivotNode_nodes(node) = nil;
    }
}

/* Convert a pivot node to bind one, freeing children array. */
void DK_pivotNode_toBind(DK_Node *node) {
    DK_Node_free(node);
    DK_pivotNode_nodes(node) = nil;
    DK_pivotNode_alloc(node) = 0;
    DK_bindNode_code(node) = 0;
}

/*
Find a DK_Node descedant node with a matching DK_Key.
If a node is found, first convert it to bind or pivot (if mismatch) according to pivot arg.
Otherwise, append a child node initialized to bind or pivot according to pivot arg.
*/
DK_Node *DK_pivotNode_fetch(DK_Node * const node, DK_Key key, bool pivot) {
    int len = DK_pivotNode_length(node);
    int alloc = DK_pivotNode_alloc(node);
    DK_Node *nodes = DK_pivotNode_nodes(node);
    DK_Node *node1;
    int i;

    dput("DK_pivot_fetch node(addr:%p alloc:%d length:%d) key(%d,%d) pivot:%d\n", (void*)node, alloc, len, key.type, key.code, pivot);
    /* try to find an existing child */
    fori (i, 1, len) {
        node1 = nodes + i;
        /* dput("DK_pivot_fetch   child[%d] alloc:%d key:(%d,%d)\n", i, node1->count, node1->key.type, node1->key.code); */
        if (DK_Key_eq(node1->key, key)) {
            bool is_pivot1 = DK_is_pivotNode(node1);
            dput("DK_pivot_fetch   child[%d] is eq, is_pivot:%d\n", i, is_pivot1);
            if (is_pivot1 != pivot) {
                if (pivot) {
                    DK_bindNode_toPivot(node1);
                } else {
                    DK_pivotNode_toBind(node1);
                }
            }
            /* dput("found"); dput_node(node1); */
            return node1;
        }
    }

    /* get a free slot */
    len = !len ? 2 : len+1;
    if (alloc < len) {
        size_t bytes = sizeof(DK_Node) * alloc;
        size_t alloc1 = alloc * 3 / 2;
        size_t bytes1 = sizeof(DK_Node) * alloc1;
        dput("DK_pivot_fetch %ld -> %ld, %ld -> %ld\n", (long)alloc, (long)alloc1, (long)bytes, (long)bytes1);
        nodes = (DK_Node*) realloc(nodes, bytes1);
        DK_pivotNode_nodes(node) = nodes;
        DK_pivotNode_alloc(node) = alloc1;
    }
    node1 = nodes + len - 1;
    nodes[0].count = len;

    /* initialize the child */
    if (pivot) {
        dput("DK_pivot_fetch to_pivot\n");
        DK_bindNode_toPivot(node1);
    } else {
        dput("DK_pivot_fetch to_bind\n");
        DK_pivotNode_alloc(node1) = 0;
        DK_bindNode_code(node1) = 0;
    }
    dput("DK_pivot_fetch key=(%d,%d)\n", key.type, key.code);
    node1->key = key;
    /* dput("created"); dput_node(node1); */

    dput("DK_pivot_fetch done(len:%d).\n", DK_pivotNode_length(node));
    return node1;
}

/* Find a DK_Node descedant node with matching DK_Key. */
DK_Node *DK_pivotNode_get(DK_Node *node, DK_Key key) {
    dput("DK_pivot_get %p,%p (%d,%d), %d\n", (void*)node, (void*)&DK_node, key.type, key.code, node->count); fflush(stdout);
    DK_Node *nodes = DK_pivotNode_nodes(node);
    dput("DK_pivot_get nodes:%p.\n", (void*)nodes); fflush(stdout);
    int len = DK_pivotNode_length(node);
    dput("DK_pivot_get len:%d.\n", len); fflush(stdout);
    int i;
    fori (i, 1, len) {
        dput("DK_pivot_get %d-th.\n", i); fflush(stdout);
        if (DK_Key_eq(nodes[i].key, key)) {
            return nodes + i;
        }
    }
    dput("DK_pivot_get nil.\n"); fflush(stdout);
    return nil;
}

typedef SC DK_StrOffset;
typedef struct {
    SC code;
    DK_StrOffset name_ofs;
} DK_CharName;

/* A map from key name to UCS-2 character */
size_t DK_cns_count = 0;
size_t DK_cns_alloc = 0;
DK_CharName *DK_cns;

/* An append-only string heap */
char *DK_s;
size_t DK_s_len = 0;
size_t DK_s_alloc = 0;
#define DK_ofs_to_s(ofs) (DK_s + (ofs))

/* Output all the defined key name => UCS-2 character associations. */
void DK_print_key_names() {
    size_t i;
    dputs("");
    fori (i, 0, DK_cns_count) {
        DK_CharName cn = DK_cns[i];
        dput("%s:U+%04x ", DK_ofs_to_s(cn.name_ofs), cn.code);
    }
    dputs("");
}

/* Copy given string to string heap, and return offset of the copy. */
DK_StrOffset DK_s_get(char *s) {
    size_t len = DK_s_len;
    size_t alloc = DK_s_alloc;
    size_t l = strlen(s);
    /* printf("DK_get: [%ld, %ld, %ld] %s\n", len, l, len+l, s); */
    if (l >= DK_charbuf_SIZE)
        l = DK_charbuf_SIZE-1;
    size_t len1 = len + l + 1;
    if (len1 > alloc) {
        size_t alloc1 = alloc * 3 / 2;
        dput("DK_grow %ld,%ld, %ld -> %ld\n", (long)len, (long)len1, (long)alloc, (long)alloc1);
        DK_s = (char*) realloc(DK_s, (DK_s_alloc = alloc1));
        dput("DK_grow done.\n");
        /* DK_print_key_names(); */
    }
    char *s1 = DK_s + len;
    memcpy(s1, s, l);
    DK_s[len+l] = 0;
    DK_s_len = len1;
    /* printf("DK_get:  => %ld -> %ld %s\n", len, len1, s1); */
    return len;
}

/* Add a key name => UCS-2 character association. */
void DK_cns_push(char *name, int code) {
    /* dput("%s=U+%04x ", name, code); */
    size_t count = DK_cns_count;
    size_t count1 = count + 1;
    size_t alloc = DK_cns_alloc;
    size_t alloc1 = alloc * 3 / 2;
    if (count >= DK_cns_alloc) {
        dput("DK_cns grow %ld -> %ld, %ld -> %ld\n", (long)count, (long)count1, (long)alloc, (long)alloc1);
        DK_cns = (DK_CharName*) realloc(DK_cns, sizeof(DK_CharName) * (DK_cns_alloc = alloc1));
        dput("DK_cns grow done.\n");
    }
    DK_StrOffset name_ofs = DK_s_get(name);
    DK_CharName cn;
    cn.name_ofs = name_ofs;
    cn.code = code;
    DK_cns[count] = cn;
    DK_cns_count = count1;
}

/* Check if character is a hexadecimal digit. */
bool ishex(char c);/* {
    return between(c, '0', '9') || between(c, 'a', 'f') || between(c, 'A', 'F');
}*/

/* Convert hexadecimal digit to integer value. */
int hextoi(char c) {
    if (between(c, '0', '9')) {
        return c - '0';
    } else if (between(c, 'a', 'f')) {
        return c - 'a' + 10;
    } else /*if (between(c, 'A', 'F'))*/ {
        return c - 'A' + 10;
    }
}

/* Read "U+xxxx" or "Uxxxx" hexadecimal numbers */
int ucs2_from_str(char *s) {
    if (s[0] != 'U')
        return 0;
    s += (s[1] == '+') ? 2 : 1;
    int i;
    int n = 0;
    fori (i, 0, 4) {
        char c = *s;
        if (c == 0) {
            break;
        } else if (!ishex(c)) {
            return 0;
        }
        n = (n * 0x10) + hextoi(c);
        s+=1;
    }
    char c = *s;
    if (!c || !(between(c, 'a', 'z') || between(c, 'A', 'Z') || between(c, '0', '9')))
        return n;
    return 0;
}

/* Convert key name to character */
DK_Bind DK_Bind_from_charbuf(char *str) {
    DK_Bind ret;
    size_t i;
    if ((ret.code = ucs2_from_str(str))) {
        return ret;
    }
    fori (i, 0, DK_cns_count) {
        DK_CharName kn = DK_cns[i];
        if (!strncmp(str, DK_ofs_to_s(kn.name_ofs), DK_charbuf_SIZE)) {
            ret.code = kn.code;
            //printf(" == U+%04x\n", kn.code);
            return ret;
        }
    }
    ret.code = 0;
    return ret;
}

typedef struct {
    SC sym;
    DK_StrOffset name_ofs;
} DK_SymName;

/*
 A map from dead key names to their syms and key action ids.
 Key names are stored as offsets for DK_s_get().

 Sym is used to support dead key aliases.
 Key action id is an element's index.
*/
DK_SymName DK_dks[80];
size_t DK_dks_count = 0;

void DK_dksym_add(SC sym, char *name) {
    dput("dkn_id_add %04x |%s|\n", sym, name);
    DK_StrOffset name_ofs = DK_s_get(name);
    dput("dkn_id_add done.\n");
    DK_dks[DK_dks_count].sym = sym;
    DK_dks[DK_dks_count].name_ofs = name_ofs;
    DK_dks_count += 1;
}

/* Convert dead key sym to key name. */
char *DK_dksym_to_s(SC sym) {
    size_t i;
    fori (i, 0, DK_dks_count) {
        if (DK_dks[i].sym == sym) {
            return DK_ofs_to_s(DK_dks[i].name_ofs);
        }
    }
    return nil;
}

/* Convert dead key sym to key action id. */
int DK_dksym_to_ka(SC sym) {
    size_t i;
    fori (i, 0, DK_dks_count) {
        if (DK_dks[i].sym == sym) {
            return i;
        }
    }
    return -1;
}

/* Convert key name to dead key sym. */
int DK_name_to_dksym(char *name) {
    size_t i;
    /* printf(" dkns count: %d\n", DK_dks_count); */
    fori (i, 0, DK_dks_count) {
        if (!strncmp(name, DK_ofs_to_s(DK_dks[i].name_ofs), DK_charbuf_SIZE)) {
            return DK_dks[i].sym;
        } else {
            /* printf(" dkn|%s|\n", DK_ofs_to_s(DK_dks[i].name_ofs)); */
        }
    }
    return -1;
}

/* Convert key name to either { 0, character code }, or { KLM_KA, key action id }. */
DK_Key DK_Key_from_charbuf(char *str) {
#define dput_ret() printf("{%d,%04x}", ret.type, ret.code)
    DK_Key ret;
    size_t i;
    int sym;
    if ((ret.code = ucs2_from_str(str))) {
        ret.type = 0;
        return ret;
    }
    /* dput("!ku "); */
    if ((sym = DK_name_to_dksym(str)) >= 0) {
        ret.type = KLM_KA;
        int code = DK_dksym_to_ka(sym);
        if (code >= 0) {
            /* dput("key_ka"); dput_ret(); */
            //printf(" == ka %02x = %02x\n", sym, code);
            ret.code = code;
            return ret;
        }
    }
    /* dput("!ka [%d]", DK_cns_count); */
    fori (i, 0, DK_cns_count) {
        DK_CharName kn = DK_cns[i];
        char *kn_str = DK_ofs_to_s(kn.name_ofs);
        if (!strncmp(str, kn_str, DK_charbuf_SIZE)) {
            ret.type = 0;
            ret.code = kn.code;
            //printf(" == ch U+%04x\n", ret.code);
            return ret;
        }
        /* dput(" |%s|<>|%s|\n", str, kn_str); */
    }
    /* dput("!kn "); */
    ret.type = 0;
    ret.code = 0;
    /* dput_ret(); */
    //printf(" == |%s| U+%04x\n", str, ret.code);
    return ret;
#undef dput_ret
}

/* Current Compose node */
DK_Node *DK_cur_node = nil;
/* Main (root) Compose node */
DK_Node DK_node;

/* Key name buffers for use in file reading */
DK_charbuf DK_code_buf;
DK_charbuf DK_read_bufs[8];

/*
Assign a Compose sequence to character code.

Read the given number of DK_read_bufs extracting DK_Key-s,
and descend the root Compose node using those keys,
assigning (or leaving in place) child pivot nodes.
Finally, extract the DK_code_buf character code,
and assign it to the deepest child node (a bind one).
*/
void DK_assign(int read_bufs_cnt) {
    int i, cnt1 = read_bufs_cnt - 1;
    DK_Node *node = &DK_node;
    /* Descend n-1 pivot nodes, creating (or converting from bind ones) them as needed. */
    fori (i, 0, cnt1) {
        char *key_name = DK_read_bufs[i];
        DK_Key key = DK_Key_from_charbuf(key_name);
        /* dput("%s {%d,U+%04x}, ", key_name, key.type, key.code); fflush(stdout); */
        node = DK_pivotNode_fetch(node, key, true);
        /* dput_node(node); */
    }
    /* Descend nth node, making it a bind one. */
    {
        char *key_name = DK_read_bufs[cnt1];
        DK_Key key = DK_Key_from_charbuf(key_name);
        /* dput("%s {%d,U+%04x}...", key_name, key.type, key.code); fflush(stdout); */
        node = DK_pivotNode_fetch(node, key, false);
        /* dput_node(node); */
    }
    /* Assign the character code to bind node. */
    {
        DK_Bind bind = DK_Bind_from_charbuf(DK_code_buf);
        node->data.bind = bind;
        /* dput(": %s => U+%04x;\n", DK_code_buf, bind.code); fflush(stdout); */
    }
}

/*
Call SendInput() and reset the current node to root.
*/
void DK_send_code(SC ch) {
    DK_cur_node = &DK_node;
    printf("DKSend U+%04x;\n", ch);
}

/* Convert { 0, character code }, or { KLM_KA, key action id } back to key name. */
char *DK_Key_to_name(DK_Key key) {
    size_t i;
    if (!key.type) {
        fori (i, 0, DK_cns_count) {
            if (key.code == DK_cns[i].code) {
                return DK_ofs_to_s( DK_cns[i].name_ofs );
            }
        }
    } else if (key.code < DK_dks_count) {
        return DK_ofs_to_s( DK_dks[key.code].name_ofs );
    }
    return nil;
}

/* Pretty-print DK_Node and its children */
void DK_print_node(DK_Node *node, int ofs) {
    int i;
    fori (i, 0, ofs) {
        fputc(' ', stdout);
    }
    printf("[node%s]", (node == &DK_node) ? "(g)" : "");
    printf("\"%s\"", DK_Key_to_name(node->key));
    printf("{%d,U+%04x} =>", node->key.type, node->key.code);
    if (!DK_is_pivotNode(node)) {
        printf(" U+%04x\n", node->data.bind.code);
    } else {
        int len = DK_pivotNode_length(node);
        printf(" %d alloc, %d descendants\n", node->count, len-1);
        DK_Node *nodes = DK_pivotNode_nodes(node);
        ofs += 2;
        fori (i, 1, len) {
            DK_print_node(nodes + i, ofs);
        }
        if (len) {
            puts("");
        }
    }
}

/*
Find the node corresponding to DK_Key given in current Compose node.
On failure, reset current node to root.
On success, set the current node to a found one.
*/
void DK_descend(DK_Key key) {
    dputs("DK_descend"); fflush(stdout);
    DK_Node *node = DK_cur_node;
    if (node == nil) {
        dput("DK_descend (%d,%d) on a nil node => reset\n", key.type, key.code); fflush(stdout);
        DK_cur_node = &DK_node;
    } else if (!DK_is_pivotNode(node)) {
        dput("DK_descend (%d,%d) on a bind node => sendinput\n", key.type, key.code); fflush(stdout);
        DK_send_code(node->data.bind.code);
    } else {
        dputs("here1"); fflush(stdout);
        DK_Node *node1 = DK_pivotNode_get(node, key);
        printf("descend { %d, U+%04x }: %snil\n", key.type, key.code, (node1 == nil ? "" : "non-"));
        if (node1 == nil) {
            /* DK_print_node(node, 2); */
            DK_cur_node = &DK_node;
        } else if (DK_is_pivotNode(node1)) {
            DK_cur_node = node1;
        } else {
            printf("descend bind => sendinput\n");
            DK_send_code(DK_bindNode_code(node1));
        }
    }
}

/* Take the action corresponding to n-th dead key being pressed: call DK_descend( { KLM_KA, n } ). */
void DK_dkn(UINT n, bool down, SC sc) {
    if (down) {
        dput("dkn:%d\n", n);
        if ((n < KA_dkn_count)) {
            DK_Key key;
            key.type = KLM_KA;
            key.code = n;
            DK_descend(key);
        }
    }
}

/* Take the action corresponding to character being typed: call DK_descend( { 0, character code } ). */
void DK_on_char(SC ch) {
    DK_Node *node = DK_cur_node;
    if (node != nil) {
        if (DK_is_pivotNode(node)) {
            DK_Key key;
            key.type = 0;
            key.code = ch;
            DK_descend(key);
        }
    }
}

/* Read "/usr/include/X11/keysymdef.h"-like file, assigning key names with DK_cns_push(). */
void DK_read_keydef_file(char *filename) {
    dput("DK_rk %ld %s\n", (long)lenof(DK_read_bufs), filename);
    FILE *f;
    f = fopen(filename, "r");
    size_t nl = 0;
    char *s;
    read_line_buf buf;
    read_line_buf_init(&buf);
    char *name = DK_read_bufs[0];
    long int code;
    while ((s = fread_line(f, &buf)) != nil) {
        ++nl;
        /* dput(" %d %s", nl, s); */
        char c = *s;
#define skip_spc() while ((c == ' ') || (c == '\t')) { c = *(s+=1); }
#define skip_nonspc() while (c && !((c == ' ') || (c == '\t'))) { c = *(s+=1); }
#define forward_char(n) c = *(s+=(n));
#define forward_str(str) do { if (strncmp(s, (str), lenof((str))-1)) { goto next; }; forward_char(lenof((str))-1); } while (0)
#define save_nonspc(buf) do { char *s1 = s; skip_nonspc(); memcpyzn(buf, s1, s-s1, DK_charbuf_SIZE-1); } while (0)
        skip_spc();
        if (c == '#') {
            forward_char(1);
            skip_spc();
            skip_nonspc();
            skip_spc();
            forward_str("XK_");
            save_nonspc(name);
            if (!strncmp(name, "dead_", 5)) {
                skip_spc();
                int sym = strtoul(s, nil, 0x10);
                if (!sym) {
                    goto next;
                }
                int n = DK_dksym_to_ka(sym);
                /* printf("dead_: %s %d:%04x:|%s", name, n, sym, s); */
                if (n > 0) {
                    sym = DK_dks[n].sym;
                }
                if (n != 0) {
                    DK_dksym_add(sym, name);
                }
            }
            skip_spc();
            skip_nonspc();
            skip_spc();
            forward_str("/*");
            skip_spc();
            code = ucs2_from_str(s);
            if (code) {
                DK_cns_push(name, code);
            }
        }
#undef skip_spc
#undef skip_nonspc
#undef forward_char
#undef forward_str
#undef save_nonspc
      next:
        do {} while(0);
    }
    /* DK_print_key_names(); */
    s = nil;
    read_line_buf_free(&buf);
    fclose(f);
}

/* Read "/usr/share/X11/locale/en_US.UTF-8/Compose"-like file, assigning the Compose sequences with DK_assign(). */
void DK_read_compose_file(char *filename) {
    dput("DK_rf %ld %s\n", (long)lenof(DK_read_bufs), filename);
    FILE *f;
    f = fopen(filename, "r");
    size_t nl = 0;
    char *s;
    read_line_buf buf;
    read_line_buf_init(&buf);
    while ((s = fread_line(f, &buf)) != nil) {
        ++nl;
        UINT ki = 0;
        dput(" |%04ld| %s", (long)nl, s);
        do {
            char c = *s;
#define skip_spc() while ((c == ' ') || (c == '\t')) { c = *(s+=1); }
#define skip_nonspc() while (c && !((c == ' ') || (c == '\t'))) { c = *(s+=1); }
#define skip_to_gt() while (c && (c != '>')) { c = *(s+=1); }
#define skip_to_dq() while (c && (c != '"')) { c = *(s+=1); }
#define forward_char() do { if (c) { c = *(s+=1); }; } while (0)
            skip_spc();
            /* dput(" >|%s", s); */
            if (!c) {
                break;
            } else if (c == '#') {
                /* dput("%04d %s", nl, s); */
                break;
            } else if (c == '<') {
                if (ki >= lenof(DK_read_bufs)) {
                    printf("Too many keys (%d)\n", ki);
                    return;
                }
                char *s1 = s + 1;
                skip_to_gt();
                ssize_t l1 = s - s1;
                memcpyzn(DK_read_bufs[ki], s1, l1, DK_charbuf_SIZE-1);
                s+=1;
                ++ki;
            } else if (c == ':') {
                forward_char();
                skip_spc();
                if (*s == '"') {
                    forward_char();
                    skip_to_dq();
                    forward_char();
                    skip_spc();
                    char *s1 = s;
                    skip_nonspc();
                    size_t l1 = s - s1;
                    memcpyzn(DK_code_buf, s1, l1, DK_charbuf_SIZE-1);
                }
                bool is_dead = !strncmp("dead_", DK_read_bufs[0], 5);
                bool is_multi = !strncmp("Multi_key", DK_read_bufs[0], 9);
                if (ki && (is_dead || is_multi)) {
                    if (is_multi)
                        puts("multi_assign");
                    DK_assign(ki);
                }
                break;
            } else {
                break;
            }
#undef skip_spc
#undef skip_nonspc
#undef skip_to_gt
#undef skip_to_dq
#undef forward_char
        } while (1);
        /* dputs("next"); */
    }
    dput("End of compose file read\n");
    read_line_buf_free(&buf);
    fclose(f);
}

/* Prerequisities. */
void DK_init() {
    DK_bindNode_toPivot(&DK_node);
    DK_cns = (DK_CharName*) malloc(sizeof(DK_CharName) * (DK_cns_alloc = 64));
    DK_s = (char*)malloc((DK_s_alloc = 16*1024));
    DK_cur_node = &DK_node;
    char multi_key[] = "Multi_key";
    DK_dksym_add(0, multi_key);
}
