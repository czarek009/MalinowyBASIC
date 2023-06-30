#include "interpreter.h"
#include "instructions.h"
#include "evaluator.h"
#include "session.h"
#include "mm.h"
#include "butils.h"
#include "printf.h"
#include "io.h"


void goto_instr(sessionS* env, char* cmd) {
  char buf[32] = {0};
  tokenE tok = TOK_NONE;

  cmd = consume_whitespaces(cmd);
  tok = get_next_token(cmd, buf);
  cmd += strlen(buf);

  if (tok != TOK_NUMBER) {
    ERROR(" GOTO: expected number, got '%s'\n", buf);
    return;
  }

  s64 ln = str2s64(buf);

  if (ln < 1) {
    ERROR("GOTO: line number must be > 0\n");
    return;
  }

  set_jump_flag(env, ln);
}