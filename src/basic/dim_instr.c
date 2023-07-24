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


sessionErrorCodeE dim_instr(sessionS* env, char* cmd) {
  char buf[32] = {0};
  tokenE tok = TOK_NONE;
  tok = get_next_token(&cmd, buf, TOK_ANY);
  u8 arr_type;
  switch(tok){
    case TOK_ARRAY_INT:
      arr_type = INTEGER;
      break;
    case TOK_ARRAY_FLOAT:
      arr_type = FLOATING_POINT;
      break;
    case TOK_ARRAY_STRING:
      arr_type = STRING;
      break;
    default:
      return SESSION_PARSING_ERROR;
  }
  u8 dimentions = 0;
  sessionErrorCodeE parse_err = parse_array_dimentions(cmd, &dimentions);
  if(parse_err != SESSION_NO_ERROR) return parse_err;
  u8 *dim_sizes = eval_array_sizes(env, &cmd, dimentions);
  if(dim_sizes == NULL) return SESSION_PARSING_ERROR;
  add_array_variable(env, buf, dimentions, dim_sizes, arr_type);
  free(dim_sizes);
  tok = get_next_token(&cmd, buf, TOK_NONE);
  if (tok == TOK_ERROR) return SESSION_PARSING_ERROR; // PARSING ERROR
  return SESSION_NO_ERROR;
}
