#include "instructions.h"
#include "interpreter.h"
#include "types.h"
#include "butils.h"
#include "printf.h"
#include "mm.h"


tokenE map_str2tok(char* cmd, char* dest) {
  /* zwierzęca funkcja */
  if (!strncmp(cmd, "=", 1)) {
    // strncpy(dest, "=", 2);
    dest[0] = '=';
    dest[1] = '\0';
    return TOK_EQ;
  }
  if (!strncmp(cmd, ";", 1)) {
    strncpy(dest, ";", 2);
    return TOK_SEMICOLON;
  }
  if (!strncmp(cmd, ",", 1)) {
    strncpy(dest, ",", 2);
    return TOK_COMMA;
  }
  if (!strncmp(cmd, "\"", 1)) {
    strncpy(dest, "\"", 2);
    return TOK_QUOTE;
  }
  if (!strncmp(cmd, "LET", 3)) {
    strncpy(dest, "LET", 4);
    return TOK_LET;
  }
  if (!strncmp(cmd, "PRINT", 5)) {
    strncpy(dest, "PRINT", 6);
    return TOK_PRINT;
  }
  if (!strncmp(cmd, "INPUT", 5)) {
    strncpy(dest, "INPUT", 6);
    return TOK_INPUT;
  }
  
  return TOK_NONE;
}

u64 get_line_number(char* cmd) {
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

u8 is_valid_varname(char* cmd) {
  /*
   * Check if a valid variable name starts at *cmd
   *
   * Args:
   *   cmd - sting that contains a basic command
   *
   * Return:
   *   If *cmd contains first character of a valid variable name, return its length
   *   returns 0 otherwise
   */
  if (!isalpha(*cmd)) {
    // must start with a letter
    return 0;
  }
  for (int i=1; i<7; ++i) {
    if (!isalphanum(cmd[i])) {
      // if non alphanum character occurs then varname lenght is less than 7, return it
      return i;
    }
  }
  if (isalphanum(cmd[7])) {
    // varname too long
    return 0;
  }
  // valid varname of max length (eq 7)
  return 7;
}


tokenE get_next_token(char* cmd, char* dest) {
  DEBUG("[*] get_next_token(%s)\n", cmd);

  if (isdigit(*cmd)) {
    /* jeszcze znaki +- */
    /* a w ogóle to czyta tylko jedną cyfrę */
    dest[0] = *cmd;
    dest[1] = '\0';
    DEBUG(" token read = \"%s\"\n", dest);
    return TOK_NUMBER;
  }

  tokenE tok = map_str2tok(cmd, dest);
  if (tok != TOK_NONE) {
    DEBUG(" token read = \"%s\"\n", dest);
    return tok;
  }

  u8 varlen = is_valid_varname(cmd);
  if (varlen) {
    strncpy(dest, cmd, varlen);
    dest[varlen] = '\0';
    DEBUG(" token read = \"%s\"\n", dest);
    return TOK_VAR;
  }

  dest[0] = '\0';
  // DEBUG("[*] no token\n", 0);
  return TOK_ERROR;
}

char* consume_whitespaces(char* cmd) {
  while (*cmd == ' ') {
    ++cmd;
  }
  return cmd;
}


void execute_command(void* env, char* cmd) {

  DEBUG("[*] execute_command(%s)\n", cmd);

  u64 ln = get_line_number(cmd);

  if (ln == ~0) {
    DEBUG("   no line number\n", 0);
    interprete_command(env, cmd);
    return;
  }
  DEBUG("    line number: %lu\n", ln);
  // zapisz na linked list w środowisku
  // save_command(env, cmd, ln);
}

void interprete_command(void* env, char* cmd) {
  char buf[32];
  cmd = consume_whitespaces(cmd);
  tokenE tok = get_next_token(cmd, buf);
  cmd += strlen(buf);
  cmd = consume_whitespaces(cmd);

  switch (tok) {
    case TOK_LET:
      let_instr(env, cmd);
      break;

    case TOK_PRINT:
      print_instr(env, cmd);
      break;

    case TOK_INPUT:
      input_instr(env, cmd);
      break;
    
    default:
      // report invald token error
      ERROR("[!] Invalid token: %s\n", buf);
      break;
  }
  printf("\n");
}