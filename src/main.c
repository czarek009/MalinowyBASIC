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
#include "sd.h"


#define BOOT_SIGNATURE 0xAA55

typedef struct __attribute__((__packed__)) {
    u8 head;
    u8 sector : 6;
    u8 cylinder_hi : 2;
    u8 cylinder_lo;
} chs_address ;

typedef struct __attribute__((__packed__)) {
    u8 status;
    chs_address first_sector;
    u8 type;
    chs_address last_sector;
    u32 first_lba_sector;
    u32 num_sectors;
} partition_entry ;

typedef struct __attribute__((__packed__)) {
    u8 bootCode[0x1BE];
    partition_entry partitions[4];
    u16 bootSignature;
} master_boot_record;


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
}


void main(void){
  rand_init();
  init_keyboard();

  uart_init_gpio();
  init_printf(0, putc);

  irq_init_vectors();
  enable_interrupt_controller();
  irq_enable();

  mem_init();

  // hdmi_init(320, 200, 32);

  print_greetings();

  if (!emmc_init()) {
    printf("FAILED TO INIT EMMC\n");
    return;
  }

  printf("EMMC Disk initialized\n");

  master_boot_record mbr;

  int r = emmc_read(&mbr, sizeof(mbr));

  printf("Read disk returned: %d\n", r);

  if (mbr.bootSignature != BOOT_SIGNATURE) {
    printf("BAD BOOT SIGNATURE: %X\n", mbr.bootSignature);
  }

  u64 start = 0;

  for (int i=0; i<4; i++) {
    if (mbr.partitions[i].type == 0) {
      break;
    }

    printf("Partition %d:\n", i);
    printf("\t Type: %d\n", mbr.partitions[i].type);
    printf("\t NumSecs: %d\n", mbr.partitions[i].num_sectors);
    printf("\t Status: %d\n", mbr.partitions[i].status);
    printf("\t Start: 0x%X\n", mbr.partitions[i].first_lba_sector);
    start = mbr.partitions[i].first_lba_sector;
  }

  printf("[SD] Set position to %lu*512...\n", start);
  emmc_seek(start*512);
  printf("Done.\n");

  char* rbuf1 = malloc(512);

  printf("[SD] Read data...\n");
  emmc_read(rbuf1, 512);
  rbuf1[8] = '\0';
  printf("Data: %s\n", rbuf1);

  char* wbuf = malloc(512);
  wbuf = "qpad00pa";
  
  printf("[SD] Write data...\n");
  emmc_write(wbuf, 512);
  printf("[SD] Done.\n");

  delay_ms(1000);
  char* rbuf2 = malloc(512);

  printf("[SD] Read data...\n");
  emmc_read(rbuf2, 512);
  rbuf2[8] = '\0';
  printf("Data: %s\n", rbuf2);

  // printf("HDMI test\n");
  // hdmi_draw_image(cat_320x200, 320, 200, 0, 0);
  // delay_ms(1000);

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
    lickitung_check();
  }
}