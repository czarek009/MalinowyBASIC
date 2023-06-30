#include "evaluator.h"
#include "types.h"
#include "butils.h"
#include "printf.h"
#include "mm.h"

#define ERROR(...)  printf(__VA_ARGS__)

#define MAX_DATA_NUMBER     32
#define MAX_OPERATOR_NUMBER (MAX_DATA_NUMBER - 1) / 2

typedef enum {
  OP_ADD,          /* +  */
  OP_SUB,          /* -  */
  OP_DIV,          /* /  */
  OP_MUL,          /* *  */
  OP_POW,          /* ^  */
  OP_MOD,          /* %  */
  OP_LPARENT,      /* (  */
  OP_RPARENT,      /* )  */
  OP_AND,          /* && */
  OP_OR,           /* || */
  OP_NOT,          /* !  */
  OP_EQ,           /* == */
  OP_NEQ,          /* != */
  OP_GT,           /* >  */
  OP_LT,           /* <  */
  OP_GEQ,          /* >= */
  OP_LEQ,          /* <= */
  OP_NONE
} opE;

typedef union {
  opE op;
  bool boolean;
  s64 integer;
  double floating_point;
  char *string;
} dataU;

typedef struct {
  u8 expr_number;
  u8 types[MAX_DATA_NUMBER];
  dataU exprs[MAX_DATA_NUMBER];
} exprDataS;

typedef struct {
  u8 stack_pointer;
  u8 stack[MAX_OPERATOR_NUMBER];
} opStackS;

typedef enum {
  EVAL_SUCCESS,
  EVAL_INTERNAL_ERROR
} evalErrorE;

evalErrorE error_g = EVAL_SUCCESS;

/*                       +  -  /  *  ^  %  (  ) && ||  ! == !=  >  <  >= <=  */
u8 op_precedenceA[17] = {3, 3, 4, 4, 5, 4, 0, 0, 2, 1, 3, 1, 1, 2, 2, 2, 2};

/* types */
void print_types(u8 type) {
  switch(type) {
    case INTEGER:
      printf("integer");
      return;
    case FLOATING_POINT:
      printf("floating_point");
      break;
    case BOOLEAN:
      printf("boolean");
      break;
    case STRING:
      printf("string");
      break;
    case OPERATOR:
      printf("operator");
      break;
    default:
      ERROR("[!] EVAL - print_types: type not supported = %d\n", type);
      error_g = EVAL_INTERNAL_ERROR;
      return;
  }
}

/* opE */
void print_opE(opE op) {
  switch(op) {
    case OP_ADD:
      printf("+ ");
      return;
    case OP_SUB:
      printf("- ");
      return;
    case OP_DIV:
      printf("/ ");
      return;
    case OP_MUL:
      printf("* ");
      return;
    case OP_POW:
      printf("^ ");
      return;
    case OP_MOD:
      printf("%% ");
      return;
    case OP_LPARENT:
      printf("( ");
      return;
    case OP_RPARENT:
      printf(") ");
      return;
    case OP_AND:
      printf("&& ");
      return;
    case OP_OR:
      printf("|| ");
      return;
    case OP_NOT:
      printf("! ");
      return;
    case OP_EQ:
      printf("== ");
      return;
    case OP_NEQ:
      printf("!= ");
      return;
    case OP_GT:
      printf("> ");
      return;
    case OP_LT:
      printf("< ");
      return;
    case OP_GEQ:
      printf(">= ");
      return;
    case OP_LEQ:
      printf("<= ");
      return;
    case OP_NONE:
      return;
    default:
      ERROR("[!] EVAL - print_opE: there in no such an operator\n");
      error_g = EVAL_INTERNAL_ERROR;
      return;
  }
}

double powfu(double x, u64 p) {
  double result = 1;
  while (p > 0) {
    if (p & 0x1) {
      result *= x;
    }
    x *= x;
    p >>= 1;
  }
  return result;
}

double powfi(double x, s64 p) {
  if (p < 0) {
    return 1.0 / powfu(x , (u64)(-1 * p));
  }
  return powfu(x, p);
}

double fact2(u8 n) {
  if (n < 0 || n >= 172) return 0;
  double result = 1;
  for (u8 i = 2; i <= n; i++) {
    result *= i;
  }
  return result;
}

double exp2(double x) {
  if (x < 0) return 0;
  double result = 0;
  for (u8 n = 0; n < 100; n++) {
    result += powfi(x, n) / fact2(n);
  }
  return result;
}

double ln2(double x) {
  if (x <= 0) return 0;

  double result = 0;
  for (u32 n = 1; n < 1000; n++) {
    result += powfu((x-1)/(x+1), 2*n-1)/(2*n-1);
  }
  return 2*result;
}

double powff(double x, double p) {
  if (x <= 0.0) return 0.0;

  bool neg_p = 0;
  if (p < 0.0) {
    p = (-1.0 * p);
    neg_p = 1;
  }

  s64 int_power = (s64)p;

  double int_pow_result = powfi(x, int_power);

  double remaining_power = p - int_power;
  double remaining_result = exp2(remaining_power*ln2(x));
  double result = int_pow_result * remaining_result;
  if(neg_p){
    result = 1.0 / result;
  }
  return result;
}


/* EVAL INT*/
u8 eval_int(s64 first, s64 second, dataU *res, opE op) {
  switch(op) {
    case OP_ADD:
      res->integer = first + second;
      return INTEGER;
    case OP_SUB:
      res->integer = first - second;
      return INTEGER;
    case OP_DIV:
      if(second == 0){
        res->integer = 0;
        ERROR("[!] EVAL - division by zero!\n");
        error_g = EVAL_INTERNAL_ERROR;
        return EVAL_ERROR;
      }
      res->integer = first / second;
      return INTEGER;
    case OP_MUL:
      res->integer = first * second;
      return INTEGER;
    case OP_POW:
      res->integer = (s64)powfi(first, second);
      return INTEGER;
    case OP_MOD:
      res->integer = first % second;
      return INTEGER;
    case OP_AND:
      res->boolean = first && second;
      return BOOLEAN;
    case OP_OR:
      res->boolean = first || second;
      return BOOLEAN;
    case OP_EQ:
      res->boolean = first == second;
      return BOOLEAN;
    case OP_NEQ:
      res->boolean = first != second;
      return BOOLEAN;
    case OP_GT:
      res->boolean = first > second;
      return BOOLEAN;
    case OP_LT:
      res->boolean = first < second;
      return BOOLEAN;
    case OP_GEQ:
      res->boolean = first >= second;
      return BOOLEAN;
    case OP_LEQ:
      res->boolean = first <= second;
      return BOOLEAN;
    default:
      error_g = EVAL_INTERNAL_ERROR;
      ERROR("[!] EVAL - eval_int: unknown operator\n");
      return EVAL_ERROR;
  }
}

/* EVAL DOUBLE */
u8 eval_double(double first, double second, dataU *res, opE op) {
  switch(op) {
    case OP_ADD:
      res->floating_point = first + second;
      return FLOATING_POINT;
    case OP_SUB:
      res->floating_point = first - second;
      return FLOATING_POINT;
    case OP_DIV:
      if(second == 0.0){
        res->floating_point = 0.0;
        ERROR("[!] EVAL - division by zero!\n");
        error_g = EVAL_INTERNAL_ERROR;
        return EVAL_ERROR;
      }
      res->floating_point = first / second;
      return FLOATING_POINT;
    case OP_MUL:
      res->floating_point = first * second;
      return FLOATING_POINT;
    case OP_POW:
      res->floating_point = powff(first, second);
      return FLOATING_POINT;
    case OP_MOD:
      error_g = EVAL_INTERNAL_ERROR;
      ERROR("[!] EVAL - eval_double: modulo operands must be integers\n");
      return EVAL_ERROR;
    case OP_AND:
      res->boolean = first && second;
      return BOOLEAN;
    case OP_OR:
      res->boolean = first || second;
      return BOOLEAN;
    case OP_EQ:
      res->boolean = first == second;
      return BOOLEAN;
    case OP_NEQ:
      res->boolean = first != second;
      return BOOLEAN;
    case OP_GT:
      res->boolean = first > second;
      return BOOLEAN;
    case OP_LT:
      res->boolean = first < second;
      return BOOLEAN;
    case OP_GEQ:
      res->boolean = first >= second;
      return BOOLEAN;
    case OP_LEQ:
      res->boolean = first <= second;
      return BOOLEAN;
    default:
      error_g = EVAL_INTERNAL_ERROR;
      ERROR("[!] EVAL - eval_double: unknown operator\n");
      return EVAL_ERROR;
  }
}

u8 eval_string(char *first, char *second, dataU *res, opE op){
  switch(op){
    case OP_ADD:
      {
        u8 first_size = strlen(first);
        u8 second_size = strlen(second);
        char *res_str = malloc(first_size + second_size + 1);
        memcpy(res_str, first, first_size);
        memcpy(res_str + first_size, second, second_size + 1);
        res->string = res_str;
      }
      free(first);
      free(second);
      return STRING;
    case OP_EQ:
      if(strcmp(first, second)){
        res->boolean = 0;
      }
      else{
        res->boolean = 1;
      }
      free(first);
      free(second);
      return BOOLEAN;
    case OP_NEQ:
      res->boolean = (first != second);
      free(first);
      free(second);
      return BOOLEAN;
    default:
      error_g = EVAL_INTERNAL_ERROR;
      ERROR("[!] EVAL - eval_string: operator cannot be used on string\n");
      free(first);
      free(second);
      return EVAL_ERROR;
  }
}

u8 eval_not(dataU data, u8 type, dataU *res) {
  switch(type) {
    case INTEGER:
      res->boolean = !data.integer;
      break;
    case FLOATING_POINT:
      res->boolean = !data.floating_point;
      break;
    case BOOLEAN:
      res->boolean = !data.boolean;
      break;
    case STRING:
      res->boolean = !data.string;
      break;
    default:
      error_g = EVAL_INTERNAL_ERROR;
      ERROR("[!] EVAL - eval_not: unknown operator\n");
      return EVAL_ERROR;
  }
  return BOOLEAN;
}

u8 eval_bin(dataU first, dataU second, u8 first_type, u8 second_type, dataU *res, opE op) {
  if(first_type == STRING) {
    switch(second_type) {
      case STRING:
        return eval_string(first.string, second.string, res, op);
      default:
        error_g = EVAL_INTERNAL_ERROR;
        ERROR("[!] EVAL - eval_bin: operation between string and other type\n");
        return EVAL_ERROR;
    }
  }
  else if(first_type == INTEGER) {
    switch(second_type) {
      case INTEGER:
        return eval_int(first.integer, second.integer, res, op);
      case FLOATING_POINT:
        return eval_int(first.integer, (s64)second.floating_point, res, op);
      case BOOLEAN:
        return eval_int(first.integer, (s64)second.boolean, res, op);
      default:
        error_g = EVAL_INTERNAL_ERROR;
        ERROR("[!] EVAL - eval_bin: operation between integer and type that dont match\n");
        return EVAL_ERROR;
    }
  }
  else if(first_type == FLOATING_POINT) {
    switch(second_type) {
      case FLOATING_POINT:
        return eval_double(first.floating_point, second.floating_point, res, op);
      case INTEGER:
        return eval_int((s64)first.floating_point, second.integer, res, op);
      case BOOLEAN:
        return eval_double(first.floating_point, (double)second.boolean, res, op);
      default:
        error_g = EVAL_INTERNAL_ERROR;
        ERROR("[!] EVAL - eval_bin: operation between floating_point and type that dont match\n");
        return EVAL_ERROR;
    }
  }
  else if(first_type == BOOLEAN) {
    switch(second_type) {
      case INTEGER:
        return eval_int((s64)first.boolean, second.integer, res, op);
      case FLOATING_POINT:
        return eval_double((double)first.boolean, second.floating_point, res, op);
      case BOOLEAN:
        return eval_int((s64)first.boolean, (s64)second.boolean, res, op);
      default:
        error_g = EVAL_INTERNAL_ERROR;
        ERROR("[!] EVAL - eval_bin: operation between boolean and type that dont match\n");
        return EVAL_ERROR;
    }
  }
  else {
    error_g = EVAL_INTERNAL_ERROR;
    ERROR("[!] EVAL - eval_bin: type of operand is invalid\n");
    return EVAL_ERROR;
  }
}

/* opStackS */
opStackS init_opStackS(void) {
  opStackS stack;
  stack.stack_pointer = 0;
  return stack;
}

void push_opStackS(opStackS *data, u8 op) {
  if(data->stack_pointer < MAX_OPERATOR_NUMBER) {
    data->stack[data->stack_pointer] = op;
    data->stack_pointer++;
    return;
  }
  error_g = EVAL_INTERNAL_ERROR;
  ERROR("[!] EVAL - push_opStackS: stack is FULL\n");
}

u8 pop_opStackS(opStackS *data) {
  if(data->stack_pointer > 0) {
    data->stack_pointer--;
    return data->stack[data->stack_pointer];
  }
  return OP_NONE;
}

u8 peek_opStackS(opStackS *data) {
  if(data->stack_pointer > 0) {
    return data->stack[data->stack_pointer - 1];
  }
  return OP_NONE;
}

void print_opStackS(opStackS *data) {
  printf("opStackS:\n");
  for(u8 i = 0; i < data->stack_pointer; i++) {
    print_opE(data->stack[i]);
  }
  printf("\n");
}

/* exprDataS */
exprDataS init_exprDataS(void) {
  exprDataS data;
  data.expr_number = 0;
  return data;
}

u8 pop_exprDataS(exprDataS *expr_data, dataU *data) {
  if(expr_data->expr_number > 0) {
    expr_data->expr_number--;
    *data = expr_data->exprs[expr_data->expr_number];
    return expr_data->types[expr_data->expr_number];
  }
  return EVAL_ERROR;
}

void push_exprDataS(exprDataS *expr_data, dataU data, u8 type) {
  if(type == OPERATOR) {
    dataU second;
    opE op = data.op;
    u8 second_type = pop_exprDataS(expr_data, &second);
    if(op == OP_NOT) {
      type = eval_not(second, second_type, &data);
    }
    else {
      dataU first;
      u8 first_type = pop_exprDataS(expr_data, &first);
      type = eval_bin(first, second, first_type, second_type, &data, op);
    }
  }
  if (expr_data->expr_number < MAX_DATA_NUMBER) {
    expr_data->exprs[expr_data->expr_number] = data;
    expr_data->types[expr_data->expr_number] = type;
    expr_data->expr_number++;
    return;
  }
  error_g = EVAL_INTERNAL_ERROR;
  ERROR("[!] EVAL - push_exprDataS: expr_data is FULL\n");
}

u8 pop_result(exprDataS *expr_data, variableDataU *data) {
  if(expr_data->expr_number > 0) {
    u8 i = expr_data->expr_number - 1;
    u8 type = expr_data->types[i];
    switch(type){
      case INTEGER:
        data->integer = expr_data->exprs[i].integer;
        return type;
      case FLOATING_POINT:
        data->floating_point = expr_data->exprs[i].floating_point;
        return type;
      case BOOLEAN:
        data->boolean = expr_data->exprs[i].boolean;
        return type;
      case STRING:
        data->string = expr_data->exprs[i].string;
        return type;
      default:
        break;
    }
  }
  ERROR("[!] EVAL - pop_result: no matching data in eval_stack\n");
  return EVAL_ERROR;
}

void free_exprDataS(exprDataS *expr_data) {
  for(u8 i = 0; i < expr_data->expr_number; i++) {
    if(expr_data->types[i] == STRING){
      free(expr_data->exprs[i].string);
    }
  }
}

void print_exprDataS(exprDataS *expr_data) {
  for(u8 i = 0; i < expr_data->expr_number; i++) {
    switch(expr_data->types[i]) {
      case INTEGER:
        printf("%ld ", expr_data->exprs[i].integer);
        break;
      case FLOATING_POINT:
        printf("%ld ", (s64)(expr_data->exprs[i].floating_point));
        break;
      case BOOLEAN:
        if(expr_data->exprs[i].boolean) {
          printf("true ");
        }
        else {
          printf("false ");
        }
        break;
      case STRING:
        printf("%s ", expr_data->exprs[i].string);
        break;
      case OPERATOR:
        print_opE((u8)expr_data->exprs[i].op);
        break;
      default:
        error_g = EVAL_INTERNAL_ERROR;
        ERROR("[!] EVAL - print_exprDataS: type not supported = %d\n", expr_data->types[i]);
        break;
    }
  }
  printf("\n");
}

/* TRANSFORM */
dataU get_number(u8 *type, char **str){
  dataU result;
  s64 res_integer = 0;
  s64 neg_integer = 1;
  double res_double = 0.0;
  double neg_double = 1.0;
  if(**str == '-') {
    (*str)++;
    neg_integer = -1;
    neg_double = -1.0;
  }
  for(; isdigit(**str); (*str)++){
    res_integer = res_integer * 10 + (s64)(**str - '0');
    res_double = res_double * 10.0 + (double)(**str - '0');
  }
  if(**str == '.') {
    (*str)++;
    for(double j = 0.1; isdigit(**str); (*str)++, j*=0.1){
      res_double = res_double + j * (double)(**str - '0');
    }
    result.floating_point = res_double * neg_double;
    *type = FLOATING_POINT;
  }
  else {
    result.integer = res_integer * neg_integer;
    *type = INTEGER;
  }
  return result;
}

bool is_negative(char *expr){
  return ((*expr == '-') && isdigit(*(expr + 1)));
}

dataU get_var(sessionS *s, u8 *type, char **str){
  u8 type_var;
  variableDataU var_data;
  dataU result;
  u8 length = is_valid_varname(*str);
  if(length > 0) {
    type_var = get_variable_value(s, *str, &var_data);
    switch(type_var){
      case INTEGER:
        result.integer = var_data.integer;
        *type = INTEGER;
        break;
      case FLOATING_POINT:
        result.floating_point = var_data.floating_point;
        *type = FLOATING_POINT;
        break;
      case BOOLEAN:
        result.boolean = var_data.boolean;
        *type = BOOLEAN;
        break;
      case STRING:
        {
          size_t size = strlen(var_data.string);
          char *str = malloc(size + 1);
          memcpy(str, var_data.string, size);
          str[size] = '\0';
          result.string = str;
        }
        *type = STRING;
        break;
      default:
        error_g = EVAL_INTERNAL_ERROR;
        ERROR("[!] EVAL - get_var: data type not compatible\n");
    }
    *str = *str + length;
  }
  else{
    error_g = EVAL_INTERNAL_ERROR;
    ERROR("[!] EVAL - get_var: varname not correct\n");
  }
  return result;
}

void add_operator(opStackS *op_stack, exprDataS *expr_data, opE op){
  if(op == OP_NONE) {
    error_g = EVAL_INTERNAL_ERROR;
    return;
  }
  u8 peek_op = peek_opStackS(op_stack);
  dataU data;
  while((peek_op != OP_NONE) && (op_precedenceA[op] <= op_precedenceA[peek_op])){
    data.op = pop_opStackS(op_stack);
    push_exprDataS(expr_data, data, OPERATOR);
    peek_op = peek_opStackS(op_stack);
  }
  push_opStackS(op_stack, op);
}

bool get_operator(opE *op, char **expr) {
  switch(**expr){
    case '+':
      *op = OP_ADD;
      return 1;
    case '-':
      *op = OP_SUB;
      return 1;
    case '/':
      *op = OP_DIV;
      return 1;
    case '*':
      *op = OP_MUL;
      return 1;
    case '^':
      *op = OP_POW;
      return 1;
    case '%':
      *op = OP_MOD;
      return 1;
    case '&':
      if(*((*expr) + 1) == '&') {
        (*expr)++;
        *op = OP_AND;
        return 1;
      }
      *op = OP_NONE;
      return 0;
    case '|':
      if(*((*expr) + 1) == '|') {
        (*expr)++;
        *op = OP_OR;
        return 1;
      }
      *op = OP_NONE;
      return 0;
    case '!':
      if(*((*expr) + 1) == '=') {
        (*expr)++;
        *op = OP_NEQ;
      }
      else{
        *op = OP_NOT;
      }
      return 1;
    case '=':
      if(*((*expr) + 1) == '=') {
        (*expr)++;
        *op = OP_EQ;
        return 1;
      }
      *op = OP_NONE;
      return 0;
    case '>':
      if(*((*expr) + 1) == '=') {
        (*expr)++;
        *op = OP_GEQ;
      }
      else{
        *op = OP_GT;
      }
      return 1;
    case '<':
      if(*((*expr) + 1) == '=') {
        (*expr)++;
        *op = OP_LEQ;
      }
      else{
        *op = OP_LT;
      }
      return 1;
    default:
      *op = OP_NONE;
      return 0;
  }
}

void move_op_to_expr(opStackS *op_stack, exprDataS *expr_data){
  opE op = pop_opStackS(op_stack);
  dataU data;
  while((op != OP_LPARENT) && (op != OP_NONE)){
    data.op = op;
    push_exprDataS(expr_data, data, OPERATOR);
    op = pop_opStackS(op_stack);
  }
  if(op == OP_NONE){
    error_g = EVAL_INTERNAL_ERROR;
    ERROR("[!] EVAL - parentheses don't match\n");
  }
}

void add_remaining_op(opStackS *op_stack, exprDataS *expr_data) {
  dataU data;
  while(op_stack->stack_pointer > 0){
    data.op = pop_opStackS(op_stack);
    push_exprDataS(expr_data, data, OPERATOR);
  }
}

dataU get_str(char **expr, u8 *type) {
  (*expr)++;
  char *expr_str = *expr;
  dataU data;
  u8 i = 0;
  *type = STRING;
  while(expr_str[i] != '\"' && expr_str[i] != '\0'){
    i++;
  }
  if(expr_str[i] == '\0'){
    error_g = EVAL_INTERNAL_ERROR;
    data.string = NULL;
    ERROR("[!] EVAL - get_str: string does not end with quotes\n");
    return data;
  }
  char *str = malloc(i + 1);
  memcpy(str, expr_str, i+1);
  str[i] = '\0';
  data.string = str;
  *expr = *expr + i + 1;
  return data;
}


u8 eval_expr(sessionS *s, char **expr, variableDataU *result) {
  exprDataS expr_data = init_exprDataS();
  opStackS op_stack = init_opStackS();
  u8 type = NOT_DECLARED;
  dataU data;
  opE operator;
  bool possible_neg = 1;
  while(**expr != '\0' && **expr != ';' && **expr != ',') {
    /* number */
    if(isdigit(**expr) || (possible_neg && is_negative(*expr))) {
      data = get_number(&type, expr);
      push_exprDataS(&expr_data, data, type);
      possible_neg = 0;
    }
    /* variable */
    else if(isalpha(**expr)) {
      data = get_var(s, &type, expr);
      push_exprDataS(&expr_data, data, type);
      possible_neg = 0;
    }
    /* string */
    else if(**expr == '\"') {
      data = get_str(expr, &type);
      push_exprDataS(&expr_data, data, type);
      possible_neg = 0;
    }
    /* operstors */
    else if(**expr == '(') {
      push_opStackS(&op_stack, OP_LPARENT);
      possible_neg = 1;
      (*expr)++;
    }
    else if(**expr == ')') {
      move_op_to_expr(&op_stack, &expr_data);
      possible_neg = 0;
      (*expr)++;
    }
    else if(get_operator(&operator, expr)){
      add_operator(&op_stack, &expr_data, operator);
      possible_neg = 1;
      (*expr)++;
    }
    else if(**expr == ' ') {
      (*expr)++;
    }
    else{
      ERROR("[!] EVAL - transform: invalid character in expression = %c\n", **expr);
      error_g = EVAL_INTERNAL_ERROR;
    }
    if(error_g){
      return EVAL_ERROR;
    }
  }
  add_remaining_op(&op_stack, &expr_data);
  return pop_result(&expr_data, result);;
}