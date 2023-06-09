#include "evaluator.h"
#include "types.h"
#include "butils.h"


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

/* --------------------------------------------------------- */

char *operator_push = "+-*/([%";

bool isoperator(char c) {
  for(u8 i = 0; operator_push[i] != '\0'; i++){
    if(c == operator_push[i]){
      return true;
    }
  }
  return false;
}

/* EXPR DATA */
ExprData init_ExprData(void) {
  ExprData data;
  data.expr_number = 0;
  return data;
}

void add_data(ExprData *data, char val) {
  if(data->expr_number < STACK_LENGTH) {
    data->expr[data->expr_number] = val;
    data->expr_number++;
    return;
  }
  DEBUG("EVAL - ADD_DATA - ExprData is FULL\n");
}

char pop_data(ExprData *data){
  if(data->expr_number > 0) {
    data->expr_number--;
    return data->expr[data->expr_number];
  }
  DEBUG("EVAL - POP_DATA - ExprData is empty\n");
  return '\0';
}

char peek(ExprData *data){
  if(data->expr_number > 0) {
    return data->expr[data->expr_number - 1];
  }
  DEBUG("EVAL - PEEK - ExprData is empty\n");
  return '\0';
}

void add_operator(ExprData *op_stack, ExprData *expr_data, char op) {
  char op_from_stack = peek(op_stack);
  if((op == '+' || op == '-') && (op_from_stack == '*' || op_from_stack == '/')) {
    add_data(expr_data, pop_data(op_stack));
  }
  add_data(op_stack, op);
}

void move_op_to_expr(ExprData *op_stack, ExprData *expr_data, char op) {
  char c = pop_data(op_stack);
  while(c != op){
    add_data(expr_data, c);
    c = pop_data(op_stack);
  }
}

void add_remaining_op(ExprData *op_stack, ExprData *expr_data) {
  while(op_stack->expr_number > 0){
    add_data(expr_data, pop_data(op_stack));
  }
}

void print_ExprData(ExprData *data) {
  for(u8 i = 0; i < data->expr_number; i++) {
    DEBUG("%c\n", data->expr[i]);
  }
}

ExprData transform(char *expr) {
  ExprData op_stack = init_ExprData();
  ExprData expr_data = init_ExprData();

  for(u8 i = 0; expr[i] != '\0'; i++){
    if(isdigit(expr[i])){
      add_data(&expr_data, expr[i]);
    }
    else if(isoperator(expr[i])){
      add_operator(&op_stack, &expr_data, expr[i]);
    }
    else if(expr[i] == ')') {
      move_op_to_expr(&op_stack, &expr_data, '(');

    }
    else if(expr[i] == ']') {
      move_op_to_expr(&op_stack, &expr_data, '[');
    }
  }
  add_remaining_op(&op_stack, &expr_data);
  return expr_data;
}

/* EVAL STACK */
EvalStack init_EvalStack(void) {
  EvalStack stack;
  stack.stack_pointer = 0;
  return stack;
}

void add_data_EvalStack(EvalStack *data, s64 val) {
  if(data->stack_pointer < STACK_LENGTH) {
    data->stack[data->stack_pointer] = val;
    data->stack_pointer++;
    return;
  }
  DEBUG("EVAL - ADD_DATA - EvalStack is FULL\n");
}

s64 pop_data_EvalStack(EvalStack *data){
  if(data->stack_pointer > 0) {
    data->stack_pointer--;
    return data->stack[data->stack_pointer];
  }
  DEBUG("EVAL - POP_DATA - EvalStack is empty\n");
  return '\0';
}

s64 atoi(char c){
  return (s64)(c - '0');
}

s64 eval_bin(s64 first, s64 second, char op){
  switch(op) {
    case '+':
      return first + second;
    case '-':
      return first - second;
    case '*':
      return first * second;
    case '/':
      return first / second;
    case '%':
      return first % second;
    default:
      return 0;
  }
}

s64 eval_int(char* expr) {
  EvalStack eval_stack = init_EvalStack();
  ExprData expr_data = transform(expr);
  u8 expr_number = expr_data.expr_number;
  for(u8 i = 0; i < expr_number; i++){
    if(isdigit(expr_data.expr[i])){
      add_data_EvalStack(&eval_stack, atoi(expr_data.expr[i]));
    }
    if(isoperator(expr_data.expr[i])) {
      s64 second = pop_data_EvalStack(&eval_stack);
      s64 first = pop_data_EvalStack(&eval_stack);
      add_data_EvalStack(&eval_stack, eval_bin(first, second, expr_data.expr[i]));
    }
  }
  return pop_data_EvalStack(&eval_stack);
}