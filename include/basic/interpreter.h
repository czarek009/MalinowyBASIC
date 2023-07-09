#ifndef _INTERPRETER_H
#define _INTERPRETER_H

#include "session.h"
#include "types.h"
#include "printf.h"
#include "debug.h"


enum interpreter_resultE {
  INTERP_SUCCESS,
  INTERP_UNKNOWN_ERROR,
  INTERP_UNEXPECTED_TOKEN,
  INTERP_INVALID_VAR_NAME,
  INTERP_INVALID_EXPR,
} typedef interpreter_resultE;

void interpreter_process_input(sessionS* env, char* cmd);
void interpreter_execute_command(sessionS* env, char* cmd, u64 line_number);


#endif /* _INTERPRETER_H */
