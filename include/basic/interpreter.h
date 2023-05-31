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
  char* tok_name;
  tokenE tok_id;
} typedef tokenS;

// static const tokenS tokens[] = {
//   /* single char */
//   {.tok_name=",",  .tok_id=TOK_COMMA},
//   {.tok_name="\"", .tok_id=TOK_QUOTE},
//   {.tok_name=";",  .tok_id=TOK_SEMICOLON},
//   {.tok_name="+",  .tok_id=TOK_ADD},
//   {.tok_name="-",  .tok_id=TOK_SUB},
//   {.tok_name="/",  .tok_id=TOK_DIV},
//   {.tok_name="*",  .tok_id=TOK_MULT},
//   {.tok_name="$",  .tok_id=TOK_AND},
//   {.tok_name="|",  .tok_id=TOK_OR},
//   {.tok_name="(",  .tok_id=TOK_LPAREN},
//   {.tok_name=")",  .tok_id=TOK_RPAREN},
//   {.tok_name="#",  .tok_id=TOK_HASH},
//   {.tok_name="<",  .tok_id=TOK_LT},
//   {.tok_name=">",  .tok_id=TOK_GT},
//   {.tok_name="=",  .tok_id=TOK_EQ},

//   /* keywords */
//   {.tok_name="DATA",    .tok_id=TOK_DATA},
//   {.tok_name="DEF",     .tok_id=TOK_DEF},
//   {.tok_name="DIM",     .tok_id=TOK_DIM},
//   {.tok_name="END",     .tok_id=TOK_END},
//   {.tok_name="FOR",     .tok_id=TOK_FOR},
//   {.tok_name="TO",      .tok_id=TOK_TO},
//   {.tok_name="STEP",    .tok_id=TOK_STEP},
//   {.tok_name="ON",      .tok_id=TOK_ON},
//   {.tok_name="GOSUB",   .tok_id=TOK_GOSUB},
//   {.tok_name="GOTO",    .tok_id=TOK_GOTO},
//   {.tok_name="IF",      .tok_id=TOK_IF},
//   {.tok_name="THEN",    .tok_id=TOK_THEN},
//   {.tok_name="ELSE",    .tok_id=TOK_ELSE},
//   {.tok_name="INPUT",   .tok_id=TOK_INPUT},
//   {.tok_name="LET",     .tok_id=TOK_LET},
//   {.tok_name="NEXT",    .tok_id=TOK_NEXT},
//   {.tok_name="PRINt",   .tok_id=TOK_PRINT},
//   {.tok_name="READ",    .tok_id=TOK_READ},
//   {.tok_name="REM",     .tok_id=TOK_REM},
//   {.tok_name="RESTORE", .tok_id=TOK_RESTORE},
//   {.tok_name="RETURN",  .tok_id=TOK_RETURN},
//   {.tok_name="STOP",    .tok_id=TOK_STOP},

//   {.tok_name="", TOK_NONE}
// };


tokenE get_next_token(char* cmd, u64 pos, char* dest);

void execute_command(void* env, char* cmd);
void get_variable_name(char* dest, char* cmd, u64* pos);
void interprete_command(void* env, char* cmd);
u64 consume_whitespaces(char* cmd, u64 pos);


#endif /* _INTERPRETER_H */