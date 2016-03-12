#ifndef PARSE_H_INCLUDED
#define PARSE_H_INCLUDED

#include "stdafx.h"

/* parse.h -- parsing and lexing of key binding assignments */

void parse_args(int argc, char *argv[], int argb);
void parse_str(char *str);

#endif // PARSE_H_INCLUDED
