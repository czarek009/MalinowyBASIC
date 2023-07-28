#ifndef _PARSER_H
#define _PARSER_H

#include "session.h"
#include "types.h"
#include "debug.h"


enum tokenE {
  TOK_NONE, TOK_ERROR, TOK_ANY, TOK_NOTNUMBER,

  TOK_NUMBER, TOK_VAR, TOK_FN, TOK_ARRAY_INT, TOK_ARRAY_FLOAT, TOK_ARRAY_STRING,

  /* single char */
  TOK_COMMA, TOK_QUOTE, TOK_SEMICOLON,
  TOK_ADD, TOK_SUB, TOK_DIV, TOK_MULT, TOK_MOD, TOK_POW,
  TOK_AND, TOK_OR, TOK_NEG,
  TOK_LT, TOK_GT, TOK_EQ,
  TOK_LPAREN, TOK_RPAREN, TOK_LSQUARE, TOK_RSQUARE,
  TOK_HASH, TOK_DOLAR,

  /* keywords */
  TOK_DATA, TOK_READ, TOK_RESTORE,
  TOK_LET, TOK_INPUT, TOK_PRINT,
  TOK_DEF, TOK_DIM, TOK_END,
  TOK_FOR, TOK_TO, TOK_STEP, TOK_NEXT,
  TOK_ON, TOK_GOTO, TOK_GOSUB, TOK_RETURN,
  TOK_IF, TOK_THEN, TOK_ELSE,
  TOK_REM, TOK_STOP, TOK_CONT,

  /* additional instructions */
  TOK_ENV, TOK_LIST, TOK_MEM, TOK_RUN, TOK_SINFO,
  TOK_SESSEND,
} typedef tokenE;

struct tokenS {
  tokenE tok_id;
  char tok_name[16];
} typedef tokenS;


tokenE get_next_token(char** cmd_p, char* dest, tokenE expected_token);
void reverse_get_next_token(char** cmd_p, char* buf);
void report_error(char* expected, char* found, char* cmd);
void get_const_str(char** cmd_p, char* dest);
sessionErrorCodeE parse_array_dim_nr(char *cmd, u8 *dim_nr);
u8 get_array_parsed_type(tokenE tok);

#endif /* _PARSER_H */