#ifndef _INSTRUCTIONS_H
#define _INSTRUCTIONS_H

#include "session.h"
#include "types.h"

void let_instr(sessionS* env, char* cmd);
void print_instr(sessionS* env, char* cmd);
void input_instr(sessionS* env, char* cmd);
void goto_instr(sessionS* env, char* cmd);
void gosub_instr(sessionS* env, char* cmd, u64 ln);
void return_instr(sessionS* env, char* cmd);

#endif /* _INSTRUCTIONS_H */