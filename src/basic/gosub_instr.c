#include "interpreter.h"
#include "parser.h"
#include "instructions.h"
#include "evaluator.h"
#include "session.h"
#include "mm.h"
#include "butils.h"


sessionErrorCodeE gosub_instr(sessionS* env, char* cmd, u64 ln) {
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

  u64 next_instr = get_next_instr_line(env, ln);
  push_return_address_to_stack(env, next_instr);
  set_jump_flag(env, jump_addr);

  return SESSION_NO_ERROR;
}

sessionErrorCodeE return_instr(sessionS* env, char* cmd) {
  u64 ln = pop_return_address_from_stack(env);
  if (ln == ~0) {
    return SESSION_EMPTY_STACK;
  }
  set_jump_flag(env, ln);

  return SESSION_NO_ERROR;
}