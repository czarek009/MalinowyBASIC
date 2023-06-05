#include "session.h"
#include "printf.h"

/* SESSION */
void print_structures_size(void) {
    printf("Metadata size = %ld\n", sizeof(Metadata));
    printf("Variable size = %ld\n", sizeof(Variable));
    printf("Session size = %ld\n", sizeof(Session));
}

Session *session_init(void) {
    Session *s = malloc(sizeof(Session));
    s->metadata.instructions_start = NULL;
    s->metadata.instructions_end = NULL;
    s->metadata.return_address_stackpointer = 0;
    s->metadata.data_stackpointer = 0;
    s->metadata.variables_number = 0;
    s->metadata.errno = 0;
    s->metadata.status = NEW;
    s->metadata.jump_flag = 0;
    return s;
}

SessionStatus get_session_status(Session *s) {
    return s->metadata.status;
}

void set_session_status(Session *s, SessionStatus status) {
    s->metadata.status = status;
}

void set_jump_flag(Session *s, u64 line_number) {
    s->metadata.jump_flag = line_number;
}

/* STACK */
void push_data_to_stack(Session *s, s32 data) {
    if(s->metadata.data_stackpointer < DATA_STACK_MAX_FIELD){
        s->data_stack[s->metadata.data_stackpointer] = data;
        s->metadata.data_stackpointer++;
    }
}

s32 pop_data_from_stack(Session *s) {
    s32 data = s->data_stack[s->metadata.data_stackpointer];
    if(s->metadata.data_stackpointer > 0) {
        s->metadata.data_stackpointer++;
    }
    return data;
}

void push_return_address_to_stack(Session *s, s32 address) {
    if(s->metadata.return_address_stackpointer < RET_ADDR_STACK_MAX_FIELD){
        s->return_address_stack[s->metadata.return_address_stackpointer] = address;
        s->metadata.return_address_stackpointer++;
    }
}

u32 pop_return_address_from_stack(Session *s) {
    u32 data = s->data_stack[s->metadata.data_stackpointer];
    if(s->metadata.data_stackpointer > 0) {
        s->metadata.data_stackpointer++;
    }
    return data;
}

/* VARIABLE */
bool compare_name(char *variable_name, char* name) {
    while(*variable_name != '\0' || *name != '\0') {
        if((*variable_name != *name) || (*variable_name == '\0') || (*name == '\0')){
            return false;
        }
        variable_name++;
        name++;
    }
    return true;
}

Variable *get_variable_ptr(Session *s, char* name) {
    Variable *variables = s->variables;
    u8 variables_number = s->metadata.variables_number;
    for(u8 i = 0; i < variables_number; i++, variables++) {
        if(compare_name(variables->name, name)) {
            return variables;
        }
    }
    return NULL;
}

u8 get_variable_value(Session *s, char* name, VariableData *var_data) {
    Variable *var = get_variable_ptr(s, name);
    /* variable not found */
    if (var == NULL) {
        return 255;
    }
    *var_data = var->data;
    return var->type;
}

void add_variable(Variable *var, VariableData data, char *name, u8 type) {
    for(u8 i = 0; (*name != '\0' && i < VARIABLE_NAME_MAX_FIELD); i++, name++) {
        var->name[i] = *name;
    }
    var->data = data;
    var->type = type;
}

void check_and_add_variable(Session *s, VariableData data, char *name, u8 type) {
    Variable *var = get_variable_ptr(s, name);
    if(var != NULL){
        add_variable(var, data, name, type);
    }
    else if (s->metadata.variables_number < VARIABLES_MAX_FIELD) {
        add_variable(&(s->variables[s->metadata.variables_number]), data, name, type);
        s->metadata.variables_number++;
    }
}

void add_integer_variable(Session *s, s32 data, char *name) {
    VariableData var_data;
    var_data.integer = data;
    check_and_add_variable(s, var_data, name, INTEGER);
}

void add_floating_point_variable(Session *s, float data, char *name) {
    VariableData var_data;
    var_data.floating_point = data;
    check_and_add_variable(s, var_data, name, FLOATING_POINT);
}

void add_boolean_variable(Session *s, bool data, char *name) {
    VariableData var_data;
    var_data.boolean = data;
    check_and_add_variable(s, var_data, name, BOOLEAN);
}

void add_string_variable(Session *s, char *data, char *name) {
    VariableData var_data;
    var_data.string = data;
    check_and_add_variable(s, var_data, name, STRING);
}

/* INSTRUCTIONS */
Node *create_node(Node *previous, Node *next, u64 line_number, char *instruction) {
    Node *new = malloc(sizeof(Node));
    new->previous = previous;
    new->next = next;
    new->line_number = line_number;
    new->instruction = instruction;
    return new;
}

Node *find_place(Node *head, u64 line_number) {
    Node *node = head;
    while((node->next != NULL) && (node->next->line_number < line_number)) {
        node = node->next;
    }
    return node;
}

void add_instruction(Session *s, u64 line_number, char *instruction) {
    if(s->metadata.instructions_start == NULL){
        Node *new_node = create_node(NULL, NULL, line_number, instruction);
        s->metadata.instructions_start = new_node;
        s->metadata.instructions_end = new_node;

    }
    else if(line_number < s->metadata.instructions_start->line_number) {
        Node *new_node = create_node(NULL, s->metadata.instructions_start, line_number, instruction);
        s->metadata.instructions_start->previous = new_node;
        s->metadata.instructions_start = new_node;
    }
    else if(line_number > s->metadata.instructions_end->line_number) {
        Node *new_node = create_node(s->metadata.instructions_end, NULL, line_number, instruction);
        s->metadata.instructions_end->next = new_node;
        s->metadata.instructions_end = new_node;

    }
    else {
        Node *place = find_place(s->metadata.instructions_start, line_number);
        if(place->line_number == line_number){
            place->instruction = instruction;
        }
        else if(place->next->line_number == line_number){
            place->next->instruction = instruction;
        }
        else {
            Node *next = place->next;
            Node *new_node = create_node(place, next, line_number, instruction);
            place->next = new_node;
            next->previous = new_node;
        }
    }
}

Node *find_node(Node *head, u64 line_number) {
    Node *node = head;
    while((node->line_number != line_number) && (node != NULL)) {
        node = node->next;
    }
    return node;
}

void delete_node(Node *node, Session *s) {
    if(node == s->metadata.instructions_start && node == s->metadata.instructions_end){
        s->metadata.instructions_start = NULL;
        s->metadata.instructions_end = NULL;
    }
    else if(node == s->metadata.instructions_start){
        node->next->previous = NULL;
        s->metadata.instructions_start = node->next;
    }
    else if(node == s->metadata.instructions_end){
        node->previous->next = NULL;
        s->metadata.instructions_end = node->previous;
    }
    else {
        node->previous->next = node->next;
        node->next->previous = node->previous;
    }
    free(node);
}

void delete_single_instruction(Session *s, u64 line_number) {
    Node *node = find_node(s->metadata.instructions_start, line_number);
    if(node != NULL){
        delete_node(node, s);
    }
}

void delete_all_instructions(Session *s){
    Node *node = s->metadata.instructions_start;
    while(node != NULL){
        delete_node(node, s);
        node = node->next;
    }
}

void print_instructions(Session *s) {
    printf("instructions:\n");
    Node *node = s->metadata.instructions_start;
    while(node != NULL){
        printf("line_number = %d,  instruction = %s\n", node->line_number, node->instruction);
        node = node->next;
    }
}

void run_program(Session *s) {
    Node *node = s->metadata.instructions_start;
    while(node != NULL){
        // interprete_command(s, node->instruction);
        if(s->metadata.jump_flag != 0){
            node = find_node(s->metadata.instructions_start, s->metadata.jump_flag);
            s->metadata.jump_flag = 0;
        }
        else{
            node = node->next;
        }
    }
}

void session_end(Session *s) {
    // delete_all_instructions(s);
    free(s);
}