#include "interpreter.h"
#include "parser.h"
#include "instructions.h"
#include "evaluator.h"
#include "session.h"
#include "mm.h"
#include "butils.h"
#include "printf.h"
#include "io.h"


void gosub_instr(sessionS* env, char* cmd, u64 ln) {
  char buf[32] = {0};
  tokenE tok = TOK_NONE;

  tok = get_next_token(&cmd, buf, TOK_NUMBER);
  if (tok == TOK_ERROR) return; // PARSING ERROR

  s64 jump_addr = str2s64(buf);

  if (jump_addr < 1) {
    ERROR("GOSUB: line number must be > 0\n");
    return;
  }

  tok = get_next_token(&cmd, buf, TOK_NONE);
  if (tok == TOK_ERROR) return; // PARSING ERROR

  u64 next_instr = get_next_instr_line(env, ln);
  push_return_address_to_stack(env, next_instr);
  set_jump_flag(env, jump_addr);
}

void return_instr(sessionS* env, char* cmd) {
  u64 ln = pop_return_address_from_stack(env);
  set_jump_flag(env, ln);
}