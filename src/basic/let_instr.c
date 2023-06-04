#include "interpreter.h"
#include "instructions.h"
#include "evaluator.h"
#include "mm.h"
#include "butils.h"


void let_instr(void* env, char* cmd) {
  char buf[32];
  char varname[8];

  tokenE tok = TOK_NONE;
  cmd = consume_whitespaces(cmd);
  //s64 value = 0; // don't know type yet, should be union

  /* varname */
  tok = get_next_token(cmd, buf);
  if (tok != TOK_VAR) {
    ERROR("[!] Invalid token: %s\n", buf);
    return;
  }
  cmd += strlen(buf);
  cmd = consume_whitespaces(cmd);
  strncpy(varname, buf, 8);

  /* = */
  tok = get_next_token(cmd, buf);
  if (tok != TOK_EQ) {
    ERROR("[!] Invalid token: %s\n", buf);
    return;
  }
  cmd += strlen(buf);
  cmd = consume_whitespaces(cmd);

  /* value */
  tok = get_next_token(cmd, buf);
  if (tok != TOK_NUMBER && tok != TOK_LPAREN && tok != TOK_VAR && tok != TOK_FN) {
    ERROR("[!] Invalid token: %s\n", buf);
    return;
  }
  cmd += strlen(buf);
  cmd = consume_whitespaces(cmd);

  // value = eval_expr(env, cmd, pos);
  // add_variable(env, value, TYPE);
  DEBUG("[*] Add variable %s = %s\n", varname, buf);
  return;
}