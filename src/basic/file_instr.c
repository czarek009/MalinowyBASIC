#include "types.h"
#include "session.h"
#include "parser.h"
#include "fs.h"
#include "butils.h"
#include "interpreter.h"


#define min(a,b) \
  ({ __typeof__ (a) _a = (a); \
      __typeof__ (b) _b = (b); \
    _a < _b ? _a : _b; })


/* PRIVATE FUNCTIONS DECLARATIONS */
static char* consume_whitespaces(char* cmd);

/* PUBLIC FUNCTIONS DEFINITIONS */
sessionErrorCodeE save_instr(sessionS* env, char* cmd) {
  char* buf = malloc(128);
  char progname[FILENAME_LEN] = {0};

  cmd = consume_whitespaces(cmd);
  if (*cmd == '\0') {
    ERROR("[INSTRUCTION ERROR] Expected file name\n");
    return SESSION_FS_ERROR;
  }

  strncpy(progname, cmd, min(FILENAME_LEN, strlen(cmd)));

  fileS* file = create_file(progname);
  if (file == NULL) {
    return SESSION_FS_ERROR;
  }

  instructionS *node = env->metadata.instructions_start;
  while(node != NULL) {
    sprintf(buf, "%lu%s\0", node->line_number, node->instruction);
    write_to_file(file, buf, strlen(buf)+1, true);
    printf("Saved line: '%s'\n", buf);
    node = node->next;
  }

  free(buf);

  return SESSION_NO_ERROR;
}

sessionErrorCodeE load_instr(sessionS* env, char* cmd) {
  char progname[FILENAME_LEN] = {0};

  cmd = consume_whitespaces(cmd);
  if (*cmd == '\0') {
    ERROR("[INSTRUCTION ERROR] Expected file name\n");
    return SESSION_FS_ERROR;
  }

  strncpy(progname, cmd, min(FILENAME_LEN, strlen(cmd)));

  fileS* file = open_file(progname);
  if (file == NULL) {
    return SESSION_FS_ERROR;
  }

  char* file_buf = malloc(file->size_in_bytes);
  char* aux = file_buf;
  read_from_file(file, file_buf, file->size_in_bytes);

  char* line_buf = malloc(128);
  u64 to_read = file->size_in_bytes;
  while (to_read > 0) {
    u64 len = strlen(file_buf);
    strncpy(line_buf, file_buf, len);
    line_buf[len] = '\0';
    printf("Loaded line: %s\n", line_buf);
    interpreter_process_input(env, line_buf);
    file_buf += len+1;
    to_read -= len+1;
  }

  free(line_buf);
  free(aux);

  return SESSION_NO_ERROR;
}

sessionErrorCodeE delete_instr(sessionS* env, char* cmd) {
  cmd = consume_whitespaces(cmd);
  if (*cmd == '\0') {
    ERROR("[INSTRUCTION ERROR] Expected file name\n");
    return SESSION_FS_ERROR;
  }
  bool out = delete_file(cmd);
  if (!out) {
    return SESSION_FS_ERROR;
  }

  return SESSION_NO_ERROR;
}

/* PRIVATE FUNCTIONS DEFINITIONS */
static char* consume_whitespaces(char* cmd) {
  while (*cmd == ' ') {
    ++cmd;
  }
  return cmd;
}