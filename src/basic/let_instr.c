#include "instructions.h"
#include "interpreter.h"
#include "parser.h"
#include "evaluator.h"
#include "session.h"
#include "butils.h"
#include "mm.h"


u64 assign_end(char* instr);

sessionErrorCodeE let_instr(sessionS* env, char* cmd) {
  char varname[8] = {0};
  char buf[32] = {0};
  bool isStr = false;
  bool more_vars = false;
  tokenE tok = TOK_NONE;

  u64 next_assign = assign_end(cmd);
  if (next_assign != strlen(cmd)) {
    more_vars = true;
    cmd[next_assign] = '\0';
  }

  /* varname */
  tok = get_next_token(&cmd, buf, TOK_ANY); // copy straight to varname instead buf?
  if (tok != TOK_VAR && tok != TOK_ARRAY_FLOAT && tok != TOK_ARRAY_INT && tok != TOK_ARRAY_STRING) {
    ERROR("[INSTRUCTION ERROR] Expected variable or array element, got '%s'\n", buf);
    return SESSION_PARSING_ERROR; // PARSING ERROR
  }
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
      if (tok != TOK_NUMBER && tok != TOK_LPAREN &&
          tok != TOK_VAR && tok != TOK_FN &&
          tok != TOK_ARRAY_INT && tok != TOK_ARRAY_FLOAT &&
          tok != TOK_BUILTIN && tok != TOK_HEX_NUMBER) {
        return SESSION_PARSING_ERROR; // PARSING ERROR
      }
      if(tok == TOK_ARRAY_FLOAT) cmd -= 2;
      if(tok == TOK_ARRAY_INT) cmd -= 1;
      cmd -= strlen(buf);
      variableDataU value;
      u8 value_type = eval_expr(env, &cmd, &value);
      if (value_type >= 253) {
        ERROR("[INSTRUCTOION ERROR] Expression evaluation error\n", 0);
        return SESSION_EVAL_ERROR;
      }
      add_variable(env, value, varname, value_type);
    }
  }
  else {
    u8 type = get_array_parsed_type(tok);
    if(type == NOT_FOUND) return SESSION_PARSING_ERROR;
    u8 dim_nr = 0;
    sessionErrorCodeE array_err = SESSION_NO_ERROR;
    array_err = parse_array_dim_nr(cmd, &dim_nr);
    if(array_err != SESSION_NO_ERROR) return array_err;
    u64 *idxs = eval_array_sizes(env, &cmd, dim_nr);
    if(idxs == NULL) return SESSION_PARSING_ERROR;
    array_err = check_array_parameters(env, varname, type, dim_nr, idxs);
    if(array_err != SESSION_NO_ERROR) {
      free(idxs);
      return array_err;
    }
    tok = get_next_token(&cmd, buf, TOK_EQ);
    if (tok == TOK_ERROR) {
      free(idxs);
      return SESSION_PARSING_ERROR;
    }
    variableDataU value;
    u8 value_type = eval_expr(env, &cmd, &value);
    if (value_type >= 253) {
      free(idxs);
      ERROR("[INSTRUCTOION ERROR] Expression evaluation error\n", 0);
      return SESSION_EVAL_ERROR;
    }
    if(type != value_type){
      free(idxs);
      ERROR("[INSTRUCTION ERROR] Array type not compatible with eval type\n");
      return SESSION_PARSING_ERROR;
    }
    array_err = update_array(env, varname, value, idxs);
    free(idxs);
    if(array_err != SESSION_NO_ERROR) return array_err;
  }
  // tok = get_next_token(&cmd, buf, TOK_NONE);
  // if (tok == TOK_ERROR) return SESSION_PARSING_ERROR; // PARSING ERROR

  if (more_vars) {
    return let_instr(env, cmd+1);
  }

  return SESSION_NO_ERROR; // SUCCESS
}


u64 assign_end(char* instr) {
  bool quote = false;
  for (u64 i = 0; instr[i] != '\0'; ++i) {
    if (instr[i] == '"') {
      quote = !quote;
    }
    if (!quote && instr[i] == ':') {
      return i;
    }
  }
  return strlen(instr);
}