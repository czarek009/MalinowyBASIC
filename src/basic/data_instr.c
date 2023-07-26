#include "instructions.h"
#include "interpreter.h"
#include "parser.h"
#include "evaluator.h"
#include "session.h"
#include "butils.h"
#include "mm.h"


sessionErrorCodeE data_instr(sessionS* env, char* cmd) {
  u64 len = strlen(cmd);
  u64 sep = find_substring(",", cmd);
  cmd[sep] = '\0';
  variableDataU value;
  u8 type = eval_expr(env, &cmd, &value);

  if (type > 250) {
    ERROR("[INSTRUCTION ERROR] Cannot evaluate expression\n");
    return SESSION_EVAL_ERROR;
  }

  dataQueueS data = {.type = type, .value = value};

  push_data_to_queue(env, data);

  if (len != sep) {
    cmd++;
    return data_instr(env, cmd);
  }
  return SESSION_NO_ERROR;
}

sessionErrorCodeE read_instr(sessionS* env, char* cmd) {
  char varname[8] = {0};
  char buf[32] = {0};
  // bool isStr = false;
  tokenE tok = TOK_NONE;

  tok = get_next_token(&cmd, buf, TOK_VAR);
  if (tok == TOK_ERROR) return SESSION_PARSING_ERROR;
  strncpy(varname, buf, 8);

  dataQueueS* data = read_data_from_queue(env);

  add_variable(env, data->value, varname, data->type);

  tok = get_next_token(&cmd, buf, TOK_ANY);
  if (tok == TOK_ERROR) return SESSION_PARSING_ERROR;

  if (tok == TOK_COMMA) {
    return read_instr(env, cmd);
  }

  return SESSION_NO_ERROR;
}

sessionErrorCodeE restore_instr(sessionS* env, char* cmd) {
  env->metadata.data_queue_start = 0;
  return SESSION_NO_ERROR;
}