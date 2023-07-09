#ifndef _INTERPRETER_H
#define _INTERPRETER_H

#include "session.h"
#include "types.h"
#include "printf.h"
#include "debug.h"


enum errorE {
  SUCCESS,
  UNKNOWN_ERROR,
  UNEXPECTED_TOKEN,
  INVALID_VAR_NAME,
  INVALID_EXPR,
} typedef errorE;

enum tokenE {
  TOK_NONE,
  TOK_ERROR,
  TOK_ANY,

  TOK_NUMBER,
  TOK_VAR,
  TOK_FN,

  /* single char */
  TOK_COMMA,
  TOK_QUOTE,
  TOK_SEMICOLON,
  TOK_DOLAR,
  TOK_ADD,
  TOK_SUB,
  TOK_DIV,
  TOK_MULT,
  TOK_MOD,
  TOK_AND,
  TOK_OR,
  TOK_LPAREN,
  TOK_RPAREN,
  TOK_HASH,
  TOK_LT,
  TOK_GT,
  TOK_EQ,

  /* keywords */
  TOK_DATA,
  TOK_DEF,
  TOK_DIM,
  TOK_END,
  TOK_FOR,
  TOK_TO,
  TOK_STEP,
  TOK_ON,
  TOK_GOSUB,
  TOK_GOTO,
  TOK_IF,
  TOK_THEN,
  TOK_ELSE,
  TOK_INPUT,
  TOK_LET,
  TOK_NEXT,
  TOK_PRINT,
  TOK_READ,
  TOK_REM,
  TOK_RESTORE,
  TOK_RETURN,
  TOK_STOP,

  TOK_ENV,
  TOK_LIST,
  TOK_MEM,
  TOK_RUN,
} typedef tokenE;

struct tokenS {
  tokenE tok_id;
  char tok_name[16];
} typedef tokenS;


void report_error(tokenE expected_token, char* found, char* cmd);
tokenE get_next_token(char** cmd_p, char* dest, tokenE expected_token);
char* consume_whitespaces(char* cmd);
u64 get_str_len(char* cmd);
void get_const_str(char** cmd_p, char* dest);

void execute_command(sessionS* env, char* cmd);
void interprete_command(sessionS* env, char* cmd, u64 line_number);


#endif /* _INTERPRETER_H */
