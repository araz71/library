#ifndef TOOLS_H_
#define TOOLS_H_

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>

char* extract_number(const char* str, int* result, int* ok);
int extract_nth_number(const char* str, int nth, int* ok);

char* separate_string_by(char* msg, const char delim, char** next);

int get_token(const char* str, const char* deli, int* fin);
#endif
