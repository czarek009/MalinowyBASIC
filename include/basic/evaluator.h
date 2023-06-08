#ifndef _EVALUATOR_H
#define _EVALUATOR_H

#include "types.h"
#include "session.h"

void get_expression_type(char* expr);
s64 eval_int_expr(char* expr);
float eval_float_expr(char* expr);
bool eval_bool_expr(Session* env, char* expr);
void eval_expr(void*, char* expr);


#endif /* _INTERPRETER_H */