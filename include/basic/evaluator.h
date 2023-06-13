#ifndef _EVALUATOR_H
#define _EVALUATOR_H

#include "types.h"
#include "session.h"
#include "printf.h"
#include "mm.h"

#define STACK_LENGTH 32

typedef struct OpStack {
  u8 stack_pointer;
  char stack[STACK_LENGTH];
} OpStack;

typedef struct ExprData {
    u8 expr_number;
    char *expr[STACK_LENGTH];
} ExprData;

typedef struct EvalStack {
  u8 stack_pointer;
  s64 stack[STACK_LENGTH];
} EvalStack;

/* ------------------------------------------------------ */


enum opE {
  OP_ADD,
  OP_SUB,
  OP_DIV,
  OP_MUL,
  OP_POW,
  OP_MOD,
  OP_LPARENT,
  OP_RPARENT,
} typedef opE;

enum itemE {
  OPERATOR,
  INT_NUM,
  FLOAT_NUM,
  VARIABLE,
  FUNCTION,
} typedef itemE;

union itemU {
  opE op_id;
  s64 int_val;
  // float float_val;
  // char* varname;
  // char* funname;
} typedef itemU;

struct itemS {
  itemE item_type;
  itemU item_val;
} typedef itemS;



void get_expression_type(char* expr);
s64 eval_int_expr(Session *s, char** expr);
float eval_float_expr(char* expr);
bool eval_bool_expr(Session* env, char* expr);
void eval_expr(void*, char* expr);


#endif /* _INTERPRETER_H */