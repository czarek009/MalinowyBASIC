#include "instructions.h"
#include "interpreter.h"
#include "parser.h"
#include "evaluator.h"
#include "session.h"
#include "butils.h"
#include "mm.h"


sessionErrorCodeE let_instr(sessionS* env, char* cmd) {
  char varname[8] = {0};
  char buf[32] = {0};
  bool isStr = false;
  tokenE tok = TOK_NONE;

  /* varname */
  tok = get_next_token(&cmd, buf, TOK_ANY); // copy straight to varname instead buf?
  if (tok != TOK_VAR && tok != TOK_ARRAY_FLOAT && tok != TOK_ARRAY_INT && tok != TOK_ARRAY_STRING) return SESSION_PARSING_ERROR; // PARSING ERROR
  strncpy(varname, buf, 8);
  if (tok == TOK_VAR) {
    /* $ = */
    tok = get_next_token(&cmd, buf, TOK_ANY);
    if (tok == TOK_DOLAR) {
      isStr = true;
      tok = get_next_token(&cmd, buf, TOK_EQ);
      if (tok == TOK_ERROR) return SESSION_PARSING_ERROR; // PARSING ERROR
    } else if (tok != TOK_EQ) {
      return SESSION_PARSING_ERROR; // PARSING ERROR
    }

    /* value */
    if (isStr) {
      /* string */
      tok = get_next_token(&cmd, buf, TOK_QUOTE);
      if (tok == TOK_ERROR) return SESSION_PARSING_ERROR; // PARSING ERROR
      get_const_str(&cmd, buf);
      add_string_variable(env, buf, varname);
    } else {
      /* number */
      tok = get_next_token(&cmd, buf, TOK_ANY);
      if (tok != TOK_NUMBER && tok != TOK_LPAREN && tok != TOK_VAR && tok != TOK_FN && tok != TOK_ARRAY_INT && tok != TOK_ARRAY_FLOAT) {
        return SESSION_PARSING_ERROR; // PARSING ERROR
      }
      if(tok == TOK_ARRAY_FLOAT) cmd -= 2;
      if(tok == TOK_ARRAY_INT) cmd -= 1;
      cmd -= strlen(buf);
      variableDataU value;
      u8 value_type = eval_expr(env, &cmd, &value);
      DEBUG(" value_type: %u\n", (u32)value_type);
      DEBUG(" value: %ld\n", value.integer);
      if (value_type >= 253) {
        ERROR("[INSTRUCTOION ERROR] Expression evaluation error\n", 0);
        return SESSION_EVAL_ERROR;
      }
      add_variable(env, value, varname, value_type);
    }
  }
  else {
    u8 dimentions = 0;
    sessionErrorCodeE array_err = parse_array_dimentions(cmd, &dimentions);
    if(array_err != SESSION_NO_ERROR) return array_err;
    u8 real_dimentions = 0;
    u8 arr_type = get_array_dimentions_and_type(env, varname, &real_dimentions);
    if(arr_type == NOT_FOUND) return SESSION_INVALID_VAR_NAME;
    u8 arr_parsed_type;
    switch(tok){
      case TOK_ARRAY_INT:
        arr_parsed_type = INTEGER;
        break;
      case TOK_ARRAY_FLOAT:
        arr_parsed_type = FLOATING_POINT;
        break;
      case TOK_ARRAY_STRING:
        arr_parsed_type = STRING;
        break;
      default:
        return SESSION_PARSING_ERROR;
    }
    if(arr_type != arr_parsed_type){
      ERROR("[INSTRUCTION ERROR] Array %s is different type than parsed\n", varname);
      return SESSION_PARSING_ERROR;
    }
    if(real_dimentions != dimentions){
      ERROR("[INSTRUCTION ERROR] wrong dimentions to varname %s, given dim=%d, rel dim=%d\n", varname, dimentions, real_dimentions);
      return SESSION_PARSING_ERROR;
    }
    u8 *idxs = eval_array_sizes(env, &cmd, dimentions);
    if(idxs == NULL) return SESSION_PARSING_ERROR;
    tok = get_next_token(&cmd, buf, TOK_EQ);
    if (tok == TOK_ERROR) {
      free(idxs);
      return SESSION_PARSING_ERROR;
    }
    variableDataU value;
    u8 value_type = eval_expr(env, &cmd, &value);
    DEBUG(" value_type: %u\n", (u32)value_type);
    DEBUG(" value: %ld\n", value.integer);
    if (value_type >= 253) {
      free(idxs);
      ERROR("[INSTRUCTOION ERROR] Expression evaluation error\n", 0);
      return SESSION_EVAL_ERROR;
    }
    if(arr_type != value_type){
      free(idxs);
      ERROR("[INSTRUCTION ERROR] Array type not compatible with eval type\n");
      return SESSION_PARSING_ERROR;
    }
    array_err = update_array(env, varname, value, idxs);
    free(idxs);
    if(array_err != SESSION_NO_ERROR) return array_err;
  }
  tok = get_next_token(&cmd, buf, TOK_NONE);
  if (tok == TOK_ERROR) return SESSION_PARSING_ERROR; // PARSING ERROR

  return SESSION_NO_ERROR; // SUCCESS
}