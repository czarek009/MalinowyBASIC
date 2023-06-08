u64 get_next_instr(Session* env)                             // return line number
void set_next_instr(Session* env, u64 lnum)                  // what if there's no instruction at `lnum`?

void get_sess_status(Session* env)                           // stopped? finished? new?
void set_sess_status(Session* env, statusE stat)             //       --||--

varU get_var_value(Session* env, char* varname)              // what with var type?
void set_val_value(Session* env, char* varname, varU value)  // if it's complex data structure, caller must allocate memory and set pointer
                                                          // if variable exists, update value
                                                          // dynamic typing? then someone has to free memory (if type changes from [] to int)

void add_instruction(Session* env, u64 lnum, char* cmd)      // insert `cmd` to linked list at pos `lnum`; if exists than update
void delete_instr(Session* env, u64 lnum)                    // delete instruction with given `lnum` from linked list