#include "interpreter.h"
#include "instructions.h"
#include "evaluator.h"
#include "mm.h"
#include "butils.h"
#include "printf.h"
#include "io.h"


char* print_prompt(char* cmd);

void input_instr(void* env, char* cmd) {
  char buf[32];
  tokenE tok = TOK_NONE;
  cmd = consume_whitespaces(cmd);

  tok = get_next_token(cmd, buf);
  cmd += strlen(buf);

  if (tok == TOK_QUOTE) {
    cmd = print_prompt(cmd);
    cmd = consume_whitespaces(cmd);
    tok = get_next_token(cmd, buf);
    cmd += strlen(buf);
  }

  if (tok != TOK_VAR) {
    ERROR("[!] Invalid token in INPUT: %s\n", buf);
    return;
  }

  char input[256];
  readline(input, ":");

  DEBUG("[*] INPUT to var %s: %s\n", buf, input);
}

char* print_prompt(char* cmd) {
  DEBUG("[*] print_prompt(%s)\n", cmd);
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