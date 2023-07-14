#ifndef _INTERPRETER_H
#define _INTERPRETER_H

#include "session.h"
#include "types.h"
#include "printf.h"
#include "debug.h"


#define NO_LINE_NUMBER ((u64)(~0))


sessionErrorCodeE interpreter_process_input(sessionS* env, char* cmd);
sessionErrorCodeE interpreter_execute_command(sessionS* env, char* cmd, u64 line_number);


#endif /* _INTERPRETER_H */
