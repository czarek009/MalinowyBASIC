#include "instructions.h"
#include "parser.h"
#include "interpreter.h"
#include "session.h"
#include "butils.h"
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
  {.tok_name="%",  .tok_id=TOK_MOD},
  {.tok_name="^",  .tok_id=TOK_POW},
  {.tok_name="&",  .tok_id=TOK_AND},
  {.tok_name="|",  .tok_id=TOK_OR},
  {.tok_name="!",  .tok_id=TOK_NEG},
  {.tok_name="(",  .tok_id=TOK_LPAREN},
  {.tok_name=")",  .tok_id=TOK_RPAREN},
  {.tok_name="]",  .tok_id=TOK_RSQUARE},
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

  {.tok_name="RND",     .tok_id=TOK_BUILTIN},
  {.tok_name="INT",     .tok_id=TOK_BUILTIN},
  {.tok_name="FLOAT",   .tok_id=TOK_BUILTIN},

  {.tok_name="DELAY",    .tok_id=TOK_DELAY},
  {.tok_name="SESSEND",  .tok_id=TOK_SESSEND},
  {.tok_name="FONT",     .tok_id=TOK_FONT},
  {.tok_name="BG",       .tok_id=TOK_BG},
  {.tok_name="CLEAR",    .tok_id=TOK_CLEAR},
  {.tok_name="HELP",     .tok_id=TOK_HELP},
  {.tok_name="BASIC",    .tok_id=TOK_BASIC},

  /* FILE SYSTEM COMMANDS */
  {.tok_name="SAVE",    .tok_id=TOK_SAVE},
  {.tok_name="LOAD",    .tok_id=TOK_LOAD},
  {.tok_name="DELETE",  .tok_id=TOK_DELETE},
  {.tok_name="LS",      .tok_id=TOK_LS},


  {.tok_name="", .tok_id=TOK_NONE}
};


/* PRIVATE FUNCTIONS DECLARATIONS */
static char* consume_whitespaces(char* cmd);
static char* get_tokname(tokenE tok);


/* PUBLIC FUNCTIONS DEFINITIONS */
tokenE get_next_token(char** cmd_p, char* dest, tokenE expected_token) {
  *cmd_p = consume_whitespaces(*cmd_p);
  char* cmd = *cmd_p;
  DEBUG("[DEBUG] get_next_token(%s)\n", cmd);

  /* EOL */
  if (*cmd == '\0') {
    dest[0] = '\0';
    if (expected_token != TOK_NOTNUMBER && expected_token != TOK_ANY && expected_token != TOK_NONE) {
      ERROR("[*] ERROR: unexpected eol\n", 0);
      return TOK_ERROR;
    }
    return TOK_NONE;
  }

  /* HEX NUMBER */
  if (expected_token != TOK_NOTNUMBER && (cmd[0] == '0' && (cmd[1] == 'X' || cmd[1] == 'x'))) {
    dest[0] = cmd[0];
    dest[1] = cmd[1];
    u32 i = 2;
    for(; ishealphaxnum(cmd[i]); i++) {
      dest[i] = cmd[i];
    }
    dest[i] = '\0';

    if (isin(cmd[i], " +-*/=<>)]%^,;") || cmd[i] == '\0') {
      DEBUG("[DEBUG] Token read: '%s'\n", dest);
      *cmd_p += i;
      if (expected_token != TOK_ANY && expected_token != TOK_NUMBER && expected_token != TOK_HEX_NUMBER) {
        report_error(get_tokname(expected_token), "hexnumber", cmd);
        return TOK_ERROR;
      }
      return TOK_HEX_NUMBER;
    }

    ERROR(" [PARSER ERROR] Unexpected char while parsing hexnumber '%c'\n", cmd[i]);
    dest[0] = '\0';
    return TOK_ERROR;
  }

  /* NUMBER */
  if (expected_token != TOK_NOTNUMBER && (isdigit(*cmd) || *cmd == '-' || *cmd == '+')) {
    bool isFloat = false;
    dest[0] = cmd[0];
    int i = 1;
    for (; isdigit(cmd[i]) || cmd[i] == '.'; ++i) {
      if (cmd[i] == '.') {
        if (isFloat) {
          ERROR(" [PARSER ERROR] Invalid float: %s\n", cmd);
          dest[0] = '\0';
          return TOK_ERROR;
        }
        isFloat = true;
      }
      dest[i] = cmd[i];
    }
    if (cmd[i-1] == '.') {
      ERROR(" [PARSER ERROR] Invalid float: %s\n", cmd);
      dest[0] = '\0';
      return TOK_ERROR;
    }
    dest[i] = '\0';

    if (isin(cmd[i], " +-*/=<>)]%^,;") || cmd[i] == '\0') {
      DEBUG("[DEBUG] Token read: '%s'\n", dest);
      *cmd_p += i;
      if (expected_token != TOK_ANY && expected_token != TOK_NUMBER) {
        report_error(get_tokname(expected_token), "number", cmd);
        return TOK_ERROR;
      }
      return TOK_NUMBER;
    }

    ERROR(" [PARSER ERROR] Unexpected char while parsing number '%c'\n", cmd[i]);
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
      DEBUG("[DEBUG] Token read: '%s'\n", dest);
      if (expected_token != TOK_ANY && expected_token != TOK_NOTNUMBER && expected_token != t->tok_id) {
        report_error(get_tokname(expected_token), get_tokname(t->tok_id), cmd);
        return TOK_ERROR;
      }
      return t->tok_id;
    }
  }

  /* VARIABLE OR FUNCTION */
  u8 varlen = is_valid_varname(cmd);
  if (varlen) {
    strncpy(dest, cmd, varlen);
    dest[varlen] = '\0';
    *cmd_p += varlen;
    DEBUG("[DEBUG] Token read: '%s'\n", dest);

    if (varlen == 2 && dest[0] == 'F' && dest[1] == 'N') {
      /* name cannot be just 'fn' */
      ERROR("[PARSER ERROR] Invalid variable name 'FN'\n", 0);
      return TOK_ERROR;
    }
    /* check if this is a function name */
    if (varlen > 2 && dest[0] == 'F' && dest[1] == 'N') {
      if (expected_token != TOK_ANY && expected_token != TOK_NOTNUMBER && expected_token != TOK_FN) {
        report_error(get_tokname(expected_token), "function", cmd);
        return TOK_ERROR;
      }
      return TOK_FN;
    }
    /* check if this is an array name */
    if ((*cmd_p)[0] == '[') {
      if (expected_token != TOK_ANY && expected_token != TOK_NOTNUMBER && expected_token != TOK_ARRAY_INT) {
        report_error(get_tokname(expected_token), "array int", cmd);
        return TOK_ERROR;
      }
      *cmd_p += 1;
      return TOK_ARRAY_INT;
    }
    if ((*cmd_p)[0] == '%' && (*cmd_p)[1]== '[') {
      if (expected_token != TOK_ANY && expected_token != TOK_NOTNUMBER && expected_token != TOK_ARRAY_FLOAT) {
        report_error(get_tokname(expected_token), "array float", cmd);
        return TOK_ERROR;
      }
      *cmd_p += 2;
      return TOK_ARRAY_FLOAT;
    }
    if ((*cmd_p)[0] == '$' && (*cmd_p)[1]== '[') {
      if (expected_token != TOK_ANY && expected_token != TOK_NOTNUMBER && expected_token != TOK_ARRAY_STRING) {
        report_error(get_tokname(expected_token), "array string", cmd);
        return TOK_ERROR;
      }
      *cmd_p += 2;
      return TOK_ARRAY_STRING;
    }

    if (expected_token != TOK_ANY && expected_token != TOK_NOTNUMBER && expected_token != TOK_VAR) {
      report_error(get_tokname(expected_token), "variable", cmd);
      return TOK_ERROR;
    }
    return TOK_VAR;
  }

  /* UNKNOWN TOKEN */
  dest[0] = '\0';
  ERROR("[PARSER ERROR] Unknown token: %s\n", cmd);
  return TOK_ERROR;
}

void reverse_get_next_token(char** cmd_p, char* buf) {
  size_t length = strlen(buf);
  *cmd_p -= length;
}

sessionErrorCodeE parse_array_dim_nr(char *cmd, u8 *dim_nr) {
  if(cmd[0] == ']'){
    ERROR("[PARSER ERROR] No indexes/sizes in array brackets\n");
    return SESSION_PARSING_ERROR;
  }
  u8 dim_counter = 1;
  for(u64 i = 1; cmd[i] != ']' && cmd[i] != '\0'; i++) {
    if(cmd[i] == ',') {
      if(cmd[i - 1] == ',') {
        ERROR("[PARSER ERROR] Consecutive commas in array indexes/sizes\n");
        return SESSION_PARSING_ERROR;
      }
      if(dim_counter == UINT8_MAX) {
      ERROR("[PARSER ERROR] Maximum number of array dimentions is 255\n");
      return SESSION_PARSING_ERROR;
      }
      dim_counter++;
    }
  }
  *dim_nr = dim_counter;
  return SESSION_NO_ERROR;
}

u8 get_array_parsed_type(tokenE tok) {
  switch(tok){
    case TOK_ARRAY_INT:
      return INTEGER;
    case TOK_ARRAY_FLOAT:
      return FLOATING_POINT;
    case TOK_ARRAY_STRING:
      return STRING;
    default:
      return NOT_FOUND;
  }
}

void report_error(char* expected, char* found, char* cmd) {
  ERROR(" [PARSER ERROR] Expected '%s' got '%s'\n", expected, found);
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
