#include "instructions.h"
#include "interpreter.h"
#include "session.h"
#include "butils.h"
#include "printf.h"
#include "types.h"
#include "mm.h"


tokenS tokens[] = {
  /* single char */
  {.tok_name=",",  .tok_id=TOK_COMMA},
  {.tok_name="\"", .tok_id=TOK_QUOTE},
  {.tok_name=";",  .tok_id=TOK_SEMICOLON},
  {.tok_name="+",  .tok_id=TOK_ADD},
  {.tok_name="-",  .tok_id=TOK_SUB},
  {.tok_name="/",  .tok_id=TOK_DIV},
  {.tok_name="*",  .tok_id=TOK_MULT},
  {.tok_name="&",  .tok_id=TOK_AND},
  {.tok_name="|",  .tok_id=TOK_OR},
  {.tok_name="(",  .tok_id=TOK_LPAREN},
  {.tok_name=")",  .tok_id=TOK_RPAREN},
  {.tok_name="#",  .tok_id=TOK_HASH},
  {.tok_name="<",  .tok_id=TOK_LT},
  {.tok_name=">",  .tok_id=TOK_GT},
  {.tok_name="=",  .tok_id=TOK_EQ},
  {.tok_name="$",  .tok_id=TOK_DOLAR},

  /* keywords */
  {.tok_name="DATA",    .tok_id=TOK_DATA},
  {.tok_name="DEF",     .tok_id=TOK_DEF},
  {.tok_name="DIM",     .tok_id=TOK_DIM},
  {.tok_name="END",     .tok_id=TOK_END},
  {.tok_name="FOR",     .tok_id=TOK_FOR},
  {.tok_name="TO",      .tok_id=TOK_TO},
  {.tok_name="STEP",    .tok_id=TOK_STEP},
  {.tok_name="ON",      .tok_id=TOK_ON},
  {.tok_name="GOSUB",   .tok_id=TOK_GOSUB},
  {.tok_name="GOTO",    .tok_id=TOK_GOTO},
  {.tok_name="IF",      .tok_id=TOK_IF},
  {.tok_name="THEN",    .tok_id=TOK_THEN},
  {.tok_name="ELSE",    .tok_id=TOK_ELSE},
  {.tok_name="INPUT",   .tok_id=TOK_INPUT},
  {.tok_name="LET",     .tok_id=TOK_LET},
  {.tok_name="NEXT",    .tok_id=TOK_NEXT},
  {.tok_name="PRINT",   .tok_id=TOK_PRINT},
  {.tok_name="READ",    .tok_id=TOK_READ},
  {.tok_name="REM",     .tok_id=TOK_REM},
  {.tok_name="RESTORE", .tok_id=TOK_RESTORE},
  {.tok_name="RETURN",  .tok_id=TOK_RETURN},
  {.tok_name="STOP",    .tok_id=TOK_STOP},

  {.tok_name="ENV",     .tok_id=TOK_ENV},
  {.tok_name="LIST",    .tok_id=TOK_LIST},
  {.tok_name="MEM",     .tok_id=TOK_MEM},
  {.tok_name="RUN",     .tok_id=TOK_RUN},

  {.tok_name="", .tok_id=TOK_NONE}
};


u64 get_line_number(char* cmd) {
  if (!isdigit(cmd[0])) {
    return ~0;
  }
  u64 out = 0;

  for (int i=0; isdigit(cmd[i]); ++i) {
    out *= 10;
    out += ((u8)cmd[i])-48;
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

u64 get_str_len(char* cmd) {
  u64 len = 0;
  for (int i=0; cmd[i] != '"' && cmd[i] != '\0'; ++i,++len) {
  }

  return len;
}


tokenE get_next_token(char* cmd, char* dest) {
  DEBUG("[*] get_next_token(%s)\n", cmd);

  if (isdigit(*cmd)) {
    /* jeszcze znaki +- */
    /* a w ogóle to czyta tylko jedną cyfrę */
    dest[0] = *cmd;
    dest[1] = '\0';
    DEBUG(" 1 token read = \"%s\"\n", dest);
    return TOK_NUMBER;
  }

  for (tokenS* t = tokens; t->tok_id != TOK_NONE; ++t) {
    if (strncmp(cmd, t->tok_name, strlen(t->tok_name)) == 0) {
      strncpy(dest, cmd, strlen(t->tok_name));
      dest[strlen(t->tok_name)] = '\0';
      DEBUG(" 2 token read = \"%s\"\n", dest);
      return t->tok_id;
    }
  }

  u8 varlen = is_valid_varname(cmd);
  if (varlen) {
    strncpy(dest, cmd, varlen);
    dest[varlen] = '\0';
    DEBUG(" 3 token read = \"%s\"\n", dest);
    return TOK_VAR;
  }

  dest[0] = '\0';
  DEBUG("[*] no token\n", 0);
  return TOK_ERROR;
}

char* consume_whitespaces(char* cmd) {
  while (*cmd == ' ') {
    ++cmd;
  }
  return cmd;
}


void execute_command(Session* env, char* cmd) {

  DEBUG("[*] execute_command(%s)\n", cmd);

  cmd = consume_whitespaces(cmd);
  u64 ln = get_line_number(cmd);

  if (ln == ~0) {
    DEBUG("   no line number\n", 0);
    interprete_command(env, cmd);
    return;
  }
  while (isdigit(*cmd)) {
    ++cmd;
  }
  DEBUG("    line number: %lu\n", ln);
  DEBUG("    saved command: %s\n", cmd);
  DEBUG("    intruction pointer: %lu\n", (u64)cmd);
  // zapisz na linked list w środowisku
  u64 instrlen = strlen(cmd);
  DEBUG("strlen cmd=%lu\n", instrlen);
  char* instrbuf = malloc(instrlen+1);
  strncpy(instrbuf, cmd, instrlen);
  instrbuf[instrlen] = '\0';
  add_instruction(env, ln, instrbuf);
}

void interprete_command(Session* env, char* cmd) {
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

    case TOK_RUN:
      run_program(env);
      break;

    case TOK_LIST:
      print_instructions(env);
      break;

    case TOK_ENV:
      print_variables(env);
      break;

    case TOK_MEM:
      print_memory_map();
      break;
    
    default:
      // report invald token error
      ERROR("[!] Invalid token: %s\n", buf);
      break;
  }
  // printf("\n");
}