#ifndef _SESSION_H
#define _SESSION_H

#include "mm.h"
#include "types.h"

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
 - pointer to array

 data:
  - unia (u64, wskaźnik)

 pointer ma na początku metadane
*/

#define RET_ADDR_STACK_SIZE        256
#define RET_ADDR_STACK_MAX_FIELD  (RET_ADDR_STACK_SIZE / sizeof(u32)) - 1

#define DATA_STACK_SIZE           512
#define DATA_STACK_MAX_FIELD      (DATA_STACK_SIZE / sizeof(s32)) - 1

#define VARIABLES_MAX_FIELD       63
#define VARIABLE_NAME_SIZE
#define VARIABLE_NAME_MAX_FIELD   (VARIABLE_NAME_SIZE - 1)

#define POINTER 0
#define INTEGER 1
#define FLOATING_POINT 2
#define BOOLEAN 3

typedef enum {
    RUNNING = 0,
    STOPPED = 1,
    FINISHED = 2
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
    SessionStatus status;
    u8 reserved[232];
} Metadata;

typedef union VariableData {
    u64 *pointer;
    s32 integer;
    u64 floating_point;
    bool boolean;
} VariableData;

typedef struct Variable {
    char name[7];
    u8 type;
    VariableData data;
} Variable;

typedef struct Session {
    Metadata metadata;
    u32 return_address_stack[64];
    s32 data_stack[128];
    Variable variables[64];
} Session;


void print_structures_size(void);
Session *session_init(void);
void session_end(Session *s);
SessionStatus get_session_status(Session *s);
void set_session_status(Session *s, SessionStatus status);
void push_data_to_stack(Session *s, s32 data);
s32 pop_data_from_stack(Session *s);
void push_return_address_to_stack(Session *s, s32 address);
u32 pop_return_address_from_stack(Session *s);
bool compare_name(char *variable_name, char* name);
Variable *get_variable_ptr(Session *s, char* name);
VariableData get_variable_value(Session *s, char* name);
void add_integer_variable(Session *s, s32 data, char *name);
void add_floating_point_variable(Session *s, float data, char *name);
void add_boolean_variable(Session *s, bool data, char *name);
void add_instruction(Session *s, u64 line_number, char *instruction);
void print_instructions(Session *s);

#endif /* _SESSION_H */