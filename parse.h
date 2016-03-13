#ifndef PARSE_H_INCLUDED
#define PARSE_H_INCLUDED

#include "stdafx.h"

/* parse.h -- parsing and lexing of key binding assignments */

char *parse_failed_lines;
int parse_failed_lines_count;

void parse_args(int argc, char *argv[], int argb);
void parse_str(char *str);

#endif // PARSE_H_INCLUDED
