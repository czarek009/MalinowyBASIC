#include "interpreter.h"
#include "evaluator.h"
#include "session.h"
#include "printf.h"
#include "butils.h"
#include "parser.h"

/* SESSION */
void print_structures_size(void) {
  printf("metadataS size = %ld\n", sizeof(metadataS));
  printf("variableS size = %ld\n", sizeof(variableS));
  printf("sessionS size = %ld\n", sizeof(sessionS));
}

sessionS *session_init(void) {
  DEBUG("[DEBUG] session_init()\n");
  sessionS *s = malloc(sizeof(sessionS));
  s->metadata.instructions_start = NULL;
  s->metadata.instructions_end = NULL;
  s->metadata.resume_from = NULL;
  s->metadata.return_address_stackpointer = 0;
  s->metadata.data_queue_start = 0;
  s->metadata.data_queue_end = 0;
  s->metadata.for_stackpointer = 0;
  s->metadata.variables_number = 0;
  s->metadata.functions_number = 0;
  s->metadata.error_code = SESSION_NO_ERROR;
  s->metadata.status = SESSION_STATUS_NEW;
  s->metadata.jump_flag = 0;
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
void push_data_to_queue(sessionS *s, dataQueueS data) {
  if(s->metadata.data_queue_end < data_queue_MAX_FIELD){
    s->data_queue[s->metadata.data_queue_end] = data;
    s->metadata.data_queue_end++;
    return;
  }
  ERROR("PUSH - data stack is full\n");
}

dataQueueS* read_data_from_queue(sessionS *s) {
  if(s->metadata.data_queue_end != s->metadata.data_queue_start) {
    s->metadata.data_queue_start++;
    // dataS data = s->data_queue[s->metadata.data_queue_end];
    return &(s->data_queue[s->metadata.data_queue_start-1]);
  }
  else {
    ERROR("POP - data queue is empty\n");
    return NULL;
  }
}

void print_data_queue(sessionS *s) {
  printf("Data queue:\n");
  for(u8 i = s->metadata.data_queue_start; i < s->metadata.data_queue_end; i++) {
    u8 type = s->data_queue[i].type;
    if (type == INTEGER)
      printf(" Field: %u; Value: %ld\n", i, s->data_queue[i].value.integer);
    if (type == FLOATING_POINT)
      printf(" Field: %u; Value: %f\n", i, s->data_queue[i].value.floating_point);
    if (type == STRING)
      printf(" Field: %u; Value: %s\n", i, s->data_queue[i].value.string);
  }
}

/* RETURN ADDRESS STACK */
void push_return_address_to_stack(sessionS *s, u64 address) {
  if(s->metadata.return_address_stackpointer < RET_ADDR_STACK_MAX_FIELD){
    s->return_address_stack[s->metadata.return_address_stackpointer] = address;
    s->metadata.return_address_stackpointer++;
    return;
  }
  ERROR("PUSH - return address stack is full\n");
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
  printf("Return address stack:\n");
  for(u8 i = 0; i < s->metadata.return_address_stackpointer; i++) {
    printf(" Field: %u; Value: %lu\n", i, s->return_address_stack[i]);
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
    ERROR("[SESSION ERROR] variable '%s' not found\n", name);
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
    DEBUG("[DEBUG] Replace variable value: %s\n", name);
    copy_variable(var, data, name, type);
  }
  else if (s->metadata.variables_number < VARIABLES_MAX_FIELD) {
    DEBUG("[DEBUG] Add new variable: %s\n", name);
    copy_variable(&(s->variables[s->metadata.variables_number]), data, name, type);
    s->metadata.variables_number++;
  }
  else {
    ERROR("[SESSION ERROR] cannot add variable - max number of variables reached\n");
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
  DEBUG("[DEBUG] apply_function: %s(", funname);
  if (argtype == INTEGER) {
    DEBUG("%ld) = ", arg.integer);
  }
  if (argtype == FLOATING_POINT) {
    DEBUG("%ld) = ", arg.floating_point);
  }
  if (argtype == STRING) {
    DEBUG("%ld) = ", arg.string);
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

void add_floating_point_variable(sessionS *s, double data, char *name) {
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

#define DIM_SIZE_SIZE sizeof(u64)
#define DIM_NR_SIZE sizeof(u8)
#define TYPE_SIZE sizeof(u8)

void set_array_type(void *array, u8 type) {
  u8 *type_p = (u8 *)array;
  *type_p = type;
}

u8 get_array_type(void *array) {
  u8 *type_p = (u8 *)array;
  return *type_p;
}

char* type_to_string(u8 type) {
  switch (type) {
    case INTEGER:
      return "INT";
    case FLOATING_POINT:
      return "FLOAT";
    case STRING:
      return "STRING";
    case BOOLEAN:
      return "BOOL";
    default:
      return "UNKNOWN";
  }
}

void set_array_dim_nr(void *array, u8 dim_nr) {
  u8 *dim_nr_p = ((u8 *)array) + TYPE_SIZE;
  *dim_nr_p = dim_nr;
}

u8 get_array_dim_nr(void *array) {
  u8 *dim_nr_p = ((u8 *)array) + TYPE_SIZE;
  return *dim_nr_p;
}

void set_array_dims(void *array, u8 dim_nr, u64 *dims) {
  u64 *dims_p = (u64 *)(((u8 *)array) + TYPE_SIZE + DIM_NR_SIZE);
  for(u8 i = 0; i < dim_nr; i++) {
    dims_p[i] = dims[i];
  }
}

u64 *get_array_dims_ptr(void *array) {
  return (u64 *)(((u8 *)array) + TYPE_SIZE + DIM_NR_SIZE);
}

u64 get_array_elements_nr(u8 dim_nr, u64 *dims) {
  u64 elem_nr = 1;
  for(u8 i = 0; i < dim_nr; i++) {
    elem_nr *= dims[i];
  }
  return elem_nr;
}

u64 get_array_element_size(u8 type) {
  if (type == INTEGER) return sizeof(s64);
  else if (type == FLOATING_POINT) return sizeof(double);
  else if (type == STRING) return sizeof(char *);
  else return 0;
}

u64 get_array_size(u8 dim_nr, u64 *dims, u8 type) {
  u64 elements_nr = get_array_elements_nr(dim_nr, dims);
  u64 element_size = get_array_element_size(type);
  return elements_nr * element_size;
}

u64 get_array_header_size(u8 dim_nr) {
  return TYPE_SIZE + DIM_NR_SIZE + ((u64)dim_nr * DIM_SIZE_SIZE);
}

void add_array_variable(sessionS *s, char *name, u8 dim_nr, u64 *dims, u8 type) {
  u64 header_size = get_array_header_size(dim_nr);
  u64 array_size = get_array_size(dim_nr, dims, type);
  u64 size = header_size + array_size;

  void *array = malloc(size);
  array = memset(array, 0, size);

  set_array_type(array, type);
  set_array_dim_nr(array, dim_nr);
  set_array_dims(array, dim_nr, dims);

  variableDataU var_data;
  var_data.array = array;
  check_and_add_variable(s, var_data, name, ARRAY);
}

sessionErrorCodeE check_array_parameters(sessionS *s, char *name, u8 parsed_type, u8 parsed_dim_nr, u64 *parsed_dims) {
  variableDataU var_data;
  u8 var_type = get_variable_value(s, name, &var_data);
  if (var_type != ARRAY) {
    ERROR("[SESSION ERROR] variable %s is not an ARRAY\n", name);
    return SESSION_INVALID_VAR_NAME;
  }
  u8 type = get_array_type(var_data.array);
  if (type != parsed_type) {
    ERROR("[INSTRUCTION ERROR] Array %s is different type than parsed\n", name);
    return SESSION_PARSING_ERROR;
  }
  u8 dim_nr = get_array_dim_nr(var_data.array);
  if(dim_nr != parsed_dim_nr){
    ERROR("[INSTRUCTION ERROR] Wrong dimentions to array %s, given dim=%d, real dim=%d\n", name, parsed_dim_nr, dim_nr);
    return SESSION_PARSING_ERROR;
  }
  u64 *dims = get_array_dims_ptr(var_data.array);
  for(u8 i = 0; i < dim_nr; i++) {
    if(dims[i] <= parsed_dims[i]){
      ERROR("[PARSING ERROR] Index %u out of range (dim size = %d)\n", parsed_dims[i], dims[i]);
      return SESSION_PARSING_ERROR;
    }
  }
  return SESSION_NO_ERROR;
}

u64 array_offset(u8 dim_nr, u64* dims, u64* idxs) {
  u64 offset = 0;
  u64 m = 1;
  for(u8 i = 0; i < dim_nr; i++) {
    m = 1;
    for(u8 j = i + 1; j < dim_nr; j++) {
      m *= dims[j];
    }
    offset += (idxs[i] * m);
  }
  return offset;
}

sessionErrorCodeE update_array(sessionS *s, char *name, variableDataU value, u64 *idxs) {
  variableS *var = get_variable_ptr(s, name);
  if (var == NULL) return SESSION_INVALID_VAR_NAME;
  void *array = var->data.array;
  u8 type = get_array_type(array);
  u8 dim_nr = get_array_dim_nr(array);
  u64 *dims = get_array_dims_ptr(array);
  u64 offset = array_offset(dim_nr, dims, idxs);
  u8 *header = (u8 *)array;
  u64 header_size = get_array_header_size(dim_nr);
  switch(type){
    case INTEGER:
      {
        s64 *elements = (s64 *)(header + header_size);
        elements[offset] = value.integer;
      }
      break;
    case FLOATING_POINT:
      {
        double *elements = (double *)(header + header_size);
        elements[offset] = value.floating_point;
      }
      break;
    case STRING:
      {
        char **elements = (char **)(header + header_size);
        if(elements[offset] != NULL) free(elements[offset]);
        elements[offset] = value.string;
      }
      break;
    default:
      return SESSION_PARSING_ERROR;
  }
  return SESSION_NO_ERROR;
}

sessionErrorCodeE get_array_element(sessionS *s, char *name, u64 *idxs, variableDataU *data) {
  variableDataU array_data = {0};
  u8 var_type = get_variable_value(s, name, &array_data);
  if (var_type != ARRAY) {
    ERROR("[SESSION ERROR] variable %s is not an ARRAY\n", name);
    return SESSION_INVALID_VAR_NAME;
  }
  void *array = array_data.array;
  u8 type = get_array_type(array);
  u8 dim_nr = get_array_dim_nr(array);
  u64 *dims = get_array_dims_ptr(array);
  u64 offset = array_offset(dim_nr, dims, idxs);
  u8 *header = (u8 *)array;
  u64 header_size = get_array_header_size(dim_nr);
  switch(type){
    case INTEGER:
      {
        s64 *elements = (s64 *)(header + header_size);
        data->integer = elements[offset];
      }
      break;
    case FLOATING_POINT:
      {
        double *elements = (double *)(header + header_size);
        data->floating_point = elements[offset];
      }
      break;
    case STRING:
      {
        char **elements = (char **)(header + header_size);
        char *str = elements[offset];
        size_t size = strlen(str);
        char *out_str = malloc(size + 1);
        memcpy(out_str, str, size);
        out_str[size] = '\0';
        data->string = out_str;
      }
      break;
    default:
      return SESSION_PARSING_ERROR;
  }
  return SESSION_NO_ERROR;
}

void print_array_parameters(void *array, char *name) {
  u8 dim_nr = get_array_dim_nr(array);
  printf("print array %s\n", name);
  printf("type: %u\n", get_array_type(array));
  printf("nr of dimentions: %u\n", dim_nr);
  printf("dimentions: ");
  u64 *dims = get_array_dims_ptr(array);
  for(u8 i = 0; i < dim_nr; i++) {
    printf("%ld; ", dims[i]);
  }
  printf("\n");
}

void delete_array(void *array) {
  if(array == NULL) return;
  u8 type = get_array_type(array);
  if(type != STRING){
    free(array);
    return;
  }
  u8 dim_nr = get_array_dim_nr(array);
  u64 *dims = get_array_dims_ptr(array);
  u8 *header = (u8 *)array;
  u64 header_size = get_array_header_size(dim_nr);
  char **elements = (char **)(header + header_size);
  u64 array_elements_nr = get_array_elements_nr(dim_nr, dims);
  for(u64 i = 0; i < array_elements_nr; i++) {
    char *str = elements[i];
    if(str != NULL){
      free(str);
    }
  }
  free(array);
}

void delete_all_variables(sessionS *s) {
  variableS var;
  for(u8 i = 0; i < s->metadata.variables_number; i++){
    var = s->variables[i];
    if (var.type == STRING) {
      free(var.data.string);
    }
    if(var.type == ARRAY) {
      delete_array(var.data.array);
    }
  }
}

void print_variables(sessionS *s) {
  printf("Variables:\n");
  variableS var;
  for(u8 i = 0; i < s->metadata.variables_number; i++){
    var = s->variables[i];
    switch(var.type) {
      case INTEGER:
        printf(" INT %s = %ld\n", var.name, var.data.integer);
        break;
      case FLOATING_POINT:
        printf(" FLOAT %s = %f\n", var.name, var.data.floating_point);
        break;
      case BOOLEAN:
        if (var.data.boolean) {
          printf(" BOOL %s = TRUE\n", var.name);
        } else {
          printf(" BOOL %s = FALSE\n", var.name);
        }
        break;
      case STRING:
        printf(" STRING %s = \"%s\"\n", var.name, var.data.string);
        break;
      case ARRAY:
        u8 arr_type = get_array_type(var.data.array);
        printf(" %s %s[] = ...\n", type_to_string(arr_type), var.name);
        break;
      default:
        printf(" NOT SUPPORTED!\n");
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
    DEBUG("[DEBUG] Instruction not found\n");
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
  DEBUG_MEM("DEBUG PRINT 2: before free(node->instruction)\n");
  DEBUG_MEM("instructions: %s\n", node->instruction);
  free(node->instruction);
  DEBUG_MEM("DEBUG PRINT 3: after free(node->instruction)\n");
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

    if (s->metadata.status == SESSION_STATUS_FINISHED) {
      s->metadata.resume_from = NULL;
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

u64 find_next(sessionS *s, u64 ln) {
  s32 counter = 0;
  instructionS *node = s->metadata.instructions_start;
  while(node != NULL){
    if (node->line_number == ln) {
      break;
    }
    node = node->next;
  }

  while(node != NULL) {
    char* aux = node->instruction;
    char buf[64];
    tokenE tok = get_next_token(&aux, buf, TOK_ANY);
    if (tok == TOK_FOR) {
      counter++;
    }
    if (tok == TOK_NEXT) {
      counter--;
    }
    if (counter == 0) {
      return node->line_number;
    }

    node = node->next;
  }

  return 0;
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

instructionS *get_next_instruction(sessionS *s, u64 line_number) {
  instructionS *node = find_instruction(s->metadata.instructions_start, line_number);
  if(node == NULL) return node;
  return node->next;
}

void print_functions(sessionS* s) {
  for (u64 i = 0; i < s->metadata.functions_number; ++i) {
    printf(" %s(%s) = %s\n",
          s->functions[i].funname,
          s->functions[i].argname,
          s->functions[i].body);
  }
}

void print_session_info(sessionS* s) {
  printf("SESSION DATA:\n");
  print_variables(s);
  print_return_address_stack(s);
  print_data_queue(s);
  printf("Status: %d\n", s->metadata.status);
  printf("Error code: %d\n", s->metadata.error_code);
  printf("Jump flag: %lu\n", s->metadata.jump_flag);
  printf("Resume from: %lu\n", s->metadata.resume_from->line_number);
  printf("Functions number: %lu\n", s->metadata.functions_number);
  print_functions(s);
}


void session_end(sessionS *s) {
  DEBUG("[DEBUG] session_end()\n");
  delete_all_instructions(s);
  delete_all_variables(s);
  free(s);
}