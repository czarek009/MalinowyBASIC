#include "printf.h"
#include "utils.h"
#include "io.h"
#include "session.h"
#include "interpreter.h"
#include "hdmi.h"
#include "startup.h"

void main(void){
  initialize_all_modules();
  print_startup_info();

  while (1) {
    printf("START SESSION\n");
    sessionS *current_session = session_init();

    while (1) {
      char buf[256] = {0};
      sessionErrorCodeE result = SESSION_NO_ERROR;
      readline(buf, "$> ");
      result = interpreter_process_input(current_session, buf);

      if (result == SESSION_END) {
        printf("SESSION END\n");
        break;
      }
      if (result != SESSION_NO_ERROR) {
        ERROR("ERROR\n");
      }
    }
    session_end(current_session);
  }
  /* hdmi allocates buffer - lickitung will show memory leak untill we call hdmi_end() */
  hdmi_end();
  lickitung_check();
}