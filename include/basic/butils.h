#ifndef _BUTILS_H
#define _BUTILS_H

#include "types.h"

bool isalpha(char c);
bool isdigit(char c);
bool isalphanum(char c);
bool isin(char c, char* p);
size_t strlen(char* s);
int strncmp(const char *src1, const char *src2, register size_t n);
void strncpy(char *dest, const char *src, size_t n);

s64 str2s64(char* str);
u8 is_valid_varname(char* cmd);
u8 strcmp(char *str1, char *str2);

#endif /* _BUTILS_H */