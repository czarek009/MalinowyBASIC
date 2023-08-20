#include "printf.h"
#include "session.h"
#include "parser.h"
#include "types.h"

sessionErrorCodeE help_instr(sessionS* env, char* cmd) {
  char buf[32] = {0};
  tokenE tok = get_next_token(&cmd, buf, TOK_ANY);

  if (tok == TOK_BASIC) {
    printf("BASIC instructions:\n");
    printf(" DATA ...\n");
    printf(" DEF FNA(X) = ...\n");
    printf(" DELAY ...\n");
    printf(" DIM TAB[SIZE]\n");
    printf(" END\n");
    printf(" FOR ... TO ... (STEP ...)\n");
    printf(" GOSUB n\n");
    printf(" GOTO n\n");
    printf(" IF ... THEN ... (ELSE ...)\n");
    printf(" INPUT ...\n");
    printf(" LET VAR = VALUE (: ...)\n");
    printf(" NEXT\n");
    printf(" ON m GOSUB ...\n");
    printf(" ON m GOTO ...\n");
    printf(" PRINT ...\n");
    printf(" READ ...\n");
    printf(" REM\n");
    printf(" RESTORE\n");
    printf(" RETURN\n");
    printf(" STOP\n");
    printf("BASIC functions:\n");
    printf(" FLOAT(...)\n");
    printf(" INT(...)\n");
    printf(" RND(1)\n");

    return SESSION_NO_ERROR;
  }

  printf("BASIC commands:\n");
  printf("  LIST - print current program\n");
  printf("  RUN - runs current program\n");
  printf("  CONT - resumes stopped program\n");
  printf("  ENV - print all variables\n");

  printf("File system commands:\n");
  printf("  LS - print all files\n");
  printf("  SAVE - save current program\n");
  printf("  LOAD - load program\n");
  printf("  DELETE - delete file\n");

  printf("System commands:\n");
  printf("  MEM - print memory map\n");
  printf("  SESSEND - end current session\n");

  printf("HDMI commands:\n");
  printf("  CLEAR - clear screen\n");
  printf("  FONT - change font color\n");
  printf("  BG - change background color\n");

  return SESSION_NO_ERROR;
}