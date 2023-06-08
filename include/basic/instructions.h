#ifndef _INSTRUCTIONS_H
#define _INSTRUCTIONS_H

#include "session.h"
#include "types.h"

void let_instr(Session* env, char* cmd);
void print_instr(Session* env, char* cmd);
void input_instr(Session* env, char* cmd);

#endif /* _INSTRUCTIONS_H */