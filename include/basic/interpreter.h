#ifndef _INTERPRETER_H
#define _INTERPRETER_H

#include "types.h"
#include "printf.h"

#define ERROR(...)  printf(__VA_ARGS__)
#define DEBUG_MODE 1
#if DEBUG_MODE
#define DEBUG(...)  printf(__VA_ARGS__)
#else
#define DEBUG(...)
#endif


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
} typedef tokenE;

struct tokenS {
  tokenE tok_id;
  char tok_name[16];
} typedef tokenS;


tokenE get_next_token(char* cmd, char* dest);
char* consume_whitespaces(char* cmd);
u64 get_str_len(char* cmd);

void execute_command(void* env, char* cmd);
void interprete_command(void* env, char* cmd);


#endif /* _INTERPRETER_H */
