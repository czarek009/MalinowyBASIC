#include "interpreter.h"
#include "parser.h"
#include "instructions.h"
#include "evaluator.h"
#include "session.h"
#include "mm.h"
#include "butils.h"
#include "hdmi.h"


sessionErrorCodeE font_instr(sessionS* env, char* cmd) {
  variableDataU value;
  u8 type = eval_expr(env, &cmd, &value);
  if (type != INTEGER) {
    ERROR("[INSTRUCTION ERROR] Color must be of type INTEGER\n", 0);
    return SESSION_PARSING_ERROR;
  }
  char buf[32] = {0};
  tokenE tok = TOK_NONE;
  tok = get_next_token(&cmd, buf, TOK_NONE);
  if (tok == TOK_ERROR) return SESSION_PARSING_ERROR; // PARSING ERROR
  // hdmi_change_font_color(value.integer);
  return SESSION_NO_ERROR;
}

sessionErrorCodeE background_instr(sessionS* env, char* cmd) {
  variableDataU value;
  u8 type = eval_expr(env, &cmd, &value);
  if (type != INTEGER) {
    ERROR("[INSTRUCTION ERROR] Color must be of type INTEGER\n", 0);
    return SESSION_PARSING_ERROR;
  }
  char buf[32] = {0};
  tokenE tok = TOK_NONE;
  tok = get_next_token(&cmd, buf, TOK_NONE);
  if (tok == TOK_ERROR) return SESSION_PARSING_ERROR; // PARSING ERROR
  // hdmi_change_bg_color(value.integer);
  return SESSION_NO_ERROR;
}