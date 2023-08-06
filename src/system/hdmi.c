#include "hdmi.h"
#include "mailbox.h"
#include "printf.h"
#include "fonts.h"
#include "mm.h"
#include "dma.h"
#include "debug.h"

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

struct hdmiInfoS {
  u32 pitch;
  u32 xcoursor;
  u32 ycoursor;
  s32 last_prompt_ypos;
  u32 font_color;
  u32 bg_color;
  u32 screen_size;
  u8 *frame_buffer;
} typedef hdmiInfoS;

static hdmiInfoS fb;
static dmaChannelS *dma;
/* pointer to hdmi buffer */
static u32 *hdmi_buffer;
/* index of (0, 0) pixel in hdmi buffer */
static u32 hdmi_buffer_start;

/* PRIVATE FUNCTIONS DECLARATIONS */
void hdmi_set_resolution();
void hdmi_set_buffer();
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

/* GLOBAL FUNCTIONS DEFINITIONS*/
void hdmi_init() {
  dma = dma_open_channel(CT_NORMAL);
  hdmi_set_resolution();
  hdmi_set_buffer();
  hdmi_refresh();
}

void hdmi_end() {
  free(hdmi_buffer);
  dma_close_channel(dma);
}

void hdmi_refresh() {
  u32 second_part = (hdmi_buffer_start * BUFF_ELEM_SIZE);
  u32 first_part = fb.screen_size - second_part;
  do_dma(fb.frame_buffer, (hdmi_buffer + hdmi_buffer_start), first_part);
  do_dma(fb.frame_buffer + first_part, hdmi_buffer, second_part);
}

void hdmi_draw_image(const u32 *img, u32 xres, u32 yres, u32 xpos, u32 ypos) {
  for (u32 i = 0; i < xres; ++i) {
    for (u32 j = 0; j < yres; ++j) {
      hdmi_draw_pixel(xpos+i, ypos+j, img[j*xres + i]);
    }
  }
  hdmi_refresh();
}

void hdmi_printf_char(char c) {
  if(c == '\n') new_line();
  else if (c == 127 || c == 8) backspace();
  else character(c);
}


void hdmi_printf_string(const char *str) {
  for (u32 i = 0; str[i] != '\0'; i++) {
    hdmi_printf_char(str[i]);
  }
}

void hdmi_printf_prompt(const char *str) {
  hdmi_printf_string(str);
  fb.last_prompt_ypos = fb.ycoursor;
}

void hdmi_change_font_color(u32 color) {
  if(color == fb.bg_color) {
    ERROR("[HDMI ERROR] Cannot set font color to background color");
  }
  hdmi_change_color(fb.font_color, color);
  fb.font_color = color;
}

void hdmi_change_bg_color(u32 color) {
  if(color == fb.font_color) {
    ERROR("[HDMI ERROR] Cannot set background color to font color");
  }
  hdmi_change_color(fb.bg_color, color);
  fb.bg_color = color;
}

void hdmi_clear() {
  for (u32 i = 0; i < (fb.screen_size / BUFF_ELEM_SIZE); i++) {
    hdmi_buffer[i] = fb.bg_color;
  }
  hdmi_buffer_start = 0;
  fb.xcoursor = 0;
  fb.ycoursor = 0;
  hdmi_refresh();
}

/* PRIVATE FUNCTIONS DEFINITIONS*/
void hdmi_set_buffer() {
  hdmi_buffer = (u32 *)malloc(fb.screen_size);
  for (u32 i = 0; i < (fb.screen_size / BUFF_ELEM_SIZE); i++) {
    hdmi_buffer[i] = fb.bg_color;
  }
  hdmi_buffer_start = 0;
}

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

  fb.pitch = fb_req.pitch.pitch;
  fb.xcoursor = 0;
  fb.ycoursor = 0;
  fb.font_color = WHITE;
  fb.bg_color = BLACK;
  fb.last_prompt_ypos = -1;
  fb.screen_size = fb_req.buff.screen_size;
  fb.frame_buffer = (u8 *)(((u64)fb_req.buff.ptr | 0x40000000) & ~0xC0000000);
}

void hdmi_draw_pixel(u32 x, u32 y, u32 color) {
  u32 pixel_offset = ((x * 4) + ((y * fb.pitch))) / BUFF_ELEM_SIZE;
  hdmi_buffer[(pixel_offset + hdmi_buffer_start) % (fb.screen_size / BUFF_ELEM_SIZE)] = color;
}

void hdmi_draw_char(char c, u32 xpos, u32 ypos) {
  u32 color;
  for (u32 y = 0; y < FONT_HEIGHT; y++) {
    for (u32 x = 0; x < FONT_WIDTH; x++) {
      color = ((font8x8_basic[(u8)c][y] >> x) & 1) ? fb.font_color : fb.bg_color;
      hdmi_draw_pixel(xpos + x, ypos + y, color);
    }
  }
}

void new_line() {
  fb.xcoursor = 0;
  increment_ycoursor(FONT_HEIGHT);
}

bool check_fit_yres(u32 size) {
  return ((size + fb.ycoursor) < YRESOLUTION);
}

bool check_fit_xres(u32 size) {
  return ((size + fb.xcoursor) < XRESOLUTION);
}

void increment_ycoursor(u32 size) {
  if(check_fit_yres(size)) fb.ycoursor += size;
  else scroll_down(size);
}

void increment_xcoursor(u32 size) {
  if (check_fit_xres(size)) fb.xcoursor += size;
  else new_line();
}

void scroll_down(u32 ypixels) {
  hdmi_buffer_start = (hdmi_buffer_start + (XRESOLUTION * ypixels)) % (fb.screen_size / BUFF_ELEM_SIZE);
  fb.last_prompt_ypos -= FONT_HEIGHT;
  erease_char_line(fb.ycoursor);
}

void character(char c) {
  hdmi_draw_char(c, fb.xcoursor, fb.ycoursor);
  increment_xcoursor(FONT_WIDTH);
}

void erease_char_line(u32 y) {
  for(u32 x = 0; x < XRESOLUTION; x += FONT_WIDTH) {
    hdmi_draw_char(' ', x, y);
  }
}

bool prompt_line() {
  return fb.last_prompt_ypos == fb.ycoursor;
}

bool can_backspace(u32 cond) {
  return fb.xcoursor >= cond;
}

void backspace() {
  if(prompt_line()){
    if(can_backspace(PROMPT_WIDTH + FONT_WIDTH)){
      fb.xcoursor -= FONT_WIDTH;
      hdmi_draw_char(' ', fb.xcoursor, fb.ycoursor);
    }
  }
  else {
    if(can_backspace(FONT_HEIGHT)) {
      fb.xcoursor -= FONT_WIDTH;
      hdmi_draw_char(' ', fb.xcoursor, fb.ycoursor);
    }
    else if (fb.xcoursor == 0){
      fb.ycoursor -= FONT_HEIGHT;
      fb.xcoursor = (XRESOLUTION - FONT_WIDTH);
      hdmi_draw_char(' ', fb.xcoursor, fb.ycoursor);
    }
  }
}

void hdmi_change_color(u32 color, u32 new_color) {
  for(u32 i = 0; i < (fb.screen_size / BUFF_ELEM_SIZE); i++){
    if(hdmi_buffer[i] == color){
      hdmi_buffer[i] = new_color;
    }
  }
  hdmi_refresh();
}