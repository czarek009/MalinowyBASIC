#ifndef _INTERPRETER_H
#define _INTERPRETER_H

#include "session.h"
#include "types.h"
#include "printf.h"
#include "debug.h"


enum interpreterResultE {
  INTERP_SUCCESS,
  INTERP_UNKNOWN_ERROR,
  INTERP_UNKNOWN_TOKEN,
  INTERP_UNEXPECTED_TOKEN,
  INTERP_INVALID_VAR_NAME,
  INTERP_INVALID_EXPR
} typedef interpreterResultE;

void interpreter_process_input(sessionS* env, char* cmd);
interpreterResultE interpreter_execute_command(sessionS* env, char* cmd, u64 line_number);


#endif /* _INTERPRETER_H */
