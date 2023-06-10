#include "evaluator.h"
#include "types.h"
#include "butils.h"


/*
TODO:
 - BRAK czytania wartości zmiennych ze środowiska
 - lepsze zaokrąglanie floatów ? - teraz ucina to co jest po przecinku
 - dodać sprawdzenie poprawności wyrażenia
*/

char *operator_push = "+-*/%^";

bool isoperator(char c) {
  for(u8 i = 0; operator_push[i] != '\0'; i++){
    if(c == operator_push[i]){
      return true;
    }
  }
  return false;
}

bool is_lparen(char c) {
  return (c == '(') || (c == '[');
}

bool is_rparen(char c) {
  return (c == ')') || (c == ']');
}

char get_lparen(char c) {
  return (c == ')')? '(': '[';
}

/*                           - % /    ^    * +    */
u8 operator_precedence[9] = {0,1,1,-1,2,-1,1,0,-1};
u8 precedence(char op) {
  return operator_precedence[op % 9];
}

bool is_negative(char *expr){
  return ((*expr == '-') && isdigit(*(expr + 1)));
}

s64 atoi(char *str){
    s64 res = 0;
    u8 i = 0;
    s64 neg = 1;
    if(str[0] == '-'){
      i = 1;
      neg  = -1;
    }
    for(; (str[i] != '\0' && str[i] != '.'); i++) {
      res = res * 10 + (s64)(str[i] - '0');
    }
  return neg * res;
}

s64 pow(s64 first, s64 second) {
  s64 res = 1;
  while(second > 0) {
    if(second%2){
      res *= first;
    }
    first *= first;
    second >>= 1;
  }
  return res;
}

/* OP STACK */
OpStack init_OpStack(void) {
  OpStack stack;
  stack.stack_pointer = 0;
  return stack;
}

void push_OpStack(OpStack *data, char val) {
  if(data->stack_pointer < STACK_LENGTH) {
    data->stack[data->stack_pointer] = val;
    data->stack_pointer++;
    return;
  }
  DEBUG("EVAL - PUSH: OpStack is FULL\n");
}

char pop_OpStack(OpStack *data){
  if(data->stack_pointer > 0) {
    data->stack_pointer--;
    return data->stack[data->stack_pointer];
  }
  DEBUG("EVAL - POP: OpStack is empty\n");
  return '\0';
}

char peek_OpStack(OpStack *data){
  if(data->stack_pointer > 0) {
    return data->stack[data->stack_pointer - 1];
  }
  return '\0';
}

void print_OpStack(OpStack *data) {
  for(u8 i = 0; i < data->stack_pointer; i++) {
    printf("%c\n", data->stack[i]);
  }
}

/* EXPR DATA */
ExprData init_ExprData(void) {
  ExprData data;
  data.expr_number = 0;
  return data;
}

void push_ExprData(ExprData *data, char* val, u8 length) {
  if(data->expr_number < STACK_LENGTH) {
    char *str = malloc(length + 1);
    memcpy(str, val, length);
    str[length] = '\0';
    data->expr[data->expr_number] = str;
    data->expr_number++;
    return;
  }
  DEBUG("EVAL - PUSH: ExprData is FULL\n");
}

void free_ExprData(ExprData *data){
  for(u8 i = 0; i < data->expr_number; i++){
    free(data->expr[i]);
  }
  data->expr_number = 0;
}

void print_ExprData(ExprData *data){
  for(u8 i = 0; i < data->expr_number; i++) {
    printf("%s ", data->expr[i]);
  }
  printf("\n");
}

/* TRANSFORM */
void add_operator(OpStack *op_stack, ExprData *expr_data, char op) {
  char op_from_stack = peek_OpStack(op_stack);
  while((op_from_stack != '\0') && !is_lparen(op_from_stack) && (precedence(op) <= precedence(op_from_stack))){
    char pop = pop_OpStack(op_stack);
    push_ExprData(expr_data, &pop, 1);
    op_from_stack = peek_OpStack(op_stack);
  }
  push_OpStack(op_stack, op);
}

void move_op_to_expr(OpStack *op_stack, ExprData *expr_data, char op) {
  char c = pop_OpStack(op_stack);
  while(c != op){
    push_ExprData(expr_data, &c, 1);
    c = pop_OpStack(op_stack);
  }
}

void add_remaining_op(OpStack *op_stack, ExprData *expr_data) {
  while(op_stack->stack_pointer > 0){
    char c = pop_OpStack(op_stack);
    push_ExprData(expr_data, &c, 1);
  }
}

u8 get_number_length(char *expr){
  u8 i = (*expr == '-');
  bool one_dot = 1;
  for(; isdigit(expr[i]) || (expr[i] == '.' && one_dot); i++){
    if(expr[i] == '.'){
      one_dot = 0;
    }
  }
  return i;
}

ExprData transform(char *expr) {
  OpStack op_stack = init_OpStack();
  ExprData expr_data = init_ExprData();
  bool possible_negative = 1;

  while(*expr != '\0'){
    if(isdigit(*expr) || (possible_negative && is_negative(expr))){
      u8 length = get_number_length(expr);
      push_ExprData(&expr_data, expr, length);
      possible_negative = 0;
      expr = expr + length;
    }
    else if(isoperator(*expr)){
      add_operator(&op_stack, &expr_data, *expr);
      possible_negative = 1;
      expr++;
    }
    else if(is_lparen(*expr)){
      push_OpStack(&op_stack, *expr);
      possible_negative = 1;
      expr++;
    }
    else if(is_rparen(*expr)){
      move_op_to_expr(&op_stack, &expr_data, get_lparen(*expr));
      possible_negative = 0;
      expr++;
    }
    else {
      possible_negative = 0;
      expr++;
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

void push_EvalStack(EvalStack *data, s64 val) {
  if(data->stack_pointer < STACK_LENGTH) {
    data->stack[data->stack_pointer] = val;
    data->stack_pointer++;
    return;
  }
  DEBUG("EVAL - PUSH: EvalStack is FULL\n");
}

s64 pop_EvalStack(EvalStack *data){
  if(data->stack_pointer > 0) {
    data->stack_pointer--;
    return data->stack[data->stack_pointer];
  }
  DEBUG("EVAL - POP: EvalStack is empty\n");
  return '\0';
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
    case '^':
      return pow(first, second);
    case '%':
      return first % second;
    default:
      return 0;
  }
}

s64 eval_int_expr(char* expr) {
  EvalStack eval_stack = init_EvalStack();
  ExprData expr_data = transform(expr);
  u8 expr_number = expr_data.expr_number;
  for(u8 i = 0; i < expr_number; i++){
    if(isdigit(*expr_data.expr[i]) || is_negative(expr_data.expr[i])){
      push_EvalStack(&eval_stack, atoi(expr_data.expr[i]));
    }
    else if(isoperator(*expr_data.expr[i])) {
      s64 second = pop_EvalStack(&eval_stack);
      s64 first = pop_EvalStack(&eval_stack);
      push_EvalStack(&eval_stack, eval_bin(first, second, *expr_data.expr[i]));
    }
  }
  free_ExprData(&expr_data);
  return pop_EvalStack(&eval_stack);
}