#include "keyboard.h"
#include "types.h"
#include "gpio.h"
#include "peripherials.h"
#include "printf.h"


static char scancode_to_ascii[2][128] = {
  { 0,0,0,0,0,27,27,0,         0,0,0,0,0,9,96,0,         0,0,0,0,0,113,49,0,       0,0,122,115,97,119,50,0,    // w/o SHIFT or ALT(GR)
    0,99,120,100,101,52,51,0,0,32,118,102,116,114,53,0,0,110,98,104,103,121,54,0,0,0,109,106,117,55,56,0,
    0,44,107,105,111,48,57,0,0,46,47,108,59,112,45,0,  0,0,39,0,91,61,0,0,     0,0,10,93,0,124,0,0,
    0,60,0,0,0,0,8,0,        0,49,0,52,55,0,0,0,          0,46,50,53,54,56,27,0,         0,43,51,45,42,57,0,0  },
  { 0,0,0,0,0,0,0,0,         0,0,0,0,0,0,126,0,        0,0,0,0,0,81,33,0,        0,0,90,83,65,87,64,35,       // with SHIFT
    0,67,88,68,69,36,35,0,    0,32,86,70,84,82,37,0,     0,78,66,72,71,89,94,0,    0,0,77,74,85,38,42,0,
    0,60,75,73,79,41,40,0,   0,62,63,76,58,80,95,0,   0,0,34,0,123,43,0,0,      0,0,0,125,0,92,0,0,
    0,62,0,0,0,0,0,0,        0,0,0,0,0,0,0,0,          0,0,0,0,0,0,0,0,          0,0,0,0,0,0,0,0  } };


void init_keyboard(void) {
  GPIO_REGS->asynchronous_falling_edge_detect_enable.registers[0] = 1 << CLOCK_PIN;
  gpio_func_selection(CLOCK_PIN, INPUT);
  gpio_func_selection(DATA_PIN, INPUT);
}

char code_to_ascii(u16 code) {
  static bool shift_is_active = false;
  static bool next_is_released = false;
  char out = 0;

  code = (code>>1) & 255;

  switch (code) {
    case 18: // LEFT SHIFT
    case 89: // RIGHT SHIFT
      shift_is_active = !next_is_released;
      next_is_released = false;
      break;
    case 240: // key release code
      next_is_released = true;
      break;
    default:
      if (!next_is_released) {
        out = scancode_to_ascii[shift_is_active][code & 127];
      }
      next_is_released = false;
      break;
  }
  printf("\ncode: %u ascii: %u\n", code, (u32)out);
  return out;
}