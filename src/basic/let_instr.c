#include "instructions.h"
#include "interpreter.h"
#include "parser.h"
#include "evaluator.h"
#include "session.h"
#include "butils.h"
#include "mm.h"


sessionErrorCodeE let_instr(sessionS* env, char* cmd) {
  char varname[8] = {0};
  char buf[32] = {0};
  bool isStr = false;
  tokenE tok = TOK_NONE;

  /* varname */
  tok = get_next_token(&cmd, buf, TOK_VAR); // copy straight to varname instead buf?
  if (tok == TOK_ERROR) return SESSION_PARSING_ERROR; // PARSING ERROR
  strncpy(varname, buf, 8);

  /* $ = */
  tok = get_next_token(&cmd, buf, TOK_ANY);
  if (tok == TOK_DOLAR) {
    isStr = true;
    tok = get_next_token(&cmd, buf, TOK_EQ);
    if (tok == TOK_ERROR) return SESSION_PARSING_ERROR; // PARSING ERROR
  } else if (tok != TOK_EQ) {
    return SESSION_PARSING_ERROR; // PARSING ERROR
  }

  /* value */
  if (isStr) {
    /* string */ 
    tok = get_next_token(&cmd, buf, TOK_QUOTE);
    if (tok == TOK_ERROR) return SESSION_PARSING_ERROR; // PARSING ERROR
    get_const_str(&cmd, buf);
    add_string_variable(env, buf, varname);
  } else {
    /* number */
    tok = get_next_token(&cmd, buf, TOK_ANY);
    if (tok != TOK_NUMBER && tok != TOK_LPAREN && tok != TOK_VAR && tok != TOK_FN) {
      return SESSION_PARSING_ERROR; // PARSING ERROR
    }
    cmd -= strlen(buf);
    variableDataU value;
    s8 value_type = eval_expr(env, &cmd, &value);
    DEBUG(" value_type: %u\n", (u32)value_type);
    DEBUG(" value: %ld\n", value.integer);
    add_variable(env, value, varname, value_type);
  }

  tok = get_next_token(&cmd, buf, TOK_NONE);
  if (tok == TOK_ERROR) return SESSION_PARSING_ERROR; // PARSING ERROR

  return SESSION_NO_ERROR; // SUCCESS
}