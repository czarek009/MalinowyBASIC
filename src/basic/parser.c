#include "instructions.h"
#include "parser.h"
#include "interpreter.h"
#include "session.h"
#include "butils.h"
#include "printf.h"
#include "types.h"
#include "mm.h"
#include "debug.h"


/* GLOBAL VARIABLES */
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
  {.tok_name="CONT",    .tok_id=TOK_CONT},
  {.tok_name="STOP",    .tok_id=TOK_STOP},

  {.tok_name="SINFO",   .tok_id=TOK_SINFO},
  {.tok_name="ENV",     .tok_id=TOK_ENV},
  {.tok_name="LIST",    .tok_id=TOK_LIST},
  {.tok_name="MEM",     .tok_id=TOK_MEM},
  {.tok_name="RUN",     .tok_id=TOK_RUN},

  {.tok_name="", .tok_id=TOK_NONE}
};


/* PRIVATE FUNCTIONS DECLARATIONS */
static char* consume_whitespaces(char* cmd);
static char* get_tokname(tokenE tok);


/* PUBLIC FUNCTIONS DEFINITIONS */
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
    bool isFloat = false;
    dest[0] = cmd[0];
    int i = 1;
    for (; isdigit(cmd[i]) || cmd[i] == '.'; ++i) {
      if (cmd[i] == '.') {
        if (isFloat) {
          ERROR(" PARSING ERROR: invalid float: %s\n", cmd);
          dest[0] = '\0';
          return TOK_ERROR;
        }
        isFloat = true;
      }
      dest[i] = cmd[i];
    }
    if (cmd[i-1] == '.') {
      ERROR(" PARSING ERROR: invalid float: %s\n", cmd);
      dest[0] = '\0';
      return TOK_ERROR;
    }
    dest[i] = '\0';

    if (isin(cmd[i], " +-*/=<>") || cmd[i] == '\0') {
      DEBUG(" 1 token read = \"%s\"\n", dest);
      *cmd_p += i;
      if (expected_token != TOK_ANY && expected_token != TOK_NUMBER) {
        report_error(get_tokname(expected_token), "number", cmd);
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
        report_error(get_tokname(expected_token), get_tokname(t->tok_id), cmd);
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
      report_error(get_tokname(expected_token), "variable", cmd);
      return TOK_ERROR;
    }
    return TOK_VAR;
  }

  /* UNKNOWN TOKEN */
  dest[0] = '\0';
  ERROR("[*] ERROR: unknown token: %s\n", cmd);
  return TOK_ERROR;
}

void report_error(char* expected, char* found, char* cmd) {
  ERROR(" ERROR: expected '%s' got '%s'\n", expected, found);
  ERROR("        %s\n", cmd);
}

void get_const_str(char** cmd_p, char* dest) {
  char* cmd = *cmd_p;
  for (int i=0; cmd[i] != '"' && cmd[i] != '\0'; ++i) {
    dest[i] = cmd[i];
    *cmd_p += 1;
  }

  return;
}


/* PRIVATE FUNCTIONS DEFINITIONS */
static char* consume_whitespaces(char* cmd) {
  while (*cmd == ' ') {
    ++cmd;
  }
  return cmd;
}

static char* get_tokname(tokenE tok) {
  if (tok == TOK_NUMBER) {
    return "number";
  }
  if (tok == TOK_VAR) {
    return "variable";
  }
  for (tokenS* t = tokens; t->tok_id != TOK_NONE; ++t) {
    if (t->tok_id == tok) {
      return t->tok_name;
    }
  }
  return NULL;
}
