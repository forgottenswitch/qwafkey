#ifndef PARSE_H_INCLUDED
#define PARSE_H_INCLUDED

#include "stdafx.h"

/* parse.h -- parsing and lexing of key binding assignments */

void parse_args(int argc, char *argv[], int argb);
void parse_str(char *str);

extern char *parse_failed_lines;
extern int parse_failed_lines_count;
extern bool parse_nodefault_compose;
extern bool parse_nodefault_keydefs;
bool parse_str_has_nodefault(char *str);

#endif // PARSE_H_INCLUDED
