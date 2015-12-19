#ifndef DK_H_INCLUDED
#define DK_H_INCLUDED

#include "stdafx.h"

/* forward declaration for recursive struct */
typedef struct DK_Node DK_Node;

typedef struct {
    DK_Node *nodes;
} DK_Pivot;
typedef struct {
    uint16_t code;
} DK_Bind;
typedef union {
    DK_Pivot pivot;
    DK_Bind bind;
} DK_NodeData;

/*
A key pressed.
Could be { 0, character code }, or { KLM_KA, key action id }.
*/
typedef struct {
    uint8_t type;
    uint16_t code;
} DK_Key;

/*
The element of Compose sequences tree.
Maps .key (a character typed) to .data (either more DK_Node-s, or a character).
.key holds the associated UCS-2 character code or key action id.
A DK_Node is either:
 a pivot - .count > 0, .data.node points to array of DK_Node .count elements in size, first holding array length.
 a bind - .count <= 0, .data.bind holds the character code to simulate;
*/
struct DK_Node {
    int count;
    DK_Key key;
    DK_NodeData data;
};

void DK_init();

void DK_read_compose_file(char *path);
void DK_read_keydef_file(char *path);

void DK_dkn(UINT n, bool down, SC sc);
void DK_on_char(SC ch);

void DK_print_key_names();

DK_Node *DK_pivotNode_fetch(DK_Node * const node, DK_Key key, bool pivot);
void DK_descend(DK_Key key);
void DK_print_node(DK_Node *node, int ofs);

extern DK_Node DK_node;

#endif // DK_H_INCLUDED
