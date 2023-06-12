#include "interpreter.h"
#include "instructions.h"
#include "evaluator.h"
#include "session.h"
#include "mm.h"
#include "butils.h"
#include "printf.h"
#include "io.h"


void gosub_instr(Session* env, char* cmd, u64 ln) {
  char buf[32] = {0};
  tokenE tok = TOK_NONE;

  cmd = consume_whitespaces(cmd);
  tok = get_next_token(cmd, buf);
  cmd += strlen(buf);

  if (tok != TOK_NUMBER) {
    ERROR(" GOSUB: expected number, got '%s'\n", buf);
    return;
  }

  s64 jump_addr = str2s64(buf);

  if (jump_addr < 1) {
    ERROR("GOSUB: line number must be > 0\n");
    return;
  }

  u64 ni = get_next_instr_line(env, ln);
  push_return_address_to_stack(env, ni);
  set_jump_flag(env, jump_addr);
}

void return_instr(Session* env, char* cmd) {
  u64 ln = pop_return_address_from_stack(env);
  set_jump_flag(env, ln);
}