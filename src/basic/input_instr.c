#include "interpreter.h"
#include "instructions.h"
#include "evaluator.h"
#include "session.h"
#include "mm.h"
#include "butils.h"
#include "printf.h"
#include "io.h"


char* print_prompt(char* cmd);

void input_instr(Session* env, char* cmd) {
  char buf[32] = {0};
  char varname[8] = {0};
  tokenE tok = TOK_NONE;
  bool isStr = false;
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
  strncpy(varname, buf, strlen(buf));

  tok = get_next_token(cmd, buf);
  if (tok == TOK_DOLAR) {
    cmd += strlen(buf);
    cmd = consume_whitespaces(cmd);
    isStr = true;
  }

  char input[256];
  readline(input, ":");

  DEBUG("[*] INPUT to var %s: %s\n", buf, input);

  if (isStr) {
    u64 len = strlen(input);
    char* vardata = malloc(len+1);
    strncpy(vardata, input, strlen(input));
    vardata[len] = '\0';
    add_string_variable(env, vardata, varname);
    return;
  }

  s64 value = str2s64(input);
  add_integer_variable(env, value, varname);
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