#ifndef _EVALUATOR_H
#define _EVALUATOR_H

#include "types.h"
#include "session.h"


#define STACK_LENGTH 32

typedef struct ExprData {
    u8 expr_number;
    char expr[STACK_LENGTH];
} ExprData;

typedef struct EvalStack {
    u8 stack_pointer;
    s64 stack[STACK_LENGTH];
} EvalStack;


void get_expression_type(char* expr);
s64 eval_int_expr(char* expr);
float eval_float_expr(char* expr);
bool eval_bool_expr(Session* env, char* expr);
void eval_expr(void*, char* expr);


#endif /* _INTERPRETER_H */