#include "instructions.h"
#include "interpreter.h"
#include "parser.h"
#include "evaluator.h"
#include "session.h"
#include "butils.h"
#include "mm.h"


#define GOTO 1
#define GOSUB 2

sessionErrorCodeE on_instr(sessionS* env, char* cmd, u64 ln) {
  char buf[32] = {0};
  char* expr = NULL;
  char* aux = NULL;
  tokenE tok = TOK_NONE;
  variableDataU selector = {0};
  u64 sval = 0;
  u64 expr_end = 0;
  u8 expr_type = NOT_FOUND;
  u8 variant = 0;

  expr_end = find_substring("GOSUB", cmd);
  variant = GOSUB;
  if (expr_end == strlen(cmd)) {
    expr_end = find_substring("GOTO", cmd);
    variant = GOTO;
    if (expr_end == strlen(cmd)) {
      ERROR("[INSTRUCTION ERROR] Invalid instruction: %s\n", cmd);
      return SESSION_PARSING_ERROR;
    }
  }
  expr = malloc(expr_end+1);
  aux = expr;
  strncpy(expr, cmd, expr_end);
  expr[expr_end] = '\0';
  expr_type = eval_expr(env, &expr, &selector);
  cmd += expr_end;
  if (expr_type != INTEGER) {
    ERROR("[INSTRUCTION ERROR] ON instruction selector must be int\n", 0);
    return SESSION_PARSING_ERROR;
  }
  sval = selector.integer;

  if (variant == GOTO) {
    tok = get_next_token(&cmd, buf, TOK_GOTO);
    if (tok == TOK_ERROR) return SESSION_PARSING_ERROR; // PARSING ERROR
  }
  if (variant == GOSUB) {
    tok = get_next_token(&cmd, buf, TOK_GOSUB);
    if (tok == TOK_ERROR) return SESSION_PARSING_ERROR; // PARSING ERROR
  }

  for (u64 i = 0; i < sval-1; ++i) {
    tok = get_next_token(&cmd, buf, TOK_NUMBER);
    if (tok == TOK_ERROR) return SESSION_PARSING_ERROR; // PARSING ERROR

    tok = get_next_token(&cmd, buf, TOK_COMMA);
    if (tok == TOK_ERROR) return SESSION_PARSING_ERROR; // PARSING ERROR
  }

  tok = get_next_token(&cmd, buf, TOK_NUMBER);
  if (tok == TOK_ERROR) return SESSION_PARSING_ERROR; // PARSING ERROR

  s64 jump_addr = str2s64(buf);

  if (jump_addr < 1) {
    ERROR("[INSTRUCTION ERROR] Jump address must be > 0\n");
    return SESSION_INVALID_JUMP;
  }

  if (variant == GOSUB) {
    u64 next_instr = get_next_instr_line(env, ln);
    push_return_address_to_stack(env, next_instr);
  }
  set_jump_flag(env, jump_addr);

  free(aux);
  return SESSION_NO_ERROR;
}