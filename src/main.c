#include "uart.h"
#include "printf.h"
#include "irq.h"
#include "utils.h"
#include "io.h"
#include "mm.h"
#include "session.h"
#include "interpreter.h"
#include "tests.h"
#include "random.h"
#include "keyboard.h"
#include "timer.h"
#include "hdmi.h"
#include "images.h"


void print_greetings(void) {
  printf("\n\n\nMalinowyBASIC\n");
  printf("MalinowyBASIC\n");

  int rpiv = -1;

  #if RPI_VERSION == 3
  rpiv = 3;
  #elif RPI_VERSION == 4
  rpiv = 4;
  #endif

  printf("RPi version: %d\n\n", rpiv);

}

void putc(void *p, char c) {
  if (c == '\n')
    uart_send('\r');
  uart_send(c);
  // hdmi_printf_char(c);
}

char game[145][128] = {
"10 PRINT \"                                \";\"HAMURABI\"",
"20 PRINT \"               \";\"CREATIVE COMPUTING  MORRISTOWN, NEW JERSEY\"",
"30 PRINT",
"31 PRINT",
"32 PRINT",
"80 PRINT \"TRY YOUR HAND AT GOVERNING ANCIENT SUMERIA\"",
"90 PRINT \"FOR A TEN-YEAR TERM OF OFFICE.\",",
"95  LET D1=0",
"96  LET P1=0",
"100 LET Z=0",
"101 LET P=95",
"102 LET S=2800",
"103 LET H=3000",
"104 LET E=H-S",
"110 LET Y=3",
"111 LET A=H/Y",
"112 LET I=5",
"113 LET Q=1",
"210 LET D=0",
"215 PRINT \"\",\"\", \"HAMURABI:  I BEG TO REPORT TO YOU,\"",
"216 LET Z=Z+1",
"217 PRINT \"IN YEAR \";Z;\",\"; \" \";D;\" PEOPLE STARVED, \";I;\" CAME TO THE CITY,\"",
"218 LET P=P+I",
"227 IF Q>0 THEN 230",
"228 LET P=INT(P/2)",
"229 PRINT \"A HORRIBLE PLAGUE STRUCK!  HALF THE PEOPLE DIED.\"",
"230 PRINT \"POPULATION IS NOW \";P",
"232 PRINT \"THE CITY NOW OWNS \";A;\" ACRES.\"",
"235 PRINT \"YOU HARVESTED \";Y;\" BUSHELS PER ACRE.\"",
"250 PRINT \"THE RATS ATE \";E;\" BUSHELS.\"",
"260 PRINT \"YOU NOW HAVE \";S;\" BUSHELS IN STORE.\",",
"270 IF Z==11 THEN 860",
"310 LET C=INT(10.0*RND(1))",
"311 LET Y=C+17",
"312 PRINT \"LAND IS TRADING AT \";Y;\" BUSHELS PER ACRE.\"",
"320 PRINT \"HOW MANY ACRES DO YOU WISH TO BUY\"",
"321 INPUT Q",
"322 IF Q<0 THEN 850",
"323 IF Y*Q<=S THEN 330",
"324 GOSUB 710",
"325 GOTO 320",
"330 IF Q==0 THEN 340",
"331 LET A=A+Q",
"332 LET S=S-Y*Q",
"333 LET C=0",
"334 GOTO 400",
"340 PRINT \"HOW MANY ACRES DO YOU WISH TO SELL\"",
"341 INPUT Q",
"342 IF Q<0 THEN 850",
"343 IF Q<A THEN 350",
"344 GOSUB 720",
"345 GOTO 340",
"350 LET A=A-Q",
"351 LET S=S+Y*Q",
"352 LET C=0",
"400 PRINT",
"410 PRINT \"HOW MANY BUSHELS DO YOU WISH TO FEED YOUR PEOPLE\"",
"411 INPUT Q",
"412 IF Q<0 THEN 850",
"420 IF Q<=S THEN 430",
"421 GOSUB 710",
"422 GOTO 410",
"430 LET S=S-Q",
"431 LET C=1",
"432 PRINT",
"440 PRINT \"HOW MANY ACRES DO YOU WISH TO PLANT WITH SEED\"",
"441 INPUT D",
"442 IF D==0 THEN 511",
"443 IF D<0 THEN 850",
"445 IF D<=A THEN 450",
"446 GOSUB 720",
"447 GOTO 440",
"450 IF INT(D/2)<=S THEN 455",
"452 GOSUB 710",
"453 GOTO 440",
"455 IF D<10*P THEN 510",
"460 PRINT \"BUT YOU HAVE ONLY \";P;\" PEOPLE TO TEND THE FIELDS!  NOW THEN,\"",
"470 GOTO 440",
"510 LET S=S-INT(D/2)",
"511 GOSUB 800",
"515 LET Y=C",
"516 LET H=D*Y",
"517 LET E=0",
"521 GOSUB 800",
"522 IF INT(C/2)!=C/2 THEN 530",
"525 LET E=INT(S/C)",
"530 LET S=S-E+H",
"531 GOSUB 800",
"533 LET I=INT(C*(20*A+S)/P/100+1)",
"540 LET C=INT(Q/20)",
"542 LET Q=INT(10.0*(2.0*RND(1)-0.3))",
"550 IF P<C THEN 210",
"552 LET D=P-C",
"553 IF D>0.45*P THEN 560",
"554 LET P1=((Z-1)*P1+D*100/P)/Z",
"555 LET P=C",
"556 LET D1=D1+D",
"557 GOTO 215",
"560 PRINT \"\", \"YOU STARVED \";D;\" PEOPLE IN ONE YEAR!!!\"",
"565 PRINT \"DUE TO THIS EXTREME MISMANAGEMENT YOU HAVE NOT ONLY\"",
"566 PRINT \"BEEN IMPEACHED AND THROWN OUT OF OFFICE BUT YOU HAVE\"",
"567 PRINT \"ALSO BEEN DECLARED NATIONAL FINK!!!!\"",
"568 GOTO 990",
"710 PRINT \"HAMURABI:  THINK AGAIN.  YOU HAVE ONLY\"",
"711 PRINT S;\" BUSHELS OF GRAIN.  NOW THEN,\"",
"712 RETURN",
"720 PRINT \"HAMURABI:  THINK AGAIN.  YOU OWN ONLY \";A;\" ACRES.  NOW THEN,\"",
"730 RETURN",
"800 LET C=INT(RND(1)*5.0)+1",
"801 RETURN",
"850 PRINT \"\", \"HAMURABI:  I CANNOT DO WHAT YOU WISH.\"",
"855 PRINT \"GET YOURSELF ANOTHER STEWARD!!!!!\"",
"857 GOTO 990",
"860 PRINT \"IN YOUR 10-YEAR TERM OF OFFICE,\";P1;\"PERCENT OF THE\"",
"862 PRINT \"POPULATION STARVED PER YEAR ON THE AVERAGE, I.E. A TOTAL OF\"",
"865 PRINT D1;\"PEOPLE DIED!!\"",
"866 LET L=A/P",
"870 PRINT \"YOU STARTED WITH 10 ACRES PER PERSON AND ENDED WITH\"",
"875 PRINT L;\"ACRES PER PERSON.\",",
"880 IF P1>33 THEN 565",
"885 IF L<7 THEN 565",
"890 IF P1>10 THEN 940",
"892 IF L<9 THEN 940",
"895 IF P1>3 THEN 960",
"896 IF L<10 THEN 960",
"900 PRINT \"A FANTASTIC PERFORMANCE!!!  CHARLEMANGE, DISRAELI, AND\"",
"905 PRINT \"JEFFERSON COMBINED COULD NOT HAVE DONE BETTER!\"",
"906 GOTO 990",
"940 PRINT \"YOUR HEAVY-HANDED PERFORMANCE SMACKS OF NERO AND IVAN IV.\"",
"945 PRINT \"THE PEOPLE (REMAINING) FIND YOU AN UNPLEASANT RULER, AND,\"",
"950 PRINT \"FRANKLY, HATE YOUR GUTS!!\"",
"951 GOTO 990",
"960 PRINT \"YOUR PERFORMANCE COULD HAVE BEEN SOMEWHAT BETTER, BUT\"",
"965 PRINT \"REALLY WASN'T TOO BAD AT ALL. \";INT(FLOAT(P)*0.8*RND(1));\" PEOPLE\"",
"970 PRINT \"WOULD DEARLY LIKE TO SEE YOU ASSASSINATED BUT WE ALL HAVE OUR\"",
"975 PRINT \"TRIVIAL PROBLEMS.\"",
"990 PRINT",
"991 FOR N=1 TO 10",
"992 PRINT \"#\"",
"993 NEXT",
"995 PRINT \"SO LONG FOR NOW.\",",
"999 END",
"\0",
};

void main(void){
  rand_init();
  init_keyboard();

  uart_init_gpio();
  init_printf(0, putc);

  irq_init_vectors();
  enable_interrupt_controller();
  irq_enable();

  mem_init();

  hdmi_init();
  hdmi_set_resolution();

  print_greetings();

  printf("HDMI test\n");


  while (1) {
    printf("START SESSION\n");
    sessionS *current_session = session_init();
    for (u64 i = 0; game[i][0] != '\0'; ++i) {
      interpreter_process_input(current_session, game[i]);
    }

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
    lickitung_check();
  }
}