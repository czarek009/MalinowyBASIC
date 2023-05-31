#include "interpreter.h"
#include "instructions.h"
#include "evaluator.h"
#include "mm.h"
#include "butils.h"


void let_instr(void* env, char* cmd, u64 pos) {
  char buf[32];
  char varname[7];
  tokenE tok = TOK_NONE;
  pos = consume_whitespaces(cmd, pos);
  //s64 value = 0; // don't know type yet, should be union

  /* varname */
  tok = get_next_token(cmd, pos, buf);
  pos += strlen(buf);
  pos = consume_whitespaces(cmd, pos);
  if (tok != TOK_VAR) {
    ERROR("[!] Invalid token: %s\n", buf);
    return;
  }
  strncpy(varname, buf, 7);

  tok = get_next_token(cmd, pos, buf);
  pos += strlen(buf);
  pos = consume_whitespaces(cmd, pos);
  if (tok != TOK_EQ) {
    ERROR("[!] Invalid token: %s\n", buf);
    return;
  }

  tok = get_next_token(cmd, pos, buf);
  pos += strlen(buf);
  pos = consume_whitespaces(cmd, pos);
  if (tok != TOK_NUMBER && tok != TOK_LPAREN && tok != TOK_VAR && tok != TOK_FN) {
    ERROR("[!] Invalid token: %s\n", buf);
    return;
  }

  // value = eval_expr(env, cmd, pos);
  // add_variable(env, value, TYPE);
  DEBUG("[*] Add variable %s = %s\n", varname, buf);
}