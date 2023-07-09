#ifndef _PARSER_H
#define _PARSER_H

#include "session.h"
#include "types.h"
#include "printf.h"
#include "debug.h"


enum tokenE {
  TOK_NONE, TOK_ERROR, TOK_ANY,

  TOK_NUMBER, TOK_VAR, TOK_FN,

  /* single char */
  TOK_COMMA, TOK_QUOTE, TOK_SEMICOLON,
  TOK_ADD, TOK_SUB, TOK_DIV, TOK_MULT,TOK_MOD,
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
  TOK_ENV, TOK_LIST, TOK_MEM, TOK_RUN,
} typedef tokenE;

struct tokenS {
  tokenE tok_id;
  char tok_name[16];
} typedef tokenS;


tokenE get_next_token(char** cmd_p, char* dest, tokenE expected_token);
void report_error(char* expected, char* found, char* cmd);
void get_const_str(char** cmd_p, char* dest);

#endif /* _PARSER_H */