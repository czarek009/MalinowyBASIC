#include "types.h"
#include "session.h"
#include "parser.h"
#include "fs.h"
#include "butils.h"
#include "interpreter.h"


/* PRIVATE FUNCTIONS DECLARATIONS */
static char* consume_whitespaces(char* cmd);

/* PUBLIC FUNCTIONS DEFINITIONS */
sessionErrorCodeE save_instr(sessionS* env, char* cmd) {
  char* buf = malloc(128);
  char progname[FILENAME_LEN] = {0};

  cmd = consume_whitespaces(cmd);

  strncpy(progname, cmd, FILENAME_LEN);

  fileS* file = create_file(progname);

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

  strncpy(progname, cmd, FILENAME_LEN);

  fileS* file = open_file(progname);
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

/* PRIVATE FUNCTIONS DEFINITIONS */
static char* consume_whitespaces(char* cmd) {
  while (*cmd == ' ') {
    ++cmd;
  }
  return cmd;
}