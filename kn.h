#ifndef KN_H_INCLUDED
#define KN_H_INCLUDED

/* kn.h -- key names */

#include "stdafx.h"

void KN_init(void);
VK KN_name_to_vk(char *name);

SC KN_lname_to_sc(char *name);
const char* KN_sc_to_str(SC sc);

#endif // KN_H_INCLUDED

