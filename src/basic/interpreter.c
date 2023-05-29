#include "instructions.h"
#include "interpreter.h"
#include "types.h"
#include "butils.h"

tokenE get_next_token(char* cmd, u64* pos) {
  if (isdigit(cmd[*pos])) {
    return TOK_NUMBER;
  }

  for (const tokenS* t = tokens; t->tok_id != TOK_NONE; ++t) {
    if (strncmp(&cmd[*pos], t->tok_name, strlen(t->tok_name)) == 0) {
      *pos += strlen(t->tok_name);
      return t->tok_id;
    }
  }

  /*
  if (next token is valid varname ) {
    *pos += varname length; (tego tu nie można zrobić, bo potem trzeba przeczytać tę nazwę)
    return TOK_VAR;
  }
  */

  return TOK_ERROR;
}

u64 consume_whitespaces(char* cmd, u64 pos) {
  while (cmd[pos] == ' ') {
    ++pos;
  }

  return pos;
}

u64 get_line_number(char* cmd) {
  // NO WHITESPACES AT THE BEGINNING OF THE LINE!!!
  if (cmd[0] > 57 || cmd[0] < 48) {
    return ~0;
  }
  u64 out = 0;

  for (int i=0; cmd[i] != 0; ++i) {
    out *= 10;
    out += (u8)cmd[i]-48;
  }

  return out;
}

void get_variable_name(char* dest, char* cmd, u64* pos) {
  if (!isalpha(cmd[*pos])) {
    // report invalid var name
  }
  dest[0] = cmd[*pos];
  for (int i=1; i < 7; ++i) {
    if (!isalphanum(cmd[*pos+i])) {
      // report invalid var name
    }
    dest[i] = cmd[*pos+i];
  }
}

void execute_command(void* env, char* cmd) {

  u64 ln = get_line_number(cmd);

  if (ln == ~0) {
    interprete_command(env, cmd);
    return;
  }
  // zapisz na linked list w środowisku
  // save_command(env, cmd, ln);
}

void interprete_command(void* env, char* cmd) {
  u64 pos = consume_whitespaces(cmd, 0);
  tokenE tok = get_next_token(cmd, &pos);

  switch (tok) {
    case TOK_LET:
      let_instr(env, cmd, &pos);
      break;
    
    default:
      // report invald token error
      break;
  }
}