#ifndef _INSTRUCTIONS_H
#define _INSTRUCTIONS_H

#include "session.h"
#include "types.h"

sessionErrorCodeE let_instr(sessionS* env, char* cmd);
sessionErrorCodeE print_instr(sessionS* env, char* cmd);
sessionErrorCodeE input_instr(sessionS* env, char* cmd);
sessionErrorCodeE goto_instr(sessionS* env, char* cmd);
sessionErrorCodeE gosub_instr(sessionS* env, char* cmd, u64 ln);
sessionErrorCodeE return_instr(sessionS* env, char* cmd);
sessionErrorCodeE def_instr(sessionS* env, char* cmd);

#endif /* _INSTRUCTIONS_H */