#ifndef _SESSION_H
#define _SESSION_H

#include "mm.h"
#include "types.h"
#include "variable.h"
// #include "debug.h"
// #include "interpreter.h"

/*

TODO:
- napisać dokumentację w folderze doc
- array variable

*/


/*
1 session:
- 256B metadata
- 256B stos adresów powrotu
- 512B stos danych
.      | 7B | 1B | 8B |
- 64 x |name|type|data| = 1024B

types:
- int
- bool
- floating point
- string
- pointer to array

*/

#define RET_ADDR_STACK_SIZE        256
#define RET_ADDR_STACK_MAX_FIELD  (RET_ADDR_STACK_SIZE / sizeof(u64)) - 1

#define data_queue_SIZE           512
#define data_queue_MAX_FIELD      (data_queue_SIZE / sizeof(s32)) - 1

#define VARIABLES_MAX_FIELD       63
#define VARIABLE_NAME_SIZE        7


typedef enum sessionErrorCodeE {
  SESSION_NO_ERROR,
  SESSION_UNKNOWN_ERROR,
  SESSION_PARSING_ERROR,
  SESSION_FS_ERROR,
  SESSION_UNKNOWN_TOKEN,
  SESSION_EVAL_ERROR,
  SESSION_INVALID_INSTRUCTION,
  SESSION_INVALID_JUMP,
  SESSION_INVALID_VAR_NAME,
  SESSION_INVALID_EXPR,
  SESSION_EMPTY_STACK,
  SESSION_END,
} sessionErrorCodeE;

typedef enum {
  SESSION_STATUS_RUNNING = 0,
  SESSION_STATUS_STOPPED = 1,
  SESSION_STATUS_FINISHED = 2,
  SESSION_STATUS_MODIFIED = 3,
  SESSION_STATUS_NEW = 4,
  SESSION_STATUS_ERROR,
} sessionStatusE;

typedef struct instructionS {
  struct instructionS *previous;
  struct instructionS *next;
  u64 line_number;
  char *instruction;
} instructionS;

typedef struct metadataS {
  instructionS *instructions_start;
  instructionS *instructions_end;
  instructionS *resume_from;
  sessionErrorCodeE error_code;
  sessionStatusE status;
  u64 jump_flag;
  u8 return_address_stackpointer;
  u8 data_queue_start;
  u8 data_queue_end;
  u8 for_stackpointer;
  u8 variables_number;
  u8 functions_number;
  u8 reserved[208];
} metadataS;

typedef struct variableS {
  char name[7];
  u8 type;
  variableDataU data;
} variableS;

typedef struct dataQueueS {
  u8 type;
  variableDataU value;
} dataQueueS;

typedef struct functionS {
  char funname[7];
  char argname[7];
  char* body;
} functionS;

typedef struct forS {
  u64 line;
  u64 limit;
  u64 step;
  u64 next_line;
  char iterator[8];
} forS;

typedef struct sessionS {
  metadataS metadata;
  forS for_stack[8];
  u64 return_address_stack[32];
  dataQueueS data_queue[128];
  variableS variables[64];
  functionS functions[64];
} sessionS;


void print_structures_size(void);
sessionS *session_init(void);
sessionStatusE get_session_status(sessionS *s);
void set_session_status(sessionS *s, sessionStatusE status);
void set_jump_flag(sessionS *s, u64 line_number);

void push_data_to_queue(sessionS *s, dataQueueS data);
dataQueueS* read_data_from_queue(sessionS *s);
void print_data_queue(sessionS *s);

void push_return_address_to_stack(sessionS *s, u64 address);
u64 pop_return_address_from_stack(sessionS *s);
void print_return_address_stack(sessionS *s);

variableS *get_variable_ptr(sessionS *s, char* name);
u8 get_variable_value(sessionS *s, char* name, variableDataU *var_data);
void add_variable(sessionS *s, variableDataU var_data, char *name, u8 type);
void add_integer_variable(sessionS *s, s64 data, char *name);
void add_floating_point_variable(sessionS *s, double data, char *name);
void add_boolean_variable(sessionS *s, bool data, char *name);
void add_string_variable(sessionS *s, char *data, char *name);
void print_variables(sessionS *s);

void add_function(sessionS *s, char* funname, char* argname, char* body);
u8 apply_function(sessionS *s, char* funname, variableDataU arg, u8 argtype, variableDataU* result);

void add_array_variable(sessionS *s, char *name, u8 dim_nr, u64 *dims, u8 type);
sessionErrorCodeE check_array_parameters(sessionS *s, char *name, u8 parsed_type, u8 parsed_dim_nr, u64 *parsed_dims);
sessionErrorCodeE update_array(sessionS *s, char *name, variableDataU value, u64 *idxs);
sessionErrorCodeE get_array_element(sessionS *s, char *name, u64 *idxs, variableDataU *data);

void add_instruction(sessionS *s, u64 line_number, char *instruction);
void delete_single_instruction(sessionS *s, u64 line_number);
void delete_all_instructions(sessionS *s);
void print_instructions(sessionS *s);
sessionErrorCodeE run_program(sessionS *s);
u64 find_next(sessionS *s, u64 ln);

u64 get_next_instr_line(sessionS *s, u64 ln);
instructionS *get_next_instruction(sessionS *s, u64 line_number);
void print_session_info(sessionS* s);

void session_end(sessionS *s);

#endif /* _SESSION_H */