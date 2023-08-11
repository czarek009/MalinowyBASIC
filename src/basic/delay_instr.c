#include "interpreter.h"
#include "parser.h"
#include "instructions.h"
#include "evaluator.h"
#include "session.h"
#include "mm.h"
#include "butils.h"
#include "printf.h"
#include "io.h"
#include "variable.h"
#include "timer.h"


sessionErrorCodeE delay_instr(sessionS* env, char* cmd) {
  variableDataU value;
  s8 value_type = eval_expr(env, &cmd, &value);
  if (value_type != INTEGER) {
    ERROR("[INSTRICTOION ERROR] Delay instruction requires INTEGER\n", 0);
    return SESSION_EVAL_ERROR;
  }

  delay_ms(value.integer);

  return SESSION_NO_ERROR;
}