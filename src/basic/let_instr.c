#include "interpreter.h"
#include "instructions.h"
#include "evaluator.h"


void let_instr(void* env, char* cmd, u64* pos) {
  char var_name[7] = {0};
  tokenE tok = TOK_NONE;
  //s64 value = 0; // don't know type yet, should be union

  /* varname */
  tok = get_next_token(cmd, pos);
  if (tok != TOK_VAR) {
    // report error
  }
  get_variable_name(var_name, cmd, pos);

  tok = get_next_token(cmd, pos);
  if (tok != TOK_EQ) {
    // report error
  }

  tok = get_next_token(cmd, pos);
  if (tok != TOK_NUMBER && tok != TOK_LPAREN && tok != TOK_VAR && tok != TOK_FN) {
    // report error
  }

  // value = eval_expr(env, cmd, pos);

  // add_variable(env, value, TYPE);
}