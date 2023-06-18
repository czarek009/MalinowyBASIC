#ifndef _VARIABLE_H
#define _VARIABLE_H

#include "types.h"

#define POINTER             ((u8)0)
#define INTEGER             ((u8)1)
#define FLOATING_POINT      ((u8)2)
#define BOOLEAN             ((u8)3)
#define STRING              ((u8)4)
#define OPERATOR            ((u8)5)
#define NOT_FOUND           ((u8)253)
#define NOT_DECLARED        ((u8)254)
#define EVAL_ERROR          ((u8)255)

typedef union variableDataU {
    u64 *pointer;
    s64 integer;
    float floating_point;
    bool boolean;
    char *string;
} variableDataU;


#endif /* _VARIABLE_H */