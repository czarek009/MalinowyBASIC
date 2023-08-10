#include "instructions.h"
#include "interpreter.h"
#include "parser.h"
#include "session.h"
#include "butils.h"
#include "printf.h"
#include "types.h"
#include "mm.h"
#include "debug.h"
#include "fs.h"
#include "hdmi.h"


/* PRIVATE FUNCTIONS DECLARATION */
static u64 get_line_number(char** cmd_p);

/* PUBLIC FUNCTIONS DEFINITIONS */
sessionErrorCodeE interpreter_process_input(sessionS* env, char* cmd) {
  DEBUG("[*] interpreter_process_input(%s)\n", cmd);

  u64 ln = get_line_number(&cmd);

  if (ln == ~0) {
    DEBUG("   no line number\n", 0);
    sessionErrorCodeE out = interpreter_execute_command(env, cmd, NO_LINE_NUMBER);
    return out;
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

  return SESSION_NO_ERROR;
}

sessionErrorCodeE interpreter_execute_command(sessionS* env, char* cmd, u64 line_number) {
  sessionErrorCodeE out = SESSION_NO_ERROR;
  char buf[32];
  tokenE tok = get_next_token(&cmd, buf, TOK_ANY);

  switch (tok) {

    case TOK_DATA:
      out = data_instr(env, cmd);
      break;

    case TOK_READ:
      out = read_instr(env, cmd);
      break;

    case TOK_RESTORE:
      out = restore_instr(env, cmd);
      break;

    case TOK_DEF:
      out = def_instr(env, cmd);
      break;

    case TOK_LET:
      out = let_instr(env, cmd);
      break;

    case TOK_PRINT:
      out = print_instr(env, cmd);
      break;

    case TOK_INPUT:
      out = input_instr(env, cmd);
      break;

    case TOK_GOTO:
      out = goto_instr(env, cmd);
      break;

    case TOK_GOSUB:
      out = gosub_instr(env, cmd, line_number);
      break;

    case TOK_FOR:
      out = for_instr(env, cmd, line_number);
      break;

    case TOK_NEXT:
      out = next_instr(env, cmd, line_number);
      break;

    case TOK_ON:
      out = on_instr(env, cmd, line_number);
      break;

    case TOK_IF:
      out = if_instr(env, cmd, line_number);
      break;

    case TOK_DIM:
      out = dim_instr(env, cmd);
      break;

    case TOK_RETURN:
      out = return_instr(env, cmd);
      break;

    case TOK_REM:
      break;

    case TOK_STOP:
      printf("Program execution stopped\n");
      set_session_status(env, SESSION_STATUS_STOPPED);
      break;
    
    case TOK_END:
      printf("Program finished\n");
      set_session_status(env, SESSION_STATUS_FINISHED);
      break;

    /* ONLY DIRECT MODE */
    /* GENERAL */
    case TOK_HELP:
      if (line_number != NO_LINE_NUMBER) {
        ERROR("[INTERPRETER ERROR] Instruction allowed only in direct mode\n", 0);
        out = SESSION_INVALID_INSTRUCTION;
        break;
      }
      help_instr(env, cmd);
      break;
    
    case TOK_MEM:
      if (line_number != NO_LINE_NUMBER) {
        ERROR("[INTERPRETER ERROR] Instruction allowed only in direct mode\n", 0);
        out = SESSION_INVALID_INSTRUCTION;
        break;
      }
      print_memory_map();
      break;

    case TOK_SINFO:
      if (line_number != NO_LINE_NUMBER) {
        ERROR("[INTERPRETER ERROR] Instruction allowed only in direct mode\n", 0);
        out = SESSION_INVALID_INSTRUCTION;
        break;
      }
      print_session_info(env);
      break;

    case TOK_ENV:
      if (line_number != NO_LINE_NUMBER) {
        ERROR("[INTERPRETER ERROR] Instruction allowed only in direct mode\n", 0);
        out = SESSION_INVALID_INSTRUCTION;
        break;
      }
      print_variables(env);
      break;

    case TOK_FONT:
      if (line_number != NO_LINE_NUMBER) {
          ERROR("[INTERPRETER ERROR] Instruction allowed only in direct mode\n", 0);
          out = SESSION_INVALID_INSTRUCTION;
          break;
        }
        out = font_instr(env, cmd);
        break;

    case TOK_BACKGROUND:
      if (line_number != NO_LINE_NUMBER) {
          ERROR("[INTERPRETER ERROR] Instruction allowed only in direct mode\n", 0);
          out = SESSION_INVALID_INSTRUCTION;
          break;
        }
        out = background_instr(env, cmd);
        break;

    case TOK_CLEAR:
      if (line_number != NO_LINE_NUMBER) {
        ERROR("[INTERPRETER ERROR] Instruction allowed only in direct mode\n", 0);
        out = SESSION_INVALID_INSTRUCTION;
        break;
      }
      hdmi_clear();
      break;

    case TOK_SESSEND:
      if (line_number != NO_LINE_NUMBER) {
        ERROR("[INTERPRETER ERROR] Instruction allowed only in direct mode\n", 0);
        out = SESSION_INVALID_INSTRUCTION;
        break;
      }
      out = SESSION_END;
      break;

    /* BASIC */
    case TOK_RUN:
    case TOK_CONT:
      if (line_number != NO_LINE_NUMBER) {
        ERROR("[INTERPRETER ERROR] Instruction allowed only in direct mode\n", 0);
        out = SESSION_INVALID_INSTRUCTION;
        break;
      }
      out = run_program(env);
      break;

    case TOK_LIST:
      if (line_number != NO_LINE_NUMBER) {
        ERROR("[INTERPRETER ERROR] Instruction allowed only in direct mode\n", 0);
        out = SESSION_INVALID_INSTRUCTION;
        break;
      }
      print_instructions(env);
      break;
    
    /* FILE SYSTEM */
    case TOK_LS:
      if (line_number != NO_LINE_NUMBER) {
        ERROR("[INTERPRETER ERROR] Instruction allowed only in direct mode\n", 0);
        out = SESSION_INVALID_INSTRUCTION;
        break;
      }
      list_files();
      break;

    case TOK_SAVE:
      if (line_number != NO_LINE_NUMBER) {
        ERROR("[INTERPRETER ERROR] Instruction allowed only in direct mode\n", 0);
        out = SESSION_INVALID_INSTRUCTION;
        break;
      }
      save_instr(env, cmd);
      break;

    case TOK_DELETE:
      if (line_number != NO_LINE_NUMBER) {
        ERROR("[INTERPRETER ERROR] Instruction allowed only in direct mode\n", 0);
        out = SESSION_INVALID_INSTRUCTION;
        break;
      }
      delete_instr(env, cmd);
      break;

    case TOK_LOAD:
      if (line_number != NO_LINE_NUMBER) {
        ERROR("[INTERPRETER ERROR] Instruction allowed only in direct mode\n", 0);
        out = SESSION_INVALID_INSTRUCTION;
        break;
      }
      load_instr(env, cmd);
      break;


    case TOK_NONE:
      break;

    default:
      ERROR("[INTERPRETER ERROR] Unknown token: '%s'\n", buf);
      out = SESSION_UNKNOWN_TOKEN;
      break;
  }

  return out;
}


/* PRIVATE FUNCTIONS DEFINITIONS */
static u64 get_line_number(char** cmd_p) {
  while (**cmd_p == ' ') {
    *cmd_p += 1;
  }

  char* cmd = *cmd_p;
  if (!isdigit(cmd[0])) {
    return NO_LINE_NUMBER;
  }
  u64 out = 0;

  for (int i=0; isdigit(cmd[i]); ++i) {
    *cmd_p += 1;
    out *= 10;
    out += ((u8)cmd[i])-48;
  }

  return out;
}
