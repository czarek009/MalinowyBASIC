#include "instructions.h"
#include "interpreter.h"
#include "parser.h"
#include "session.h"
#include "butils.h"
#include "printf.h"
#include "types.h"
#include "mm.h"
#include "debug.h"


/* PRIVATE FUNCTIONS DECLARATION */
static u64 get_line_number(char** cmd_p);


/* PUBLIC FUNCTIONS DEFINITIONS */
void interpreter_process_input(sessionS* env, char* cmd) {
  DEBUG("[*] interpreter_process_input(%s)\n", cmd);

  u64 ln = get_line_number(&cmd);

  if (ln == ~0) {
    DEBUG("   no line number\n", 0);
    interpreter_execute_command(env, cmd, 0);
    return;
  }
  while (isdigit(*cmd)) {
    ++cmd;
  }
  DEBUG("    line number: %lu\n", ln);
  DEBUG("    saved command: %s\n", cmd);
  DEBUG("    intruction pointer: %lu\n", (u64)cmd);
  // zapisz na linked list w środowisku
  u64 instrlen = strlen(cmd);
  DEBUG("strlen cmd=%lu\n", instrlen);
  char* instrbuf = malloc(instrlen+1);
  strncpy(instrbuf, cmd, instrlen);
  instrbuf[instrlen] = '\0';
  add_instruction(env, ln, instrbuf);
}

interpreterResultE interpreter_execute_command(sessionS* env, char* cmd, u64 line_number) {
  interpreterResultE result = INTERP_SUCCESS;
  char buf[32];
  tokenE tok = get_next_token(&cmd, buf, TOK_ANY);

  switch (tok) {
    case TOK_LET:
      let_instr(env, cmd);
      break;

    case TOK_PRINT:
      print_instr(env, cmd);
      break;

    case TOK_INPUT:
      input_instr(env, cmd);
      break;

    case TOK_RUN:
      run_program(env);
      break;

    case TOK_LIST:
      print_instructions(env);
      break;

    case TOK_ENV:
      print_variables(env);
      break;

    case TOK_MEM:
      print_memory_map();
      break;

    case TOK_GOTO:
      goto_instr(env, cmd);
      break;

    case TOK_GOSUB:
      gosub_instr(env, cmd, line_number);
      break;

    case TOK_RETURN:
      return_instr(env, cmd);
      break;
    
    default:
      // report invald token error
      ERROR("[INTERPRETER ERROR] Unknown token: '%s'\n", buf);
      result = INTERP_UNKNOWN_TOKEN;
      break;
  }

  return result;
}


/* PRIVATE FUNCTIONS DEFINITIONS */
static u64 get_line_number(char** cmd_p) {
  while (**cmd_p == ' ') {
    *cmd_p += 1;
  }

  char* cmd = *cmd_p;
  if (!isdigit(cmd[0])) {
    return ~0;
  }
  u64 out = 0;

  for (int i=0; isdigit(cmd[i]); ++i) {
    *cmd_p += 1;
    out *= 10;
    out += ((u8)cmd[i])-48;
  }

  return out;
}
