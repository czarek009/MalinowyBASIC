#ifndef _INSTRUCTIONS_H
#define _INSTRUCTIONS_H

#include "session.h"
#include "types.h"

sessionErrorCodeE let_instr(sessionS* env, char* cmd);
sessionErrorCodeE print_instr(sessionS* env, char* cmd);
sessionErrorCodeE input_instr(sessionS* env, char* cmd);
sessionErrorCodeE goto_instr(sessionS* env, char* cmd);
sessionErrorCodeE gosub_instr(sessionS* env, char* cmd, u64 ln);
sessionErrorCodeE on_instr(sessionS* env, char* cmd, u64 ln);
sessionErrorCodeE return_instr(sessionS* env, char* cmd);
sessionErrorCodeE def_instr(sessionS* env, char* cmd);
sessionErrorCodeE if_instr(sessionS* env, char* cmd, u64 ln);
sessionErrorCodeE for_instr(sessionS* env, char* cmd, u64 ln);
sessionErrorCodeE next_instr(sessionS* env, char* cmd, u64 ln);
sessionErrorCodeE dim_instr(sessionS* env, char* cmd);
sessionErrorCodeE data_instr(sessionS* env, char* cmd);
sessionErrorCodeE read_instr(sessionS* env, char* cmd);
sessionErrorCodeE restore_instr(sessionS* env, char* cmd);
sessionErrorCodeE save_instr(sessionS* env, char* cmd);
sessionErrorCodeE load_instr(sessionS* env, char* cmd);
sessionErrorCodeE font_instr(sessionS* env, char* cmd);
sessionErrorCodeE background_instr(sessionS* env, char* cmd);
sessionErrorCodeE delete_instr(sessionS* env, char* cmd);
sessionErrorCodeE help_instr(sessionS* env, char* cmd);

#endif /* _INSTRUCTIONS_H */