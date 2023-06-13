#include "interpreter.h"
#include "instructions.h"
#include "session.h"
#include "evaluator.h"
#include "mm.h"
#include "butils.h"
#include "printf.h"


char* print_instr_string(char* cmd);
char* print_instr_var(Session* env, char* cmd, char* varname);

/*
 * One PRINT instruction can print many things separated by comma
 */
void print_instr(Session* env, char* cmd) {
  char buf[32] = {0};
  tokenE tok = TOK_NONE;
  cmd = consume_whitespaces(cmd);

  tok = get_next_token(cmd, buf);
  cmd += strlen(buf);

  switch (tok) {
    case TOK_QUOTE:
      cmd = print_instr_string(cmd);
      break;

    case TOK_VAR:
      cmd = print_instr_var(env, cmd, buf);
    case TOK_NUMBER:
    case TOK_LPAREN:
    case TOK_FN:
      // eval and print
      break;

    case TOK_NONE:
    case TOK_ERROR:
      // empty print
      break;

    default:
      // report invald token error
      ERROR("[!] Invalid token in PRINT: %s\n", buf);
      break;
  }

  cmd = consume_whitespaces(cmd);
  tok = get_next_token(cmd, buf);
  if (tok == TOK_COMMA) {
    cmd += strlen(buf);
    cmd = consume_whitespaces(cmd);
    printf("\n");
  }
  if (tok == TOK_SEMICOLON) {
    cmd += strlen(buf);
    cmd = consume_whitespaces(cmd);
  }
  if (tok == TOK_NONE || tok == TOK_ERROR) {
    printf("\n");
    return;
  }
  print_instr(env, cmd);
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

char* print_instr_var(Session* env, char* cmd, char* varname) {
  DEBUG("[*] print_instr_var(%s)\n", cmd);
  VariableData vardata = {0};
  u8 vartype = get_variable_value(env, varname, &vardata);

  switch (vartype) {
    case INTEGER:
      printf("%ld", vardata.integer);
      break;

    case STRING:
      printf(vardata.string);
      break;
    
    default:
      ERROR("[!] Frobidden vartype in PRINT!\n");
      break;
  }
  return cmd;
}