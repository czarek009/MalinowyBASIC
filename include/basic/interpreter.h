#ifndef _INTERPRETER_H
#define _INTERPRETER_H

#include "types.h"


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

static const tokenS tokens[] = {
  /* single char */
  {",", TOK_COMMA},
  {"\"", TOK_QUOTE},
  {";", TOK_SEMICOLON},
  {"+", TOK_ADD},
  {"-", TOK_SUB},
  {"/", TOK_DIV},
  {"*", TOK_MULT},
  {"%", TOK_MOD},
  {"$", TOK_AND},
  {"|", TOK_OR},
  {"(", TOK_LPAREN},
  {")", TOK_RPAREN},
  {"#", TOK_HASH},
  {"<", TOK_LT},
  {">", TOK_GT},
  {"=", TOK_EQ},

  /* keywords */
  {"DATA", TOK_DATA},
  {"DEF", TOK_DEF},
  {"DIM", TOK_DIM},
  {"END", TOK_END},
  {"FOR", TOK_FOR},
  {"TO", TOK_TO},
  {"STEP", TOK_STEP},
  {"ON", TOK_ON},
  {"GOSUB", TOK_GOSUB},
  {"GOTO", TOK_GOTO},
  {"IF", TOK_IF},
  {"THEN", TOK_THEN},
  {"ELSE", TOK_ELSE},
  {"INPUT", TOK_INPUT},
  {"LET", TOK_LET},
  {"NEXT", TOK_NEXT},
  {"PRINt", TOK_PRINT},
  {"READ", TOK_READ},
  {"REM", TOK_REM},
  {"RESTORE", TOK_RESTORE},
  {"RETURN", TOK_RETURN},
  {"STOP", TOK_STOP},

  {NULL, TOK_NONE}
};


tokenE get_next_token(char* cmd, u64* pos);
void get_variable_name(char* dest, char* cmd, u64* pos);
void interprete_command(void* env, char* cmd);


#endif /* _INTERPRETER_H */