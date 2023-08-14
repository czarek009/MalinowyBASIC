#include "hdmi.h"
#include "mailbox.h"
#include "printf.h"
#include "fonts.h"
#include "mm.h"
#include "dma.h"
#include "debug.h"
#include "startup.h"

#define BUFF_ELEM_SIZE (sizeof(u32))

struct mailboxFbSizeS {
  mailboxTagS tag;
  u32 xres;
  u32 yres;
} typedef mailboxFbSizeS;

struct mailboxFbDepthS {
  mailboxTagS tag;
  u32 bits_per_pixel;
} typedef mailboxFbDepthS;

struct mailboxFbBufferS {
  mailboxTagS tag;
  u32 ptr;
  u32 screen_size;
} typedef mailboxFbBufferS;

struct mailboxFbPitchS {
  mailboxTagS tag;
  u32 pitch;
} typedef mailboxFbPitchS;

struct mailboxFbReqS {
  mailboxFbSizeS res;
  mailboxFbSizeS vres;
  mailboxFbDepthS depth;
  mailboxFbBufferS buff;
  mailboxFbPitchS pitch;
} typedef mailboxFbReqS;

struct hdmiPixelBufferS {
  u32 pitch;
  u32 xcoursor;
  u32 ycoursor;
  s32 last_prompt_ypos;
  u32 font_color;
  u32 bg_color;
  u32 screen_size;
  u32 screen_size_bytes;
  u32 start;
  bool coursor;
  u32 *buffer;
} typedef hdmiPixelBufferS;

/* structure with pixel buffer */
static hdmiPixelBufferS pixel_buff;
/* pointer to frame buffer address */
static u8 *frame_buff;
static dmaChannelS *dma;
static bool hdmi_initiated = false;


/* PRIVATE FUNCTIONS DECLARATIONS */
void hdmi_set_resolution();
void do_dma(void *dest, void *src, u32 total);
void hdmi_draw_pixel(u32 x, u32 y, u32 color);
void hdmi_draw_char(char c, u32 xpos, u32 ypos);

void new_line();
void character(char c);
void increment_ycoursor(u32 size);
void increment_xcoursor(u32 size);
bool check_fit_yres(u32 size);
bool check_fit_xres(u32 size);
void scroll_down(u32 size);
void erease_char_line(u32 start);
void backspace();
void hdmi_change_color(u32 color, u32 new_color);
void hdmi_draw_coursor();
void erase_next_char();

/* GLOBAL FUNCTIONS DEFINITIONS*/
void hdmi_init(void) {
  dma = dma_open_channel(CT_NORMAL);
  if(dma == NULL) return;
  hdmi_set_resolution();
  hdmi_clear();
  hdmi_initiated = true;
}

void hdmi_startup_info(void) {
  if(!hdmi_initiated) return;
  STARTUP("Hdmi initialized\n");
  STARTUP("Resolution: %dX%d\n", XRESOLUTION, YRESOLUTION);
  STARTUP("Bits per pixel: %d\n", BITS_PER_PIXEL);
}

void hdmi_end(void) {
  if(!hdmi_initiated) return;
  free(pixel_buff.buffer);
  dma_close_channel(dma);
}

void hdmi_refresh(void) {
  if(!hdmi_initiated) return;
  hdmi_draw_coursor();
  u32 second_part = (pixel_buff.start * BUFF_ELEM_SIZE);
  u32 first_part = pixel_buff.screen_size - second_part;
  do_dma(frame_buff, (pixel_buff.buffer + pixel_buff.start), first_part);
  do_dma(frame_buff + first_part, pixel_buff.buffer, second_part);
}

void hdmi_draw_image(const u32 *img, u32 xres, u32 yres, u32 xpos, u32 ypos) {
  if(!hdmi_initiated) return;
  for (u32 i = 0; i < xres; ++i) {
    for (u32 j = 0; j < yres; ++j) {
      hdmi_draw_pixel(xpos+i, ypos+j, img[j*xres + i]);
    }
  }
  hdmi_refresh();
}

void hdmi_printf_char(char c) {
  if(!hdmi_initiated) return;
  if(c == '\n') new_line();
  else if (c == 127 || c == 8) backspace();
  else character(c);
}

void hdmi_printf_string(const char *str) {
  if(!hdmi_initiated) return;
  for (u32 i = 0; str[i] != '\0'; i++) {
    hdmi_printf_char(str[i]);
  }
}

void hdmi_printf_prompt(const char *str) {
  if(!hdmi_initiated) return;
  hdmi_printf_string(str);
  pixel_buff.last_prompt_ypos = pixel_buff.ycoursor;
}

void hdmi_change_font_color(u32 color) {
  if(!hdmi_initiated) return;
  if(color == pixel_buff.bg_color) {
    ERROR("[HDMI ERROR] Cannot set font color to background color\n");
    return;
  }
  hdmi_change_color(pixel_buff.font_color, color);
  pixel_buff.font_color = color;
}

void hdmi_change_bg_color(u32 color) {
  if(!hdmi_initiated) return;
  if(color == pixel_buff.font_color) {
    ERROR("[HDMI ERROR] Cannot set background color to font color\ns");
    return;
  }
  hdmi_change_color(pixel_buff.bg_color, color);
  pixel_buff.bg_color = color;
}

void hdmi_clear(void) {
  if(!hdmi_initiated) return;
  for (u32 i = 0; i < pixel_buff.screen_size_bytes; i++) {
    pixel_buff.buffer[i] = pixel_buff.bg_color;
  }
  pixel_buff.start = 0;
  pixel_buff.xcoursor = 0;
  pixel_buff.ycoursor = 0;
  hdmi_refresh();
}

void hdmi_blink_coursor(void) {
  if(!hdmi_initiated) return;
  pixel_buff.coursor = !(pixel_buff.coursor);
  hdmi_refresh();
}

/* PRIVATE FUNCTIONS DEFINITIONS*/
void do_dma(void *dest, void *src, u32 total) {
  u32 start = 0;
  while(total > 0) {
    int num_bytes = total;
    if (num_bytes > 0xFFFFFF) {
      num_bytes = 0xFFFFFF;
    }
    dma_setup_mem_copy(dma, dest + start, src + start, num_bytes, 2);
    dma_start(dma);
    dma_wait(dma);
    start += num_bytes;
    total -= num_bytes;
  }
}

void hdmi_set_resolution() {
  mailboxFbReqS fb_req = {0};

  fb_req.res.tag.id = MBOX_TAG_FRAMEBUFFER_SET_PHYSICAL_WIDTH_HEIGHT;
  fb_req.res.tag.buffer_size = 8;
  fb_req.res.tag.value_length = 8;
  fb_req.res.xres = XRESOLUTION;
  fb_req.res.yres = YRESOLUTION;

  fb_req.vres.tag.id = MBOX_TAG_FRAMEBUFFER_SET_VIRTUAL_WIDTH_HEIGHT;
  fb_req.vres.tag.buffer_size = 8;
  fb_req.vres.tag.value_length = 8;
  fb_req.vres.xres = XRESOLUTION;
  fb_req.vres.yres = YRESOLUTION;

  fb_req.depth.tag.id = MBOX_TAG_FRAMEBUFFER_SET_DEPTH;
  fb_req.depth.tag.buffer_size = 4;
  fb_req.depth.tag.value_length = 4;
  fb_req.depth.bits_per_pixel = BITS_PER_PIXEL;

  fb_req.buff.tag.id = MBOX_TAG_FRAMEBUFFER_ALLOCATE;
  fb_req.buff.tag.buffer_size = 8;
  fb_req.buff.tag.value_length = 4;
  fb_req.buff.ptr = 16;
  fb_req.buff.screen_size = 0;

  fb_req.pitch.tag.id = MBOX_TAG_FRAMEBUFFER_GET_PITCH;
  fb_req.pitch.tag.buffer_size = 4;
  fb_req.pitch.tag.value_length = 4;
  fb_req.pitch.pitch = 0;

  mailbox_process((mailboxTagS *)&fb_req, sizeof(fb_req));

  /* set pixel buff info */
  pixel_buff.pitch = fb_req.pitch.pitch;
  pixel_buff.font_color = WHITE;
  pixel_buff.bg_color = BLACK;
  pixel_buff.last_prompt_ypos = -1;
  pixel_buff.screen_size = fb_req.buff.screen_size;
  pixel_buff.screen_size_bytes = (fb_req.buff.screen_size / BUFF_ELEM_SIZE);
  pixel_buff.coursor = 0;
  pixel_buff.xcoursor = 0;
  pixel_buff.ycoursor = 0;
  pixel_buff.start = 0;
  pixel_buff.buffer = (u32 *)malloc(pixel_buff.screen_size);

  /* set frame buff address */
  frame_buff = (u8 *)(((u64)fb_req.buff.ptr | 0x40000000) & ~0xC0000000);
}

void hdmi_draw_pixel(u32 x, u32 y, u32 color) {
  u32 pixel_offset = ((x * 4) + ((y * pixel_buff.pitch))) / BUFF_ELEM_SIZE;
  pixel_buff.buffer[(pixel_offset + pixel_buff.start) % pixel_buff.screen_size_bytes] = color;
}

void hdmi_draw_char(char c, u32 xpos, u32 ypos) {
  u32 color;
  for (u32 y = 0; y < FONT_HEIGHT; y++) {
    for (u32 x = 0; x < FONT_WIDTH; x++) {
      color = ((font8x8_basic[(u8)c][y] >> x) & 1) ? pixel_buff.font_color : pixel_buff.bg_color;
      hdmi_draw_pixel(xpos + x, ypos + y, color);
    }
  }
}

void erase_next_char() {
  hdmi_draw_char(' ', pixel_buff.xcoursor, pixel_buff.ycoursor);
}

void new_line() {
  erase_next_char();
  pixel_buff.xcoursor = 0;
  increment_ycoursor(FONT_HEIGHT);
}

bool check_fit_yres(u32 size) {
  return ((size + pixel_buff.ycoursor) < YRESOLUTION);
}

bool check_fit_xres(u32 size) {
  return ((size + pixel_buff.xcoursor) < XRESOLUTION);
}

void increment_ycoursor(u32 size) {
  if(check_fit_yres(size)) pixel_buff.ycoursor += size;
  else scroll_down(size);
}

void increment_xcoursor(u32 size) {
  if (check_fit_xres(size)) pixel_buff.xcoursor += size;
  else new_line();
}

void scroll_down(u32 ypixels) {
  pixel_buff.start = (pixel_buff.start + (XRESOLUTION * ypixels)) % pixel_buff.screen_size_bytes;
  pixel_buff.last_prompt_ypos -= FONT_HEIGHT;
  erease_char_line(pixel_buff.ycoursor);
}

void character(char c) {
  hdmi_draw_char(c, pixel_buff.xcoursor, pixel_buff.ycoursor);
  increment_xcoursor(FONT_WIDTH);
}

void erease_char_line(u32 y) {
  for(u32 x = 0; x < XRESOLUTION; x += FONT_WIDTH) {
    hdmi_draw_char(' ', x, y);
  }
}

bool prompt_line() {
  return pixel_buff.last_prompt_ypos == pixel_buff.ycoursor;
}

bool can_backspace(u32 cond) {
  return pixel_buff.xcoursor >= cond;
}

void backspace() {
  if(prompt_line()){
    if(can_backspace(PROMPT_WIDTH + FONT_WIDTH)){
      erase_next_char();
      pixel_buff.xcoursor -= FONT_WIDTH;
      erase_next_char();
    }
  }
  else {
    if(can_backspace(FONT_HEIGHT)) {
      erase_next_char();
      pixel_buff.xcoursor -= FONT_WIDTH;
      erase_next_char();
    }
    else if (pixel_buff.xcoursor == 0){
      erase_next_char();
      pixel_buff.ycoursor -= FONT_HEIGHT;
      pixel_buff.xcoursor = (XRESOLUTION - FONT_WIDTH);
      erase_next_char();
    }
  }
}

void hdmi_change_color(u32 color, u32 new_color) {
  for(u32 i = 0; i < pixel_buff.screen_size_bytes; i++){
    if(pixel_buff.buffer[i] == color){
      pixel_buff.buffer[i] = new_color;
    }
  }
  hdmi_refresh();
}

void hdmi_draw_coursor() {
  u32 color = pixel_buff.bg_color;
  if(pixel_buff.coursor) color = pixel_buff.font_color;
  for (u32 y = 0; y < FONT_HEIGHT; y++) {
    for (u32 x = 0; x < (FONT_WIDTH / 2); x++) {
      hdmi_draw_pixel(pixel_buff.xcoursor + x, pixel_buff.ycoursor + y, color);
    }
  }
}