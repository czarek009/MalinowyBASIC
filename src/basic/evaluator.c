#include "evaluator.h"
#include "types.h"
#include "butils.h"
#include "printf.h"
#include "mm.h"
#include "debug.h"
#include "parser.h"
#include "random.h"


#define MAX_DATA_NUMBER     32
#define MAX_OPERATOR_NUMBER (MAX_DATA_NUMBER - 1) / 2

typedef enum {
  BF_RND,
} builtinsE;

typedef enum {
  OP_ADD,          /* +  */
  OP_SUB,          /* -  */
  OP_DIV,          /* /  */
  OP_MULT,         /* *  */
  OP_POW,          /* ^  */
  OP_MOD,          /* %  */
  OP_LPAREN,       /* (  */
  OP_RPAREN,       /* )  */
  OP_AND,          /* && */
  OP_OR,           /* || */
  OP_NEG,          /* !  */
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


/* GLOBAL VARIABLES */
/*                       +  -  /  *  ^  %  (  ) && ||  ! == !=  >  <  >= <=  */
u8 op_precedenceA[17] = {3, 3, 4, 4, 5, 4, 0, 0, 2, 1, 4, 1, 1, 2, 2, 2, 2};


/* PRIVATE FUNCTIONS DECLARATIONS */
u64 find_closing_parenthesis(char* expr);
opStackS init_opStackS(void);
evalErrorE push_opStackS(opStackS *data, u8 op);
u8 pop_opStackS(opStackS *data);
u8 peek_opStackS(opStackS *data);
void print_opStackS(opStackS *data);
char* str_opE(opE op);
exprDataS init_exprDataS(void);
u8 pop_exprDataS(exprDataS *expr_data, dataU *data);
evalErrorE push_exprDataS(exprDataS *expr_data, dataU data, u8 type);
u8 pop_result(exprDataS *expr_data, variableDataU *data);
void free_exprDataS(exprDataS *expr_data);
void print_exprDataS(exprDataS *expr_data);
evalErrorE get_number(dataU *data, u8 *type, char *str);
evalErrorE get_hex_number(dataU *data, char *str);
evalErrorE get_var(sessionS *s, dataU *data, u8 *type, char *str);
evalErrorE get_str(char **expr, dataU *data);
evalErrorE get_array_data(sessionS *s, char **expr, char *varname, dataU *data, u8 arr_parsed_type);
evalErrorE add_operator(opStackS *op_stack, exprDataS *expr_data, opE op);
evalErrorE move_op_to_expr(opStackS *op_stack, exprDataS *expr_data);
evalErrorE add_remaining_op(opStackS *op_stack, exprDataS *expr_data);
evalErrorE apply_builtin_function(sessionS* s, char* name, char* args, dataU* data, u8* type);
u8 eval_bin(dataU first, dataU second, u8 first_type, u8 second_type, dataU *res, opE op);
u8 eval_int(s64 first, s64 second, dataU *res, opE op);
u8 eval_double(double first, double second, dataU *res, opE op);
u8 eval_string(char *first, char *second, dataU *res, opE op);
u8 eval_not(dataU data, u8 type, dataU *res);
double powfu(double x, u64 p);
double powfi(double x, s64 p);
double fact2(u8 n);
double exp2(double x);
double ln2(double x);
double powff(double x, double p);
bool is_valid_type(u8 type);


/* PUBLIC FUNCTIONS DEFINITIONS */
u8 eval_expr(sessionS *s, char **expr, variableDataU *result) {
  DEBUG("[DEBUG] eval_expr(%s)\n", *expr);

  evalErrorE ret_code = EVAL_SUCCESS;

  exprDataS expr_data = init_exprDataS();
  opStackS op_stack = init_opStackS();
  dataU data;
  u8 type = NOT_DECLARED;
  char buf[32] = {0};
  tokenE tok = TOK_NONE;
  tokenE expected_tok = TOK_ANY;

  do {
    tok = get_next_token(expr, buf, expected_tok);
    expected_tok = TOK_ANY;
    switch (tok) {
      case TOK_NUMBER:
        if(get_number(&data, &type, buf) == EVAL_INTERNAL_ERROR) {
          reverse_get_next_token(expr, buf);
          expected_tok = TOK_NOTNUMBER;
          break;
        }
        ret_code = push_exprDataS(&expr_data, data, type);
        expected_tok = TOK_NOTNUMBER;
        break;
      case TOK_HEX_NUMBER:
        if(get_hex_number(&data, buf) == EVAL_INTERNAL_ERROR) {
          reverse_get_next_token(expr, buf);
          expected_tok = TOK_NOTNUMBER;
          break;
        }
        ret_code = push_exprDataS(&expr_data, data, INTEGER);
        expected_tok = TOK_NOTNUMBER;
        break;
      case TOK_VAR:
        ret_code = get_var(s, &data, &type, buf);
        if(ret_code == EVAL_INTERNAL_ERROR) return EVAL_ERROR;
        ret_code = push_exprDataS(&expr_data, data, type);
        expected_tok = TOK_NOTNUMBER;
        break;
      case TOK_BUILTIN:
        {char funname[8] = {0};
        strncpy(funname, buf, strlen(buf));
        tok = get_next_token(expr, buf, TOK_LPAREN);
        if (tok == TOK_ERROR) return EVAL_ERROR;
        u64 pos = find_closing_parenthesis(*expr);
        char* args = malloc(pos+1);
        strncpy(args, *expr, pos);
        args[pos] = '\0';
        *expr += pos;
        ret_code = apply_builtin_function(s, funname, args, &data, &type);
        free(args);
        tok = get_next_token(expr, buf, TOK_RPAREN);
        if (tok == TOK_ERROR) return EVAL_ERROR;
        if(ret_code == EVAL_INTERNAL_ERROR) return EVAL_ERROR;
        ret_code = push_exprDataS(&expr_data, data, type);
        expected_tok = TOK_NOTNUMBER;}
        break;
      case TOK_FN:
        {
          char funname[8] = {0};
          strncpy(funname, buf, strlen(buf));
          tok = get_next_token(expr, buf, TOK_LPAREN);
          if (tok == TOK_ERROR) return EVAL_ERROR;
          u64 pos = find_closing_parenthesis(*expr);
          char* expr_tmp = malloc(pos+1);
          char* aux = expr_tmp;
          strncpy(expr_tmp, *expr, pos);
          expr_tmp[pos] = '\0';
          variableDataU argval;
          u8 argtype = eval_expr(s, &expr_tmp, &argval);
          free(aux);
          *expr += pos;
          tok = get_next_token(expr, buf, TOK_RPAREN);
          if (tok == TOK_ERROR) return EVAL_ERROR;
          variableDataU funval;
          type = apply_function(s, funname, argval, argtype, &funval);
          if (type == INTEGER) {
            data.integer = funval.integer;
          } else if (type == FLOATING_POINT) {
            data.floating_point = funval.floating_point;
          } else if (type == BOOLEAN) {
            data.boolean = funval.boolean;
          } else if (type == STRING) {
            data.string = funval.string;
          }
          ret_code = push_exprDataS(&expr_data, data, type);
          expected_tok = TOK_NOTNUMBER;
        }
        break;
      case TOK_ARRAY_INT:
        ret_code = get_array_data(s, expr, buf, &data, INTEGER);
        if(ret_code == EVAL_INTERNAL_ERROR) return EVAL_ERROR;
        ret_code = push_exprDataS(&expr_data, data, INTEGER);
        expected_tok = TOK_NOTNUMBER;
        break;
      case TOK_ARRAY_FLOAT:
        ret_code = get_array_data(s, expr, buf, &data, FLOATING_POINT);
        if(ret_code == EVAL_INTERNAL_ERROR) return EVAL_ERROR;
        ret_code = push_exprDataS(&expr_data, data, FLOATING_POINT);
        expected_tok = TOK_NOTNUMBER;
        break;
      case TOK_ARRAY_STRING:
        ret_code = get_array_data(s, expr, buf, &data, STRING);
        if(ret_code == EVAL_INTERNAL_ERROR) return EVAL_ERROR;
        ret_code = push_exprDataS(&expr_data, data, STRING);
        expected_tok = TOK_NOTNUMBER;
        break;
      case TOK_QUOTE:
        ret_code = get_str(expr, &data);
        if(ret_code == EVAL_INTERNAL_ERROR) return EVAL_ERROR;
        ret_code = push_exprDataS(&expr_data, data, STRING);
        expected_tok = TOK_NOTNUMBER;
        break;
      case TOK_LPAREN:
        ret_code = push_opStackS(&op_stack, OP_LPAREN);
        break;
      case TOK_RPAREN:
        ret_code = move_op_to_expr(&op_stack, &expr_data);
        expected_tok = TOK_NOTNUMBER;
        break;
      case TOK_ADD:
        ret_code = add_operator(&op_stack, &expr_data, OP_ADD);
        break;
      case TOK_SUB:
        ret_code = add_operator(&op_stack, &expr_data, OP_SUB);
        break;
      case TOK_DIV:
        ret_code = add_operator(&op_stack, &expr_data, OP_DIV);
        break;
      case TOK_MULT:
        ret_code = add_operator(&op_stack, &expr_data, OP_MULT);
        break;
      case TOK_MOD:
        ret_code = add_operator(&op_stack, &expr_data, OP_MOD);
        break;
      case TOK_POW:
        ret_code = add_operator(&op_stack, &expr_data, OP_POW);
        break;
      case TOK_AND:
        tok = get_next_token(expr, buf, TOK_AND);
        if(tok == TOK_ERROR) return EVAL_ERROR;
        ret_code = add_operator(&op_stack, &expr_data, OP_AND);
        break;
      case TOK_OR:
        tok = get_next_token(expr, buf, TOK_OR);
        if(tok == TOK_ERROR) return EVAL_ERROR;
        ret_code = add_operator(&op_stack, &expr_data, OP_OR);
        break;
      case TOK_NEG:
        tok = get_next_token(expr, buf, TOK_ANY);
        if(tok == TOK_EQ){
          ret_code = add_operator(&op_stack, &expr_data, OP_NEQ);
        }
        else{
          reverse_get_next_token(expr, buf);
          ret_code = add_operator(&op_stack, &expr_data, OP_NEG);
        }
        break;
      case TOK_LT:
        tok = get_next_token(expr, buf, TOK_ANY);
        if(tok == TOK_EQ){
          ret_code = add_operator(&op_stack, &expr_data, OP_LEQ);
        }
        else{
          reverse_get_next_token(expr, buf);
          ret_code = add_operator(&op_stack, &expr_data, OP_LT);
        }
        break;
      case TOK_GT:
        tok = get_next_token(expr, buf, TOK_ANY);
        if(tok == TOK_EQ){
          ret_code = add_operator(&op_stack, &expr_data, OP_GEQ);
        }
        else{
          reverse_get_next_token(expr, buf);
          ret_code = add_operator(&op_stack, &expr_data, OP_GT);
        }
        break;
      case TOK_EQ:
        tok = get_next_token(expr, buf, TOK_EQ);
        if(tok == TOK_ERROR) return EVAL_ERROR;
        ret_code = add_operator(&op_stack, &expr_data, OP_EQ);
        break;
      case TOK_ERROR:
        return EVAL_ERROR;
      default:
        reverse_get_next_token(expr, buf);
        tok = TOK_NONE;
        break;
    }
  } while (tok != TOK_NONE && ret_code == EVAL_SUCCESS);
  if(ret_code == EVAL_INTERNAL_ERROR) return EVAL_ERROR;
  ret_code = add_remaining_op(&op_stack, &expr_data);
  if(ret_code == EVAL_INTERNAL_ERROR) return EVAL_ERROR;
  return pop_result(&expr_data, result);
}

u64 *eval_array_sizes(sessionS *s, char** cmd, u8 dim_nr) {
  char buf[32] = {0};
  tokenE tok = TOK_COMMA;
  u64 *dims = malloc(dim_nr * sizeof(u64));
  for(u8 i = 0; (i < dim_nr) && (tok == TOK_COMMA); i++) {
    variableDataU value;
    u8 value_type = eval_expr(s, cmd, &value);
    if(value_type != INTEGER){
      free(dims);
      ERROR("[PARSER ERROR] Indexes/sizes of an array must be integers\n");
      return NULL;
    }
    dims[i] = value.integer;
    tok = get_next_token(cmd, buf, TOK_ANY);
  }
  if(tok == TOK_RSQUARE) return dims;
  if(tok == TOK_NONE) ERROR("[PARSER ERROR] There was no ] at the end of array indexes/sizes\n");
  else ERROR("[PARSER ERROR] Token '%s' is not allowed in array indexes/sizes\n", buf);
  free(dims);
  return NULL;
}


/* PRIVATE FUNCTIONS DEFINITIONS */
bool is_valid_type(u8 type) {
  return type < 250;
}

evalErrorE apply_builtin_function(sessionS* s, char* name, char* args, dataU* data, u8* type) {
  variableDataU argv[3] = {0};
  u8 argt[3] = {0};
  u8 argnum = 0;
  char* argsend = args + strlen(args);

  while (non_empty(args)) {
    argnum++;
    if (argnum > 3) {
      ERROR("[ERROR] Too many arguments\n");
      return EVAL_ERROR;
    }
    u64 pos = find_substring(",", args);
    args[pos] = '\0';
    
    argt[argnum-1] = eval_expr(s, &args, &argv[argnum-1]);
    if (!is_valid_type(argt[argnum-1])) {
      ERROR("[ERROR] Invalid function argument\n");
      return EVAL_ERROR;
    }
    if (args < argsend) {
      args++;
    }
  }

  if (!strncmp(name, "RND", 3)) {
    *type = FLOATING_POINT;
    u64 n = rand(0, 1000);
    data->floating_point = (double)n / 1000.0;
  }
  if (!strncmp(name, "INT", 3)) {
    if (argnum != 1) {
      ERROR("[ERROR] Builtin INT functions requires exactly one argument\n");
      return EVAL_ERROR;
    }
    *type = INTEGER;
    switch (argt[0]) {
      case INTEGER:
        data->integer = argv[0].integer;
        break;
      case FLOATING_POINT:
        data->integer = (s64)argv[0].floating_point;
        break;
      case BOOLEAN:
        data->integer = (s64)argv[0].boolean;
        break;
      
      default:
        ERROR("[ERROR] Cannot cast to int\n");
        return EVAL_ERROR;
        break;
    }
  }
  if (!strncmp(name, "FLOAT", 5)) {
    if (argnum != 1) {
      ERROR("[ERROR] Builtin FLOAT functions requires exactly one argument\n");
      return EVAL_ERROR;
    }
    *type = FLOATING_POINT;
    switch (argt[0]) {
      case INTEGER:
        data->floating_point = (double)argv[0].integer;
        break;
      case FLOATING_POINT:
        data->floating_point = argv[0].floating_point;
        break;
      case BOOLEAN:
        data->floating_point = (double)argv[0].boolean;
        break;
      
      default:
        ERROR("[ERROR] Cannot cast to float\n");
        return EVAL_ERROR;
        break;
    }
  }

  return EVAL_SUCCESS;
}

u64 find_closing_parenthesis(char* expr) {
  u64 idx = 0;
  int pnum = 1;

  while (pnum && *expr != '\n') {
    if (expr[idx] == ')') {
      --pnum;
      if (pnum == 0) {
        return idx;
      }
    } else if (expr[idx] == '(') {
      ++pnum;
    }
    ++idx;
  }

  ERROR("[EVALUATOR ERROR] Lack of closing parenthesis: '%s'\n", expr);
  return 0;
}

/* opStackS */
opStackS init_opStackS(void) {
  opStackS stack;
  stack.stack_pointer = 0;
  return stack;
}

evalErrorE push_opStackS(opStackS *data, u8 op) {
  if(data->stack_pointer < MAX_OPERATOR_NUMBER) {
    data->stack[data->stack_pointer] = op;
    data->stack_pointer++;
    return EVAL_SUCCESS;
  }
  ERROR("[!] EVAL - push_opStackS: stack is FULL\n");
  return EVAL_INTERNAL_ERROR;
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
    printf("%s ", str_opE(data->stack[i]));
  }
  printf("\n");
}

/* opE */
char* str_opE(opE op) {
  switch(op) {
    case OP_ADD:
      return "+";
    case OP_SUB:
      return "-";
    case OP_DIV:
      return "/";
    case OP_MULT:
      return "*";
    case OP_POW:
      return "^";
    case OP_MOD:
      return "%";
    case OP_LPAREN:
      return "(";
    case OP_RPAREN:
      return ")";
    case OP_AND:
      return "&";
    case OP_OR:
      return "|";
    case OP_NEG:
      return "!";
    case OP_EQ:
      return "==";
    case OP_NEQ:
      return "!=";
    case OP_GT:
      return ">";
    case OP_LT:
      return "<";
    case OP_GEQ:
      return ">=";
    case OP_LEQ:
      return "<=";
    default:
      ERROR("[!] EVAL - print_opE: there in no such an operator: %d\n", op);
      return "";
  }
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
  return NOT_FOUND;
}

evalErrorE push_exprDataS(exprDataS *expr_data, dataU data, u8 type) {
  if(type == OPERATOR) {
    dataU second;
    opE op = data.op;
    if(op == OP_LPAREN){
      ERROR("[!] EVAL - parentheses don't match\n");
      return EVAL_INTERNAL_ERROR;
    }
    u8 second_type = pop_exprDataS(expr_data, &second);

    if(second_type == NOT_FOUND || second_type == OPERATOR){
      ERROR("[!] EVAL - expression is incorrect: operator %s is missing components\n", str_opE(op));
      return EVAL_INTERNAL_ERROR;
    }

    if(op == OP_NEG) {
      type = eval_not(second, second_type, &data);
    }
    else {
      dataU first;
      u8 first_type = pop_exprDataS(expr_data, &first);
      if(first_type == NOT_FOUND || first_type == OPERATOR){
        ERROR("[!] EVAL - expression is incorrect: operator %s is missing second component\n", str_opE(op));
        return EVAL_INTERNAL_ERROR;
      }
      type = eval_bin(first, second, first_type, second_type, &data, op);
    }
  }

  if(type == EVAL_ERROR){
    return EVAL_INTERNAL_ERROR;
  }
  if (expr_data->expr_number < MAX_DATA_NUMBER) {
    expr_data->exprs[expr_data->expr_number] = data;
    expr_data->types[expr_data->expr_number] = type;
    expr_data->expr_number++;
    return EVAL_SUCCESS;
  }
  ERROR("[!] EVAL - push_exprDataS: expr_data is FULL\n");
  return EVAL_ERROR;
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
  ERROR("[EVALUATOR ERROR] Expression is not correct\n");
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
  printf("expr_data\n");
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
        printf("%s ", str_opE(expr_data->exprs[i].op));
        break;
      default:
        ERROR("[!] EVAL - print_exprDataS: type not supported = %d\n", expr_data->types[i]);
        break;
    }
  }
  printf("\n");
}

/* TRANSFORM */
evalErrorE get_number(dataU *data, u8 *type, char *str) {
  s64 res_integer = 0;
  s64 neg_integer = 1;
  double res_double = 0.0;
  double neg_double = 1.0;
  u8 i = 0;
  if(str[0] == '-') {
    neg_integer = -1;
    neg_double = -1.0;
    i = 1;
  }
  for(; isdigit(str[i]); i++){
    res_integer = res_integer * 10 + (s64)(str[i] - '0');
    res_double = res_double * 10.0 + (double)(str[i] - '0');
  }
  if(neg_integer == -1 && i == 1) return EVAL_INTERNAL_ERROR;
  if(str[i] == '.') {
    i++;
    for(double j = 0.1; isdigit(str[i]); i++, j*=0.1){
      res_double = res_double + j * (double)(str[i] - '0');
    }
    data->floating_point = res_double * neg_double;
    *type = FLOATING_POINT;
  }
  else {
    data->integer = res_integer * neg_integer;
    *type = INTEGER;
  }
  return EVAL_SUCCESS;
}

evalErrorE get_hex_number(dataU *data, char *str) {
  s64 res = 0;
  u64 p = strlen(str) - 3;
  u8 i = 2;
  for(; str[i] != '\0'; i++, p--){
    if(isdigit(str[i])) {
      res += powfu(16, p) * (s64)(str[i] - '0');
    }
    else if(str[i] >= 'A' && str[i] <= 'F') {
      res += powfu(16, p) * (s64)(str[i] - 55);
    }
    else if(str[i] >= 'a' && str[i] <= 'f') {
      res += powfu(16, p) * (s64)(str[i] - 87);
    }
    else {
      ERROR("[EVAL ERROR] Error while evaluating hex number: %s", str);
      return EVAL_INTERNAL_ERROR;
    }
  }
  data->integer = res;
  return EVAL_SUCCESS;
}

evalErrorE get_var(sessionS *s, dataU *data, u8 *type, char *str) {
  u8 type_var;
  variableDataU var_data;
  type_var = get_variable_value(s, str, &var_data);
  switch(type_var){
    case INTEGER:
      data->integer = var_data.integer;
      *type = INTEGER;
      break;
    case FLOATING_POINT:
      data->floating_point = var_data.floating_point;
      *type = FLOATING_POINT;
      break;
    case BOOLEAN:
      data->boolean = var_data.boolean;
      *type = BOOLEAN;
      break;
    case STRING:
      {
        size_t size = strlen(var_data.string);
        char *str = malloc(size + 1);
        memcpy(str, var_data.string, size);
        str[size] = '\0';
        data->string = str;
      }
      *type = STRING;
      break;
    case NOT_FOUND:
      return EVAL_INTERNAL_ERROR;
    default:
      ERROR("[!] EVAL - get_var: data type not compatible\n");
      return EVAL_INTERNAL_ERROR;
  }
  return EVAL_SUCCESS;
}

evalErrorE get_str(char **expr, dataU *data) {
  char *expr_str = *expr;
  u8 i = 0;
  while(expr_str[i] != '\"' && expr_str[i] != '\0'){
    i++;
  }
  if(expr_str[i] == '\0'){
    data->string = NULL;
    ERROR("[!] EVAL - get_str: string does not end with quotes\n");
    return EVAL_INTERNAL_ERROR;
  }
  char *str = malloc(i + 1);
  memcpy(str, expr_str, i+1);
  str[i] = '\0';
  data->string = str;
  *expr = *expr + i + 1;
  return EVAL_SUCCESS;
}

evalErrorE get_array_data(sessionS *s, char **expr, char *varname, dataU *data, u8 parsed_type) {
  u8 dim_nr = 0;
  sessionErrorCodeE array_err = SESSION_NO_ERROR;
  array_err = parse_array_dim_nr(*expr, &dim_nr);
  if(array_err != SESSION_NO_ERROR) return EVAL_INTERNAL_ERROR;
  u64 *idxs = eval_array_sizes(s, expr, dim_nr);
  if(idxs == NULL) return SESSION_PARSING_ERROR;
  array_err = check_array_parameters(s, varname, parsed_type, dim_nr, idxs);
  if(array_err != SESSION_NO_ERROR) {
    free(idxs);
    return EVAL_INTERNAL_ERROR;
  }
  variableDataU arr_data = {0};
  array_err = get_array_element(s, varname, idxs, &arr_data);
  free(idxs);
  if(array_err != SESSION_NO_ERROR) return EVAL_INTERNAL_ERROR;
  switch (parsed_type) {
    case INTEGER:
      data->integer = arr_data.integer;
      return EVAL_SUCCESS;
    case FLOATING_POINT:
      data->floating_point = arr_data.floating_point;
      return EVAL_SUCCESS;
    case STRING:
      data->string = arr_data.string;
      return EVAL_SUCCESS;
    default:
      return EVAL_INTERNAL_ERROR;
  }
}

evalErrorE add_operator(opStackS *op_stack, exprDataS *expr_data, opE op) {
  u8 peek_op = peek_opStackS(op_stack);
  dataU data;
  evalErrorE ret = EVAL_SUCCESS;
  while((peek_op != OP_NONE) && (op_precedenceA[op] <= op_precedenceA[peek_op]) && (ret == EVAL_SUCCESS)){
    data.op = pop_opStackS(op_stack);
    ret = push_exprDataS(expr_data, data, OPERATOR);
    peek_op = peek_opStackS(op_stack);
  }
  if(ret == EVAL_INTERNAL_ERROR) return EVAL_INTERNAL_ERROR;
  return push_opStackS(op_stack, op);
}

evalErrorE move_op_to_expr(opStackS *op_stack, exprDataS *expr_data) {
  opE op = pop_opStackS(op_stack);
  dataU data;
  evalErrorE ret = EVAL_SUCCESS;
  while((op != OP_LPAREN) && (op != OP_NONE) && (ret == EVAL_SUCCESS)){
    data.op = op;
    ret = push_exprDataS(expr_data, data, OPERATOR);
    op = pop_opStackS(op_stack);
  }
  if(op == OP_NONE){
    ERROR("[!] EVAL - parentheses don't match\n");
    return EVAL_INTERNAL_ERROR;
  }
  return ret;
}

evalErrorE add_remaining_op(opStackS *op_stack, exprDataS *expr_data) {
  dataU data;
  evalErrorE ret = EVAL_SUCCESS;
  while(op_stack->stack_pointer > 0 && ret == EVAL_SUCCESS){
    data.op = pop_opStackS(op_stack);
    ret = push_exprDataS(expr_data, data, OPERATOR);
  }
  return ret;
}

/* EVAL */
u8 eval_bin(dataU first, dataU second, u8 first_type, u8 second_type, dataU *res, opE op) {
  if(first_type == STRING) {
    switch(second_type) {
      case STRING:
        return eval_string(first.string, second.string, res, op);
      default:
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
        ERROR("[!] EVAL - eval_bin: operation between boolean and type that dont match\n");
        return EVAL_ERROR;
    }
  }
  else {
    ERROR("[!] EVAL - eval_bin: type of operand is invalid\n");
    return EVAL_ERROR;
  }
}

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
        return EVAL_ERROR;
      }
      res->integer = first / second;
      return INTEGER;
    case OP_MULT:
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
      ERROR("[!] EVAL - eval_int: unknown operator\n");
      return EVAL_ERROR;
  }
}

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
        return EVAL_ERROR;
      }
      res->floating_point = first / second;
      return FLOATING_POINT;
    case OP_MULT:
      res->floating_point = first * second;
      return FLOATING_POINT;
    case OP_POW:
      res->floating_point = powff(first, second);
      return FLOATING_POINT;
    case OP_MOD:
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
      ERROR("[!] EVAL - eval_not: unknown operator\n");
      return EVAL_ERROR;
  }
  return BOOLEAN;
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