#include "instructions.h"
#include "interpreter.h"
#include "parser.h"
#include "evaluator.h"
#include "session.h"
#include "butils.h"
#include "mm.h"


sessionErrorCodeE for_instr(sessionS* env, char* cmd, u64 ln) {
  char varname[8] = {0};
  char buf[32] = {0};
  bool first_entrence = true;
  tokenE tok = TOK_NONE;
  forS* meta = NULL;
  u64 sp = env->metadata.for_stackpointer;

  if (sp > 0 && env->for_stack[sp-1].line == ln) {
    first_entrence = false;
    meta = &(env->for_stack[sp-1]);
  }

  if (!first_entrence) {
    if (meta == NULL) {
      return SESSION_UNKNOWN_ERROR;
    }
    variableDataU iter_val;
    (void)get_variable_value(env, env->for_stack[sp-1].iterator, &iter_val);
    u64 iterator = iter_val.integer;
    add_integer_variable(env, iterator+meta->step, meta->iterator);
    if (iterator >= meta->limit) {
      DEBUG("[DEBUG] FOR INSTRUCTION END\n", 0);
      env->metadata.for_stackpointer -= 1;
      set_jump_flag(env, meta->next_line);
    }
    return SESSION_NO_ERROR;
  }

  /* first time here, need to parse */
  /* varname */
  tok = get_next_token(&cmd, buf, TOK_VAR);
  if (tok == TOK_ERROR) return SESSION_PARSING_ERROR; // PARSING ERROR
  strncpy(varname, buf, 8);

  tok = get_next_token(&cmd, buf, TOK_EQ);
  if (tok == TOK_ERROR) return SESSION_PARSING_ERROR; // PARSING ERROR

  /* iterator initial value */
  u64 expr_end = find_substring("TO", cmd);
  if (expr_end == strlen(cmd)) {
    ERROR("[INSTRUCTION ERROR] Cannot find expression end: %s\n", cmd);
    return SESSION_PARSING_ERROR;
  }

  char* new_expr = malloc(expr_end+1);
  char* aux = new_expr;
  strncpy(new_expr, cmd, expr_end);
  new_expr[expr_end] = '\0';

  variableDataU iterator_val;
  u8 iterator_type = eval_expr(env, &new_expr, &iterator_val);
  if (iterator_type != INTEGER) {
    ERROR("[INSTRUCTION ERROR] Iterator must be of type INTEGER\n", 0);
    return SESSION_PARSING_ERROR;
  }
  cmd += expr_end;
  free(aux);

  tok = get_next_token(&cmd, buf, TOK_TO);
  if (tok == TOK_ERROR) return SESSION_PARSING_ERROR; // PARSING ERROR

  /* iterator limit */
  expr_end = find_substring("STEP", cmd);
  bool no_step = false;
  if (expr_end == strlen(cmd)) { /* NO STEP */
    no_step = true;
  }
  new_expr = malloc(expr_end+1);
  aux = new_expr;
  strncpy(new_expr, cmd, expr_end);
  new_expr[expr_end] = '\0';

  variableDataU limit_val;
  u8 limit_type = eval_expr(env, &new_expr, &limit_val);
  if (limit_type != INTEGER) {
    ERROR("[INSTRUCTION ERROR] Iterator must be of type INTEGER\n", 0);
    return SESSION_PARSING_ERROR;
  }
  cmd += expr_end;
  free(aux);

  if (iterator_val.integer >= limit_val.integer) {
    u64 next_ln = find_next(env, ln);
    if (next_ln == 0) {
      ERROR("[INSTRUCTION ERROR] Cannot find NEXT\n");
      return SESSION_UNKNOWN_ERROR;
    }

    env->for_stack[sp].line = ln;
    env->for_stack[sp].limit = limit_val.integer;
    env->for_stack[sp].step = 1;
    env->for_stack[sp].next_line = 0;
    strncpy(env->for_stack[sp].iterator, varname, strlen(varname)+1);
    env->metadata.for_stackpointer += 1;
    add_integer_variable(env, iterator_val.integer, varname);

    set_jump_flag(env, next_ln);

    return SESSION_NO_ERROR;
  }

  if (no_step) {
    env->for_stack[sp].line = ln;
    env->for_stack[sp].limit = limit_val.integer;
    env->for_stack[sp].step = 1;
    env->for_stack[sp].next_line = 0;
    strncpy(env->for_stack[sp].iterator, varname, strlen(varname)+1);
    env->metadata.for_stackpointer += 1;
    add_integer_variable(env, iterator_val.integer, varname);

    return SESSION_NO_ERROR;
  }

  tok = get_next_token(&cmd, buf, TOK_STEP);
  if (tok == TOK_ERROR) return SESSION_PARSING_ERROR; // PARSING ERROR

  /* step value */
  variableDataU step_val;
  u8 step_type = eval_expr(env, &cmd, &step_val);
  if (step_type != INTEGER) {
    ERROR("[INSTRUCTION ERROR] Iterator must be of type INTEGER\n", 0);
    return SESSION_PARSING_ERROR;
  }
  cmd += expr_end;

  env->for_stack[sp].line = ln;
  env->for_stack[sp].limit = limit_val.integer;
  env->for_stack[sp].step = step_val.integer;
  env->for_stack[sp].next_line = 0;
  strncpy(env->for_stack[sp].iterator, varname, strlen(varname)+1);
  env->metadata.for_stackpointer += 1;
  add_integer_variable(env, iterator_val.integer, varname);

  return SESSION_NO_ERROR;
}

sessionErrorCodeE next_instr(sessionS* env, char* cmd, u64 ln) {
  u64 sp = env->metadata.for_stackpointer;

  if (sp == 0) {
    ERROR("[INSTRUCTION ERROR] Empty for stack\n", 0);
    return SESSION_EMPTY_STACK;
  }

  if (env->for_stack[sp-1].next_line == 0) {
    env->for_stack[sp-1].next_line = get_next_instr_line(env, ln);
  }

  set_jump_flag(env, env->for_stack[sp-1].line);

  return SESSION_NO_ERROR;
}