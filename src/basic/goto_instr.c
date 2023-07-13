#include "interpreter.h"
#include "parser.h"
#include "instructions.h"
#include "evaluator.h"
#include "session.h"
#include "mm.h"
#include "butils.h"
#include "printf.h"
#include "io.h"


sessionErrorCodeE goto_instr(sessionS* env, char* cmd) {
  char buf[32] = {0};
  tokenE tok = TOK_NONE;

  tok = get_next_token(&cmd, buf, TOK_NUMBER);
  if (tok == TOK_ERROR) return SESSION_PARSING_ERROR; // PARSING ERROR

  s64 jump_addr = str2s64(buf);

  if (jump_addr < 1) {
    ERROR("[INSTRUCTION ERROR] Jump address must be > 0\n");
    return SESSION_INVALID_JUMP;
  }

  tok = get_next_token(&cmd, buf, TOK_NONE);
  if (tok == TOK_ERROR) return SESSION_PARSING_ERROR; // PARSING ERROR

  set_jump_flag(env, jump_addr);

  return SESSION_NO_ERROR;
}