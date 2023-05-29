#ifndef _EVALUATOR_H
#define _EVALUATOR_H

#include "types.h"

bool get_expression_type(char* expr);
s64 eval_int(char* expr);
float eval_float(char* expr);


#endif /* _INTERPRETER_H */