#include "interpreter.h"
#include "parser.h"
#include "instructions.h"
#include "evaluator.h"
#include "session.h"
#include "mm.h"
#include "butils.h"
#include "printf.h"
#include "io.h"
#include "variable.h"


char* print_prompt(char* cmd);

sessionErrorCodeE input_instr(sessionS* env, char* cmd) {
  char varname[8] = {0};
  char buf[32] = {0};
  bool isStr = false;
  tokenE tok = TOK_NONE;

  tok = get_next_token(&cmd, buf, TOK_ANY);
  if (tok == TOK_ERROR) return SESSION_PARSING_ERROR; // PARSING ERROR

  if (tok == TOK_QUOTE) {
    /* prompt */
    cmd = print_prompt(cmd);
    tok = get_next_token(&cmd, buf, TOK_VAR);
    if (tok == TOK_ERROR) return SESSION_PARSING_ERROR; // PARSING ERROR
    strncpy(varname, buf, 8);
  } else if (tok == TOK_VAR) {
    /* no prompt */
    strncpy(varname, buf, 8);
  } else {
    ERROR("[INSTRUCTION ERROR] Invalid token in INPUT: %s\n", buf);
    return SESSION_PARSING_ERROR; // PARSING ERROR
  }

  tok = get_next_token(&cmd, buf, TOK_ANY);
  if (tok == TOK_DOLAR) {
    isStr = true;
  }

  char* input = malloc(256);
  char* aux = input;
  readline(input, ":");
  DEBUG("[DEBUG] INPUT to var %s: %s\n", buf, input);
  if (isStr) {
    /* string */
    u64 len = strlen(input);
    char* vardata = malloc(len+1);
    strncpy(vardata, input, strlen(input));
    vardata[len] = '\0';
    add_string_variable(env, vardata, varname);
  } else {
    /* number */
    variableDataU value;
    s8 value_type = eval_expr(env, &input, &value);
    if (value_type >= 253) {
      ERROR("[INSTRICTOION ERROR] Expression evaluation error\n", 0);
      return SESSION_EVAL_ERROR;
    }
    add_variable(env, value, varname, value_type);
  }

  free(aux);
  tok = get_next_token(&cmd, buf, TOK_NONE);
  if (tok == TOK_ERROR) return SESSION_PARSING_ERROR; // PARSING ERROR

  return SESSION_NO_ERROR;
}


char* print_prompt(char* cmd) {
  int i = 0;
  for (i=0; cmd[i] != '\0'; ++i) {
    if (cmd[i] == '"') {
      cmd[i] = '\0';
      printf(cmd);
      printf("\n");
      cmd[i] = '"';
      return &cmd[i+1];
    }
  }

  printf(cmd);
  printf("\n");
  return &cmd[i];
}