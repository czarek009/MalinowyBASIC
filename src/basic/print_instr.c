#include "interpreter.h"
#include "parser.h"
#include "instructions.h"
#include "session.h"
#include "evaluator.h"
#include "mm.h"
#include "butils.h"
#include "printf.h"


char* print_instr_string(char* cmd);
char* print_instr_var(sessionS* env, char* cmd, char* varname);
void print_instr_eval(variableDataU *eval_res, u8 eval_type);


/*
 * One PRINT instruction can print many things separated by comma
 */
sessionErrorCodeE print_instr(sessionS* env, char* cmd) {
  char buf[32] = {0};
  tokenE tok = TOK_NONE;

  tok = get_next_token(&cmd, buf, TOK_ANY);
  switch (tok) {
    case TOK_QUOTE:
      cmd = print_instr_string(cmd);
      break;
    case TOK_ARRAY_FLOAT:
    case TOK_ARRAY_STRING:
      cmd -= 1;
    case TOK_ARRAY_INT:
      cmd -= 1;
    case TOK_VAR:
    case TOK_NUMBER:
    case TOK_LPAREN:
    case TOK_FN:
      cmd -= strlen(buf);
      variableDataU eval_res;
      u8 eval_type = eval_expr(env, &cmd, &eval_res);
      if (eval_type >= 253) {
      ERROR("[INSTRICTOION ERROR] Expression evaluation error\n", 0);
      return SESSION_EVAL_ERROR;
      }
      print_instr_eval(&eval_res, eval_type);
      break;

    case TOK_NONE:
      // empty print
      break;
    case TOK_ERROR:
      return SESSION_PARSING_ERROR;

    default:
      // report invald token error
      ERROR("[INSTRUCTION ERROR] Invalid token in PRINT: %s\n", buf);
      return SESSION_PARSING_ERROR;
  }

  tok = get_next_token(&cmd, buf, TOK_ANY);
  if (tok == TOK_COMMA) {
    printf("\n");
  } else if (tok == TOK_SEMICOLON) {
    /* nothing */
    if (*cmd == '\0')
    return SESSION_NO_ERROR;
  } else if (tok == TOK_NONE || tok == TOK_ERROR) {
    /* end of instruction */
    printf("\n");
    return SESSION_NO_ERROR;
  } else {
    cmd -= strlen(buf);
  }
  print_instr(env, cmd);
  return SESSION_NO_ERROR;
}

char* print_instr_string(char* cmd) {
  DEBUG("[*] print_instr_string(%s)\n", cmd);
  int i = 0;
  for (i=0; cmd[i] != '\0'; ++i) {
    if (cmd[i] == '"') {
      cmd[i] = '\0';
      printf(cmd);
      cmd[i] = '"';
      return &cmd[i+1];
    }
  }

  printf(cmd);
  return &cmd[i];
}

char* print_instr_var(sessionS* env, char* cmd, char* varname) {
  DEBUG("[*] print_instr_var(%s)\n", cmd);
  variableDataU vardata = {0};
  u8 vartype = get_variable_value(env, varname, &vardata);

  switch (vartype) {
    case INTEGER:
      printf("%ld", vardata.integer);
      break;

    case STRING:
      printf(vardata.string);
      break;

    case FLOATING_POINT:
      printf("%f", vardata.floating_point);
      break;

    default:
      ERROR("[INSTRUCTION ERROR] Frobidden vartype in PRINT\n");
      break;
  }
  return cmd;
}

void print_instr_eval(variableDataU *eval_res, u8 eval_type) {
  switch (eval_type) {
    case INTEGER:
      printf("%ld", eval_res->integer);
      break;
    case FLOATING_POINT:
      printf("%f", eval_res->floating_point);
      break;
    case BOOLEAN:
      printf("%s", (eval_res->boolean ? "true" : "false"));
      break;
    case STRING:
      printf("%s", eval_res->string);
      free(eval_res->string);
      break;
    case EVAL_ERROR:
      ERROR("[INSTRUCTION ERROR] There was an error in EVAL\n");
      break;
    default:
      ERROR("[INSTRUCTION ERROR] Frobidden eval_type in PRINT\n");
      break;
  }
}