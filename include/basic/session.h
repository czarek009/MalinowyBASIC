#ifndef _SESSION_H
#define _SESSION_H

#include "mm.h"
#include "types.h"

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

// #define DEBUG_MODE 1
#if DEBUG_MODE
#define DEBUG(...)  printf(__VA_ARGS__)
#else
#define DEBUG(...)
#endif

#define RET_ADDR_STACK_SIZE        256
#define RET_ADDR_STACK_MAX_FIELD  (RET_ADDR_STACK_SIZE / sizeof(u64)) - 1

#define DATA_STACK_SIZE           512
#define DATA_STACK_MAX_FIELD      (DATA_STACK_SIZE / sizeof(s32)) - 1

#define VARIABLES_MAX_FIELD       63
#define VARIABLE_NAME_SIZE        7
#define VARIABLE_NAME_MAX_FIELD   (VARIABLE_NAME_SIZE - 1)

/* VARIABLE TYPES */
#define POINTER ((u8)0)
#define INTEGER ((u8)1)
#define FLOATING_POINT ((u8)2)
#define BOOLEAN ((u8)3)
#define STRING ((u8)4)
#define NOT_FOUND ((u8)255)

typedef enum {
    RUNNING = 0,
    STOPPED = 1,
    FINISHED = 2,
    MODIFIED = 3,
    NEW = 4
} SessionStatus;

typedef struct Node {
    struct Node *previous;
    struct Node *next;
    u64 line_number;
    char *instruction;
} Node;

typedef struct Metadata {
    Node *instructions_start;
    Node *instructions_end;
    u8 return_address_stackpointer;
    u8 data_stackpointer;
    u8 variables_number;
    u8 errno;
    u64 jump_flag;
    SessionStatus status;
    u8 reserved[220];
} Metadata;

typedef union VariableData {
    u64 *pointer;
    s32 integer;
    u64 floating_point;
    bool boolean;
    char *string;
} VariableData;

typedef struct Variable {
    char name[7];
    u8 type;
    VariableData data;
} Variable;

typedef struct Session {
    Metadata metadata;
    u64 return_address_stack[32];
    s32 data_stack[128];
    Variable variables[64];
} Session;


void print_structures_size(void);
Session *session_init(void);
SessionStatus get_session_status(Session *s);
void set_session_status(Session *s, SessionStatus status);
void set_jump_flag(Session *s, u64 line_number);

void push_data_to_stack(Session *s, s32 data);
s32 pop_data_from_stack(Session *s);
void print_data_stack(Session *s);

void push_return_address_to_stack(Session *s, u64 address);
u64 pop_return_address_from_stack(Session *s);
void print_return_address_stack(Session *s);

Variable *get_variable_ptr(Session *s, char* name);
u8 get_variable_value(Session *s, char* name, VariableData *var_data);
void add_integer_variable(Session *s, s32 data, char *name);
void add_floating_point_variable(Session *s, float data, char *name);
void add_boolean_variable(Session *s, bool data, char *name);
void add_instruction(Session *s, u64 line_number, char *instruction);
void add_string_variable(Session *s, char *data, char *name);
void print_variables(Session *s);

void add_instruction(Session *s, u64 line_number, char *instruction);
void delete_single_instruction(Session *s, u64 line_number);
void delete_all_instructions(Session *s);
void print_instructions(Session *s);
void run_program(Session *s);
u64 get_next_instr_line(Session *s, u64 ln);

void session_end(Session *s);

#endif /* _SESSION_H */