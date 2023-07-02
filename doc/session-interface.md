u64 get_next_instr(sessionS* env)                             // return line number
void set_next_instr(sessionS* env, u64 lnum)                  // what if there's no instruction at `lnum`?

void get_sess_status(sessionS* env)                           // stopped? finished? new?
void set_sess_status(sessionS* env, statusE stat)             //       --||--

varU get_var_value(sessionS* env, char* varname)              // what with var type?
void set_val_value(sessionS* env, char* varname, varU value)  // if it's complex data structure, caller must allocate memory and set pointer
                                                          // if variable exists, update value
                                                          // dynamic typing? then someone has to free memory (if type changes from [] to int)

void add_instruction(sessionS* env, u64 lnum, char* cmd)      // insert `cmd` to linked list at pos `lnum`; if exists than update
void delete_instr(sessionS* env, u64 lnum)                    // delete instruction with given `lnum` from linked list