#include "interpreter.h"
#include "evaluator.h"
#include "session.h"
#include "printf.h"
#include "butils.h"

/* SESSION */
void print_structures_size(void) {
  printf("metadataS size = %ld\n", sizeof(metadataS));
  printf("variableS size = %ld\n", sizeof(variableS));
  printf("sessionS size = %ld\n", sizeof(sessionS));
}

sessionS *session_init(void) {
  sessionS *s = malloc(sizeof(sessionS));
  s->metadata.instructions_start = NULL;
  s->metadata.instructions_end = NULL;
  s->metadata.resume_from = NULL;
  s->metadata.return_address_stackpointer = 0;
  s->metadata.data_stackpointer = 0;
  s->metadata.for_stackpointer = 0;
  s->metadata.variables_number = 0;
  s->metadata.functions_number = 0;
  s->metadata.error_code = SESSION_NO_ERROR;
  s->metadata.status = SESSION_STATUS_NEW;
  s->metadata.jump_flag = 0;
  DEBUG("Init session\n");
  return s;
}

sessionStatusE get_session_status(sessionS *s) {
  return s->metadata.status;
}

void set_session_status(sessionS *s, sessionStatusE status) {
  s->metadata.status = status;
}

void set_jump_flag(sessionS *s, u64 line_number) {
  s->metadata.jump_flag = line_number;
}

/* DATA STACK */
void push_data_to_stack(sessionS *s, s32 data) {
  if(s->metadata.data_stackpointer < DATA_STACK_MAX_FIELD){
    s->data_stack[s->metadata.data_stackpointer] = data;
    s->metadata.data_stackpointer++;
    return;
  }
  DEBUG("PUSH - data stack is full\n");
}

s32 pop_data_from_stack(sessionS *s) {
  if(s->metadata.data_stackpointer > 0) {
    s->metadata.data_stackpointer--;
    s32 data = s->data_stack[s->metadata.data_stackpointer];
    return data;
  }
  else {
    DEBUG("POP - data stack is empty\n");
    return 0;
  }
}

void print_data_stack(sessionS *s) {
  printf("data stack:\n");
  for(u8 i = 0; i < s->metadata.data_stackpointer; i++) {
    printf("field: %u, value: %d\n", i, s->data_stack[i]);
  }
}

/* RETURN ADDRESS STACK */
void push_return_address_to_stack(sessionS *s, u64 address) {
  if(s->metadata.return_address_stackpointer < RET_ADDR_STACK_MAX_FIELD){
    s->return_address_stack[s->metadata.return_address_stackpointer] = address;
    s->metadata.return_address_stackpointer++;
    return;
  }
  DEBUG("PUSH - return address stack is full\n");
}

u64 pop_return_address_from_stack(sessionS *s) {
  if(s->metadata.return_address_stackpointer > 0) {
    s->metadata.return_address_stackpointer--;
    u64 data = s->return_address_stack[s->metadata.return_address_stackpointer];
    return data;
  }
  else {
    ERROR("[SESSION ERROR] Return address stack is empty\n");
    return ~0;
  }
}

void print_return_address_stack(sessionS *s) {
  printf("return address stack:\n");
  for(u8 i = 0; i < s->metadata.return_address_stackpointer; i++) {
    printf("field: %u, value: %lu\n", i, s->return_address_stack[i]);
  }
}

/* VARIABLE */
bool compare_name(char *variable_name, char* name) {
  u8 i = 0;
  for(; (i < VARIABLE_NAME_SIZE && variable_name[i] != '\0' && name[i] != '\0'); i++){
    if((variable_name[i] != name[i])){
      return false;
    }
  }
  if(i == VARIABLE_NAME_SIZE || (variable_name[i] == '\0' && name[i] == '\0')){
    return true;
  }
  return false;
}

variableS *get_variable_ptr(sessionS *s, char* name) {
  variableS *variables = s->variables;
  u8 variables_number = s->metadata.variables_number;
  for(u8 i = 0; i < variables_number; i++, variables++) {
    if(compare_name(variables->name, name)) {
      return variables;
    }
  }
  return NULL;
}

u8 get_variable_value(sessionS *s, char* name, variableDataU *var_data) {
  variableS *var = get_variable_ptr(s, name);
  if (var == NULL) {
    DEBUG("variable '%s' not found\n", name);
    return NOT_FOUND;
  }
  *var_data = var->data;
  return var->type;
}

void copy_variable(variableS *var, variableDataU data, char *name, u8 type) {
  u8 i;
  for(i = 0; (name[i] != '\0' && i < VARIABLE_NAME_SIZE); i++) {
    var->name[i] = name[i];
  }
  if(i < VARIABLE_NAME_SIZE){
    var->name[i] = '\0';
  }
  var->data = data;
  var->type = type;
}

void check_and_add_variable(sessionS *s, variableDataU data, char *name, u8 type) {
  variableS *var = get_variable_ptr(s, name);
  if(var != NULL){
    DEBUG("Replace variable value\n");
    copy_variable(var, data, name, type);
  }
  else if (s->metadata.variables_number < VARIABLES_MAX_FIELD) {
    DEBUG("Add new variable\n");
    copy_variable(&(s->variables[s->metadata.variables_number]), data, name, type);
    s->metadata.variables_number++;
  }
  else {
    DEBUG("Cannot add variable - max number of variables reached\n");
  }
}

void add_variable(sessionS *s, variableDataU var_data, char *name, u8 type){
  switch(type) {
    case INTEGER:
    case FLOATING_POINT:
    case BOOLEAN:
    case STRING:
      check_and_add_variable(s, var_data, name, type);
      break;
    default:
      ERROR("[SESSION ERROR] add_variable: unknown type\n");
      break;
  }
}

void add_function(sessionS *s, char* funname, char* argname, char* body) {
  u8 fnum = s->metadata.functions_number;
  for (int i = 0; i < fnum; ++i) {
    if ( !strcmp(funname, s->functions[i].funname) ) {
      free(s->functions[i].body);
      char* new_body = malloc(strlen(body)+1);
      strncpy(new_body, body, strlen(body)+1);

      strncpy(s->functions[i].funname, funname, 7);
      strncpy(s->functions[i].argname, argname, 7);
      s->functions[i].body = new_body;

      return;
    }
  }

  char* new_body = malloc(strlen(body)+1);
  strncpy(new_body, body, strlen(body)+1);

  strncpy(s->functions[fnum].funname, funname, 7);
  strncpy(s->functions[fnum].argname, argname, 7);
  s->functions[fnum].body = new_body;

  s->metadata.functions_number++;
}

static functionS* find_function(sessionS* s, char* funname) {

  for (int i = 0; i < s->metadata.functions_number; ++i) {
    if (strcmp(funname, s->functions[i].funname) == 0) {
      return &(s->functions[i]);
    }
  }
  return NULL;
}

u8 apply_function(sessionS *s, char* funname, variableDataU arg, u8 argtype, variableDataU* result) {
  DEBUG("[DEBUG SESSION] apply_function: %s(", funname);
  if (argtype == INTEGER) {
    DEBUG("%ld) = ", arg.integer);
  }

  functionS* fun = NULL;
  u8 saved_var_type = NOT_FOUND;
  u8 out_type = NOT_FOUND;
  variableDataU saved_var_value = {0};
  char* new_body = NULL;

  fun = find_function(s, funname);
  if (fun == NULL) {
    ERROR("[SESSION ERROR] Function '%s' not found\n", funname);
    return NOT_FOUND;
  }

  saved_var_type = get_variable_value(s, fun->argname, &saved_var_value);
  add_variable(s, arg, fun->argname, argtype);

  new_body = malloc(strlen(fun->body)+3);
  char* aux = new_body;
  strncpy(new_body+1, fun->body, strlen(fun->body));
  new_body[0] = '(';
  new_body[strlen(fun->body)+3-2] = ')';
  new_body[strlen(fun->body)+3-1] = '\0';
  out_type = eval_expr(s, &new_body, result);

  if (out_type == INTEGER) {
    DEBUG("%ld\n", result->integer);
  } else {
    DEBUG(" WRONG OUT TYPE! %u\n", (u32)out_type);
  }

  if (saved_var_type < 252) {
    add_variable(s, saved_var_value, fun->argname, saved_var_type);
  }

  free(aux);

  return out_type;
}

void add_integer_variable(sessionS *s, s64 data, char *name) {
  variableDataU var_data;
  var_data.integer = data;
  check_and_add_variable(s, var_data, name, INTEGER);
}

void add_floating_point_variable(sessionS *s, float data, char *name) {
  variableDataU var_data;
  var_data.floating_point = data;
  check_and_add_variable(s, var_data, name, FLOATING_POINT);
}

void add_boolean_variable(sessionS *s, bool data, char *name) {
  variableDataU var_data;
  var_data.boolean = data;
  check_and_add_variable(s, var_data, name, BOOLEAN);
}

void add_string_variable(sessionS *s, char *data, char *name) {
  variableDataU var_data;
  size_t size = strlen(data);
  char *str = malloc(size + 1);
  memcpy(str, data, size);
  str[size] = '\0';
  var_data.string = str;
  check_and_add_variable(s, var_data, name, STRING);
}

void delete_all_variables(sessionS *s) {
  variableS var;
  for(u8 i = 0; i < s->metadata.variables_number; i++){
    var = s->variables[i];
    if (var.type == STRING) {
      free(var.data.string);
    }
  }
}

void print_variables(sessionS *s) {
  printf("variables:\n");
  variableS var;
  for(u8 i = 0; i < s->metadata.variables_number; i++){
    var = s->variables[i];
    switch(var.type) {
      case INTEGER:
        printf("int %s = %ld\n", var.name, var.data.integer);
        break;
      case FLOATING_POINT:
        printf("float %s = %f\n", var.name, var.data.floating_point);
        break;
      case BOOLEAN:
        if (var.data.boolean) {
          printf("bool %s = true\n", var.name);
        } else {
          printf("bool %s = false\n", var.name);
        }
        break;
      case STRING:
        printf("string %s = %s\n", var.name, var.data.string);
        break;
      default:
        printf("not supporting yet\n");
        break;
    }
  }
}


/* INSTRUCTIONS */
instructionS *create_node(instructionS *previous, instructionS *next, u64 line_number, char *instruction) {
  instructionS *new = malloc(sizeof(instructionS));
  new->previous = previous;
  new->next = next;
  new->line_number = line_number;
  new->instruction = instruction;
  return new;
}

instructionS *find_place(instructionS *head, u64 line_number) {
  instructionS *node = head;
  while((node->next != NULL) && (node->next->line_number < line_number)) {
      node = node->next;
  }
  return node;
}

void add_instruction(sessionS *s, u64 line_number, char *instruction) {
  if(s->metadata.instructions_start == NULL){
    instructionS *new_node = create_node(NULL, NULL, line_number, instruction);
    s->metadata.instructions_start = new_node;
    s->metadata.instructions_end = new_node;
  }
  else if(line_number < s->metadata.instructions_start->line_number) {
    instructionS *new_node = create_node(NULL, s->metadata.instructions_start, line_number, instruction);
    s->metadata.instructions_start->previous = new_node;
    s->metadata.instructions_start = new_node;
  }
  else if(line_number > s->metadata.instructions_end->line_number) {
    instructionS *new_node = create_node(s->metadata.instructions_end, NULL, line_number, instruction);
    s->metadata.instructions_end->next = new_node;
    s->metadata.instructions_end = new_node;

  }
  else {
    instructionS *place = find_place(s->metadata.instructions_start, line_number);
    if(place->line_number == line_number){
      place->instruction = instruction;
    }
    else if(place->next->line_number == line_number){
      place->next->instruction = instruction;
    }
    else {
      instructionS *next = place->next;
      instructionS *new_node = create_node(place, next, line_number, instruction);
      place->next = new_node;
      next->previous = new_node;
    }
  }
}

instructionS *find_instruction(instructionS *head, u64 line_number) {
  instructionS *node = head;
  while((node->line_number != line_number) && (node != NULL)) {
    node = node->next;
  }
  if(node == NULL){
    DEBUG("Instruction not found\n");
  }
  return node;
}

void delete_node(instructionS *node, sessionS *s) {
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

void delete_single_instruction(sessionS *s, u64 line_number) {
  instructionS *node = find_instruction(s->metadata.instructions_start, line_number);
  if(node != NULL){
    delete_node(node, s);
  }
}

void delete_all_instructions(sessionS *s){
  instructionS *node = s->metadata.instructions_start;
  while(node != NULL){
    delete_node(node, s);
    node = node->next;
  }
}

void print_instructions(sessionS *s) {
  instructionS *node = s->metadata.instructions_start;
  while(node != NULL){
    printf(" %ld %s\n", node->line_number, node->instruction);
    node = node->next;
  }
}

sessionErrorCodeE run_program(sessionS *s) {
  sessionErrorCodeE out = SESSION_NO_ERROR;
  instructionS *node = NULL;

  if (s->metadata.resume_from == NULL) {
    node = s->metadata.instructions_start;
  } else {
    node = s->metadata.resume_from->next;
    s->metadata.resume_from = NULL;
  }

  s->metadata.status = SESSION_STATUS_RUNNING;
  while(node != NULL){
    out = interpreter_execute_command(s, node->instruction, node->line_number);

    if (out != SESSION_NO_ERROR) {
      ERROR("[SESSION ERROR] Program execution failed\n");
      s->metadata.error_code = out;
      s->metadata.status = SESSION_STATUS_ERROR;
      return out;
    }

    if (s->metadata.status == SESSION_STATUS_STOPPED) {
      s->metadata.resume_from = node;
      return SESSION_NO_ERROR;
    }

    if(s->metadata.jump_flag != 0) {
      node = find_instruction(s->metadata.instructions_start, s->metadata.jump_flag);
      s->metadata.jump_flag = 0;
    }
    else{
      node = node->next;
    }
  }

  s->metadata.error_code = SESSION_NO_ERROR;
  s->metadata.status = SESSION_STATUS_FINISHED;
  return SESSION_NO_ERROR;
}

u64 get_next_instr_line(sessionS *s, u64 ln) {
  instructionS *node = s->metadata.instructions_start;
  while(node != NULL){
    if (node->line_number == ln) {
      if (node->next != NULL) {
        return node->next->line_number;
      }
      return 0;
    }
    node = node->next;
  }
  return 0;
}

void print_functions(sessionS* s) {
  for (u64 i = 0; i < s->metadata.functions_number; ++i) {
    printf("  funname: '%s'  argname: '%s'  body: '%s'\n",
          s->functions[i].funname,
          s->functions[i].argname,
          s->functions[i].body);
  }
}

void print_session_info(sessionS* s) {
  printf("SESSION DATA:\n");
  print_variables(s);
  print_return_address_stack(s);
  print_data_stack(s);
  printf("Status: %d\n", s->metadata.status);
  printf("Error code: %d\n", s->metadata.error_code);
  printf("Jump flag: %lu\n", s->metadata.jump_flag);
  printf("Resume from: %lu\n", s->metadata.resume_from->line_number);
  printf("Functions number: %lu\n", s->metadata.functions_number);
  print_functions(s);
}


void session_end(sessionS *s) {
  delete_all_instructions(s);
  delete_all_variables(s);
  free(s);
  DEBUG("sessionS end\n");
}