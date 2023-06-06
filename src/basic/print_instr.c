#include "interpreter.h"
#include "instructions.h"
#include "evaluator.h"
#include "mm.h"
#include "butils.h"
#include "printf.h"


char* print_instr_string(char* cmd);

/*
 * One PRINT instruction can print many things separated by comma
 */
void print_instr(void* env, char* cmd) {
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
    case TOK_NUMBER:
    case TOK_LPAREN:
    case TOK_FN:
      // eval and print
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
    print_instr(env, cmd);
  }
}

char* print_instr_string(char* cmd) {
  DEBUG("[*] print_instr_string(%s)\n", cmd);
  int i = 0;
  for (i=0; cmd[i] != '\0'; ++i) {
    if (cmd[i] == '"') {
      cmd[i] = '\0';
      printf(cmd);
      return &cmd[i+1];
    }
  }

  printf(cmd);
  return &cmd[i];
}