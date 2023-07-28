#include "interpreter.h"
#include "parser.h"
#include "instructions.h"
#include "evaluator.h"
#include "session.h"
#include "mm.h"
#include "butils.h"
#include "printf.h"
#include "io.h"


sessionErrorCodeE  def_instr(sessionS* env, char* cmd) {
  char funname[8] = {0};
  char argname[8] = {0};
  char buf[32] = {0};
  tokenE tok = TOK_NONE;

  /* funname */
  tok = get_next_token(&cmd, buf, TOK_FN);
  if (tok == TOK_ERROR) return SESSION_PARSING_ERROR; // PARSING ERROR
  strncpy(funname, buf, 8);

  /* ( */
  tok = get_next_token(&cmd, buf, TOK_LPAREN);
  if (tok == TOK_ERROR) return SESSION_PARSING_ERROR; // PARSING ERROR

  /* argname */
  tok = get_next_token(&cmd, buf, TOK_VAR);
  if (tok == TOK_ERROR) return SESSION_PARSING_ERROR; // PARSING ERROR
  strncpy(argname, buf, 8);

  /* ) */
  tok = get_next_token(&cmd, buf, TOK_RPAREN);
  if (tok == TOK_ERROR) return SESSION_PARSING_ERROR; // PARSING ERROR

  /* = */
  tok = get_next_token(&cmd, buf, TOK_EQ);
  if (tok == TOK_ERROR) return SESSION_PARSING_ERROR; // PARSING ERROR

  add_function(env, funname, argname, cmd);

  return SESSION_NO_ERROR;
}