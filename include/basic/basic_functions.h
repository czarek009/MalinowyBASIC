#ifndef _BASIC_FUNCTIONS_H
#define _BASIC_FUNCTIONS_H

#include "types.h"


/* general */
s8 sgn(double arg);
s64 abs(s64 arg);
s64 int(double arg);
double exp(double arg);
double log(double arg);
double sqr(double arg);
double rnd(void);

/* strings/chars */
u8 asc(char* arg);
s64 val(char* arg);
s64 len(char* arg);
char chr(u8 arg);
char* tab(u64 arg);
char* str(u64 arg);
char* left(char* arg1, u64 arg2);
char* right(char* arg1, u64 arg2);
char* mid(char* arg1, u64 arg2, u64 arg3);


/* trygonometric functions */
double fnatn(double arg);
double fncps(double arg);
double fnsin(double arg);
double fntan(double arg);


#endif /* _BASIC_FUNCTIONS_H */