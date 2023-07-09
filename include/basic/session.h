#ifndef _SESSION_H
#define _SESSION_H

#include "mm.h"
#include "types.h"
#include "variable.h"
#include "debug.h"
#include "interpreter.h"

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

#define DATA_STACK_SIZE           512
#define DATA_STACK_MAX_FIELD      (DATA_STACK_SIZE / sizeof(s32)) - 1

#define VARIABLES_MAX_FIELD       63
#define VARIABLE_NAME_SIZE        7

typedef enum {
    SESSION_RUNNING = 0,
    SESSION_STOPPED = 1,
    SESSION_FINISHED = 2,
    SESSION_MODIFIED = 3,
    SESSION_NEW = 4,
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
    interpreterResultE error_code;
    sessionStatusE status;
    u64 jump_flag;
    u8 return_address_stackpointer;
    u8 data_stackpointer;
    u8 variables_number;
    u8 reserved[217];
} metadataS;

typedef union VariableData {
    u64 *pointer;
    s32 integer;
    double floating_point;
    bool boolean;
    char *string;
} VariableData;

typedef struct variableS {
    char name[7];
    u8 type;
    variableDataU data;
} variableS;

typedef struct sessionS {
    metadataS metadata;
    u64 return_address_stack[32];
    s32 data_stack[128];
    variableS variables[64];
} sessionS;


void print_structures_size(void);
sessionS *session_init(void);
sessionStatusE get_session_status(sessionS *s);
void set_session_status(sessionS *s, sessionStatusE status);
void set_jump_flag(sessionS *s, u64 line_number);

void push_data_to_stack(sessionS *s, s32 data);
s32 pop_data_from_stack(sessionS *s);
void print_data_stack(sessionS *s);

void push_return_address_to_stack(sessionS *s, u64 address);
u64 pop_return_address_from_stack(sessionS *s);
void print_return_address_stack(sessionS *s);

variableS *get_variable_ptr(sessionS *s, char* name);
u8 get_variable_value(sessionS *s, char* name, variableDataU *var_data);
void add_variable(sessionS *s, variableDataU var_data, char *name, u8 type);
void add_integer_variable(sessionS *s, s64 data, char *name);
void add_floating_point_variable(sessionS *s, float data, char *name);
void add_boolean_variable(sessionS *s, bool data, char *name);
void add_string_variable(sessionS *s, char *data, char *name);
void print_variables(sessionS *s);

void add_instruction(sessionS *s, u64 line_number, char *instruction);
void delete_single_instruction(sessionS *s, u64 line_number);
void delete_all_instructions(sessionS *s);
void print_instructions(sessionS *s);
void run_program(sessionS *s);
void set_error_code(sessionS *s, interpreterResultE error_code);

u64 get_next_instr_line(sessionS *s, u64 ln);

void session_end(sessionS *s);

#endif /* _SESSION_H */