#include "printf.h"
#include "session.h"
#include "types.h"

sessionErrorCodeE help_instr(sessionS* env, char* cmd) {
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
  printf("  BACKGROUND - change background color\n");

  return SESSION_NO_ERROR;
}