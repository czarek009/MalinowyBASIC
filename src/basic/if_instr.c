#include "instructions.h"
#include "interpreter.h"
#include "parser.h"
#include "evaluator.h"
#include "session.h"
#include "debug.h"
#include "variable.h"
#include "butils.h"
#include "mm.h"
#include "printf.h"

/*

x, y:
- single instruction -> execute
- number -> branch to number


1. IF ... THEN x
2. IF ... THEN x ELSE y
3. IF ... THEN x
   ELSE y


*/

sessionErrorCodeE set_jump(sessionS* env, s64 jump_addr);
sessionErrorCodeE bool_of_variable(variableDataU *data, bool *res, u8 type);

sessionErrorCodeE if_instr(sessionS* env, char* cmd, u64 ln) {

  /* get condition value */
  variableDataU res;
  u8 type = eval_expr(env, &cmd, &res);
  if (type >= 253) {
    ERROR("[INSTRUCTOION ERROR] Expression evaluation error\n", 0);
    return SESSION_EVAL_ERROR;
  }
  bool condition = false;
  sessionErrorCodeE cast_err = bool_of_variable(&res, &condition, type);
  if (cast_err != SESSION_NO_ERROR) return SESSION_UNKNOWN_ERROR;

  char buf[32] = {0};
  tokenE tok = TOK_ANY;



  /* THEN */
  if (condition) {
    tok = get_next_token(&cmd, buf, TOK_THEN);
    if (tok == TOK_ERROR) return SESSION_PARSING_ERROR;

    tok = get_next_token(&cmd, buf, TOK_ANY);
    if (tok == TOK_ERROR) return SESSION_PARSING_ERROR;

    if (tok == TOK_NUMBER) {
      s64 jump_addr = str2s64(buf);
      return set_jump(env, jump_addr);
    }

    reverse_get_next_token(&cmd, buf);

    char *else_keyword = cmd;
    char *then_body = cmd;

    /* check for else keyword in instruction */
    while(tok != TOK_ELSE && tok != TOK_NONE){
      tok = get_next_token(&else_keyword, buf, TOK_ANY);
      if (tok == TOK_ERROR) return SESSION_PARSING_ERROR;
    }
    if (tok == TOK_ELSE) {
      reverse_get_next_token(&else_keyword, buf);
      size_t length = else_keyword - cmd;
      then_body = malloc(length + 1);
      memcpy(then_body, cmd, length);
      then_body[length] = '\0';
    }

    sessionErrorCodeE exec_err = interpreter_execute_command(env, then_body, ln);
    if (tok == TOK_ELSE) free(then_body);
    if (exec_err != SESSION_NO_ERROR) return exec_err;

    instructionS *next_instr = get_next_instruction(env, ln);

    /* no next instr */
    if (next_instr == NULL) return SESSION_NO_ERROR;

    /* check for else keyword in next instruction */
    char *next_cmd = next_instr->instruction;
    tok = get_next_token(&next_cmd, buf, TOK_ANY);
    if (tok != TOK_ELSE) return SESSION_NO_ERROR;

    /* set jump to skip else instruction */
    s64 jump_addr = next_instr->line_number + 1;
    next_instr = next_instr->next;
    if(next_instr != NULL) jump_addr = next_instr->line_number;
    return set_jump(env, jump_addr);
  }

  /* ELSE */
  else {
    /* check for else keyword in instruction */
    while(tok != TOK_ELSE && tok != TOK_NONE){
      tok = get_next_token(&cmd, buf, TOK_ANY);
      if (tok == TOK_ERROR) return SESSION_PARSING_ERROR;
    }
    if (tok == TOK_ELSE) {
      tok = get_next_token(&cmd, buf, TOK_ANY);
      if (tok == TOK_ERROR) return SESSION_PARSING_ERROR;
      if (tok == TOK_NUMBER) {
        s64 jump_addr = str2s64(buf);
        return set_jump(env, jump_addr);
      }
      reverse_get_next_token(&cmd, buf);
      return interpreter_execute_command(env, cmd, ln);
    }

    instructionS *next_instr = get_next_instruction(env, ln);
    /* no next instr */
    if (next_instr == NULL) return SESSION_NO_ERROR;

    /* check for else keyword in next instruction */
    char *next_cmd = next_instr->instruction;
    tok = get_next_token(&next_cmd, buf, TOK_ANY);
    if (tok != TOK_ELSE) return SESSION_NO_ERROR;

    /* execute instruction */
    tok = get_next_token(&next_cmd, buf, TOK_ANY);
    if (tok == TOK_ERROR) return SESSION_PARSING_ERROR;
    if (tok == TOK_NUMBER) {
      s64 jump_addr = str2s64(buf);
      return set_jump(env, jump_addr);
    }

    reverse_get_next_token(&next_cmd, buf);
    sessionErrorCodeE exec_err = interpreter_execute_command(env, next_cmd, next_instr->line_number);
    if (exec_err != SESSION_NO_ERROR) return exec_err;

    s64 jump_addr = next_instr->line_number + 1;
    next_instr = next_instr->next;
    if(next_instr != NULL) jump_addr = next_instr->line_number;
    return set_jump(env, jump_addr);
  }
}

sessionErrorCodeE set_jump(sessionS* env, s64 jump_addr) {
  if (jump_addr < 1) {
    ERROR("[INSTRUCTION ERROR] Jump address must be > 0\n");
    return SESSION_INVALID_JUMP;
  }

  set_jump_flag(env, jump_addr);

  return SESSION_NO_ERROR;
}

sessionErrorCodeE bool_of_variable(variableDataU *data, bool *condition, u8 type) {
  switch (type) {
    case INTEGER:
      *condition = (bool) data->integer;
      return SESSION_NO_ERROR;
    case FLOATING_POINT:
      *condition = (bool) data->floating_point;
      return SESSION_NO_ERROR;
    case BOOLEAN:
      *condition = data->boolean;
      return SESSION_NO_ERROR;
    case STRING:
      *condition = (bool) data->string;
      return SESSION_NO_ERROR;
    default:
      ERROR("[IF INSTRUCTOION ERROR] Trying to cast forbidden type to bool\n", 0);
      return SESSION_UNKNOWN_ERROR;
  }
}