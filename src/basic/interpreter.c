#include "instructions.h"
#include "interpreter.h"
#include "session.h"
#include "butils.h"
#include "printf.h"
#include "types.h"
#include "mm.h"
#include "debug.h"


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


char* get_tokname(tokenE tok) {
  for (tokenS* t = tokens; t->tok_id != TOK_NONE; ++t) {
    if (t->tok_id == tok) {
      return t->tok_name;
    }
  }
  return NULL;
}

void report_error(tokenE expected_token, char* found, char* cmd) {
  ERROR(" ERROR: expected token '%s' got '%s'\n", get_tokname(expected_token), found);
  ERROR("        %s\n", cmd);
}

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

u64 get_str_len(char* cmd) {
  u64 len = 0;
  for (int i=0; cmd[i] != '"' && cmd[i] != '\0'; ++i,++len) {
  }

  return len;
}

void get_const_str(char** cmd_p, char* dest) {
  char* cmd = *cmd_p;
  for (int i=0; cmd[i] != '"' && cmd[i] != '\0'; ++i) {
    dest[i] = cmd[i];
    *cmd_p += 1;
  }

  return;
}


tokenE get_next_token(char** cmd_p, char* dest, tokenE expected_token) {
  *cmd_p = consume_whitespaces(*cmd_p);
  char* cmd = *cmd_p;
  DEBUG("[*] get_next_token(%s)\n", cmd);

  /* EOL */
  if (*cmd == '\0') {
    dest[0] = '\0';
    if (expected_token != TOK_ANY && expected_token != TOK_NONE) {
      ERROR("[*] ERROR: unexpected eol\n", 0);
      return TOK_ERROR;
    }
    return TOK_NONE;
  }

  /* NUMBER */
  if (isdigit(*cmd) || *cmd == '-' || *cmd == '+') {
    dest[0] = cmd[0];
    int i = 1;
    for (; isdigit(cmd[i]) || cmd[i] == '.'; ++i) {
      dest[i] = cmd[i];
    }
    dest[i] = '\0';

    if (isin(cmd[i], " +-*/=<>") || cmd[i] == '\0') {
      DEBUG(" 1 token read = \"%s\"\n", dest);
      *cmd_p += i;
      if (expected_token != TOK_ANY && expected_token != TOK_NUMBER) {
        report_error(expected_token, "number", cmd);
        return TOK_ERROR;
      }
      return TOK_NUMBER;
    }

    ERROR(" this should be a number, bud idk what this is '%c'\n", cmd[i]);
    dest[0] = '\0';
    return TOK_ERROR;
  }

  /* KEYWORD OR OPERATOR */
  for (tokenS* t = tokens; t->tok_id != TOK_NONE; ++t) {
    if (strncmp(cmd, t->tok_name, strlen(t->tok_name)) == 0) {
      size_t tok_len = strlen(t->tok_name);
      strncpy(dest, cmd, tok_len);
      dest[tok_len] = '\0';
      *cmd_p += tok_len;
      DEBUG(" 2 token read = \"%s\"\n", dest);
      if (expected_token != TOK_ANY && expected_token != t->tok_id) {
        report_error(expected_token, get_tokname(t->tok_id), cmd);
        return TOK_ERROR;
      }
      return t->tok_id;
    }
  }

  /* VARIABLE */
  u8 varlen = is_valid_varname(cmd);
  if (varlen) {
    strncpy(dest, cmd, varlen);
    dest[varlen] = '\0';
    *cmd_p += varlen;
    DEBUG(" 3 token read = \"%s\"\n", dest);
    if (expected_token != TOK_ANY && expected_token != TOK_VAR) {
      report_error(expected_token, "variable", cmd);
      return TOK_ERROR;
    }
    return TOK_VAR;
  }

  /* UNKNOWN TOKEN */
  dest[0] = '\0';
  ERROR("[*] ERROR: unknown token: %s\n", cmd);
  return TOK_ERROR;
}

char* consume_whitespaces(char* cmd) {
  while (*cmd == ' ') {
    ++cmd;
  }
  return cmd;
}


void execute_command(sessionS* env, char* cmd) {

  DEBUG("[*] execute_command(%s)\n", cmd);

  cmd = consume_whitespaces(cmd);
  u64 ln = get_line_number(cmd);

  if (ln == ~0) {
    DEBUG("   no line number\n", 0);
    interprete_command(env, cmd, 0);
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

void interprete_command(sessionS* env, char* cmd, u64 line_number) {
  char buf[32];
  // cmd = consume_whitespaces(cmd);
  tokenE tok = get_next_token(&cmd, buf, TOK_ANY);
  // cmd += strlen(buf);
  // cmd = consume_whitespaces(cmd);

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

    case TOK_GOTO:
      goto_instr(env, cmd);
      break;

    case TOK_GOSUB:
      gosub_instr(env, cmd, line_number);
      break;

    case TOK_RETURN:
      return_instr(env, cmd);
      break;
    
    default:
      // report invald token error
      ERROR("[!] Invalid token: %s\n", buf);
      break;
  }
  // printf("\n");
}