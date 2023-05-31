#include "instructions.h"
#include "interpreter.h"
#include "types.h"
#include "butils.h"
#include "printf.h"
#include "mm.h"
#include <stddef.h>


tokenE map_str2tok(char* cmd, u64 pos, char* dest) {
  if (!strncmp(&cmd[pos], "=", 1)) {
    strncpy(dest, "=", 2);
    return TOK_EQ;
  }
  if (!strncmp(&cmd[pos], "LET", 3)) {
    strncpy(dest, "LET", 4);
    return TOK_LET;
  }
  
  return TOK_NONE;
}

u64 get_toklen(char* cmd, u64 pos) {
  return 0;
}

u8 is_valid_varname(char* cmd, u64 pos) {
  /*
   * Check if a valid variable name starts at cmd[pos]
   *
   * Args:
   *   cmd - sting that contains a basic command
   *   pos - index from where we are reading right now
   *
   * Return:
   *   If cmd[pos] contains first character of a valid variable name, return its length
   *   returns 0 otherwise
   */
  if (!isalpha(cmd[pos])) {
    // must start with a letter
    return 0;
  }
  for (int i=1; i<7; ++i) {
    if (!isalphanum(cmd[pos+i])) {
      // if non alphanum character occurs then varname lenght is less than 7, return it
      return i;
    }
  }
  if (isalphanum(cmd[pos+7])) {
    // varname too long
    return 0;
  }
  // valid varname of max length (eq 7)
  return 7;
}

tokenE get_next_token(char* cmd, u64 pos, char* dest) {
  DEBUG("[*] get_next_token(%s)\n", &cmd[pos]);
  if (isdigit(cmd[pos])) {
    /* jeszcze znaki +- */
    dest[0] = cmd[pos];
    dest[1] = '\0';
    DEBUG(" token read=%s\n", dest);
    return TOK_NUMBER;
  }

  tokenE t = map_str2tok(cmd, pos, dest);
  if (t) {
    DEBUG(" token read=%s\n", dest);
    return t;
  }
  // for (const tokenS* t = tokens; t->tok_id != TOK_NONE; ++t) {
  //   DEBUG(" checking token %lu %s...", t->tok_name, t->tok_name);
  //   if (strncmp(&cmd[pos], t->tok_name, strlen(t->tok_name)) == 0) {
  //     strncpy(dest, &cmd[pos], strlen(t->tok_name));
  //     dest[strlen(t->tok_name)] = '\n';
  //     DEBUG(" token read=%s\n", dest);
  //     return t->tok_id;
  //   }
  //   DEBUG(" no match\n",0);
  // }

  u8 varlen = is_valid_varname(cmd, pos);
  if (varlen) {
    strncpy(dest, &cmd[pos], varlen);
    dest[varlen] = '\0';
    DEBUG(" token read=%s\n", dest);
    return TOK_VAR;
  }

  dest[0] = '\0';
  DEBUG(" token read=%s\n", dest);
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
  /*
   * This function saves variable name that starts at cmd[pos] to dest
   * Important: assume that we have valid varname starting at cmd[pos]
   *
   * Args:
   *   dest - pointer to allocated space where varname will be stored
   *   cmd  - sting that contains a basic command
   *   pos  - index from where we are reading right now
   */
  u8 varlen = is_valid_varname(cmd, *pos);
  for (int i=0; i < varlen; ++i) {
    dest[i] = cmd[*pos+i];
  }
  *pos += varlen;
}

void execute_command(void* env, char* cmd) {

  DEBUG("[*] execute_command()\n"
        " -cmd: %s\n",
        cmd);

  u64 ln = get_line_number(cmd);

  if (ln == ~0) {
    DEBUG("[*] No line number\n", 0);
    interprete_command(env, cmd);
    return;
  }
  DEBUG("[*] Line number: %lu\n", ln);
  // zapisz na linked list w środowisku
  // save_command(env, cmd, ln);
}

void interprete_command(void* env, char* cmd) {
  char buf[32];
  u64 pos = consume_whitespaces(cmd, 0);
  tokenE tok = get_next_token(cmd, pos, buf);
  pos += strlen(buf);

  switch (tok) {
    case TOK_LET:
      let_instr(env, cmd, pos);
      break;
    
    default:
      // report invald token error
      ERROR("[!] Invalid token: %s\n", buf);
      break;
  }
  printf("\n");
}