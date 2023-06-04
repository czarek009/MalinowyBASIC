#include "interpreter.h"
#include "instructions.h"
#include "evaluator.h"
#include "mm.h"
#include "butils.h"


void let_instr(void* env, char* cmd) {
  char buf[32];
  char varname[8];
  bool isStr = false;

  tokenE tok = TOK_NONE;
  cmd = consume_whitespaces(cmd);

  /* varname */
  tok = get_next_token(cmd, buf);
  if (tok != TOK_VAR) {
    ERROR("[!] Invalid token: %s\n", buf);
    return;
  }
  cmd += strlen(buf);
  cmd = consume_whitespaces(cmd);
  strncpy(varname, buf, 8);

  /* $ */
  tok = get_next_token(cmd, buf);
  if (tok == TOK_DOLAR) {
    cmd += strlen(buf);
    cmd = consume_whitespaces(cmd);
    isStr = true;
  }

  /* = */
  tok = get_next_token(cmd, buf);
  if (tok != TOK_EQ) {
    ERROR("[!] Invalid token: %s\n", buf);
    return;
  }
  cmd += strlen(buf);
  cmd = consume_whitespaces(cmd);

  /* value */
  /* string */
  tok = get_next_token(cmd, buf);
  if (isStr) {
    if (tok != TOK_QUOTE) {
      ERROR("[!] Expected string: %s\n", buf);
      return;
    }
    ++cmd;
    u64 len = get_str_len(cmd);
    strncpy(buf, cmd, len);
    buf[len] = '\0';
    cmd += len+2;

    DEBUG("[*] Add string variable %s = %s\n", varname, buf);
    return;
  }


  if (tok != TOK_NUMBER && tok != TOK_LPAREN && tok != TOK_VAR && tok != TOK_FN) {
    ERROR("[!] Invalid token: %s\n", buf);
    return;
  }
  if (tok != TOK_NUMBER) {
    ERROR("[!] LET supports only numbers\n");
    return;
  }
  s64 value = str2s64(cmd);
  cmd += strlen(buf);
  cmd = consume_whitespaces(cmd);

  // add_variable(env, value, TYPE);
  DEBUG("[*] Add variable %s = %ld\n", varname, value);
  return;
}