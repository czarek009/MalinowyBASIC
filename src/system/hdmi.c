#include "hdmi.h"
#include "mailbox.h"
#include "printf.h"
#include "fonts.h"
#include "mm.h"

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
  u32 xres;
  u32 yres;
  u32 pitch;
  u32 bpp;
  u32 xcoursor;
  u32 ycoursor;
  u32 font_color;
  u8 *frame_buffer;
} typedef hdmiInfoS;

static hdmiInfoS fb;

/* PRIVATE FUNCTIONS DECLARATIONS */
void hdmi_draw_pixel(u32 x, u32 y, u32 color);
void hdmi_draw_char(char c, u32 xpos, u32 ypos, u32 color);
void increment_xcoursor();
void increment_ycoursor();
void reset_xcoursor();
void new_line();
void backspace();
bool check_font_fit_width();
bool check_font_fit_height();
void scroll_down();
void hdmi_erase_char(u32 xpos, u32 ypos);

/* GLOBAL FUNCTIONS DEFINITIONS*/
void hdmi_init(u32 xres, u32 yres, u32 bits_per_pixel) {
  mailboxFbReqS fb_req = {0};

  fb_req.res.tag.id = MBOX_TAG_FRAMEBUFFER_SET_PHYSICAL_WIDTH_HEIGHT;
  fb_req.res.tag.buffer_size = 8;
  fb_req.res.tag.value_length = 8;
  fb_req.res.xres = xres;
  fb_req.res.yres = yres;

  fb_req.vres.tag.id = MBOX_TAG_FRAMEBUFFER_SET_VIRTUAL_WIDTH_HEIGHT;
  fb_req.vres.tag.buffer_size = 8;
  fb_req.vres.tag.value_length = 8;
  fb_req.vres.xres = xres;
  fb_req.vres.yres = yres;

  fb_req.depth.tag.id = MBOX_TAG_FRAMEBUFFER_SET_DEPTH;
  fb_req.depth.tag.buffer_size = 4;
  fb_req.depth.tag.value_length = 4;
  fb_req.depth.bits_per_pixel = bits_per_pixel;

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

  fb.xres = fb_req.vres.xres;
  fb.yres = fb_req.vres.yres;
  fb.pitch = fb_req.pitch.pitch;
  fb.bpp = fb_req.depth.bits_per_pixel;
  fb.xcoursor = 0;
  fb.ycoursor = 0;
  fb.font_color = 0xFFFFFFFF;
  fb.frame_buffer = (u8 *)(((u64)fb_req.buff.ptr | 0x40000000) & ~0xC0000000);
}

void hdmi_printf_char(char c) {
  if(c == '\n') {
    new_line();
    return;
  }
  if (c == 127 || c == 8) {
    backspace();
    return;
  }
  if (!check_font_fit_width()){
    new_line();
  }
  hdmi_draw_char(c, fb.xcoursor, fb.ycoursor, fb.font_color);
  increment_xcoursor();
}

void hdmi_printf_string(const char *str) {
  for (u32 i = 0; str[i] != '\0'; i++) {
    hdmi_printf_char(str[i]);
  }
}

void hdmi_draw_image(const u32 *img, u32 xres, u32 yres, u32 xpos, u32 ypos) {
  for (u32 i = 0; i < xres; ++i) {
    for (u32 j = 0; j < yres; ++j) {
      hdmi_draw_pixel(xpos+i, ypos+j, img[j*xres + i]);
    }
  }
}

/* PRIVATE FUNCTIONS DEFINITIONS*/
void hdmi_draw_pixel(u32 x, u32 y, u32 color) {
  volatile u8 *frame_buffer = fb.frame_buffer;
  u32 pixel_offset = (x * 4) + (y * fb.pitch);

  if (fb.bpp == 32) {
    u8 r = (color & 0xFF000000) >> 24;
    u8 g = (color & 0xFF0000) >> 16;
    u8 b = (color & 0xFF00) >> 8;
    u8 a = color & 0xFF;

    frame_buffer[pixel_offset++] = b;
    frame_buffer[pixel_offset++] = g;
    frame_buffer[pixel_offset++] = r;
    frame_buffer[pixel_offset++] = a;
  } else {
    frame_buffer[pixel_offset++] = (color >> 8) & 0xFF;
    frame_buffer[pixel_offset++] = (color & 0xFF);
  }
}

void hdmi_draw_char(char c, u32 xpos, u32 ypos, u32 color) {
  for (u32 i = 0; i < FONT_HEIGHT; i++) {
    for (u32 j = 0; j < FONT_WIDTH; j++) {
      if((font8x8_basic[(u8)c][i] >> j) & 1) {
        hdmi_draw_pixel(xpos + j, ypos + i, color);
      }
    }
  }
}

void increment_ycoursor() {
  if(check_font_fit_height()) {
    fb.ycoursor += FONT_HEIGHT;
    return;
  }
  scroll_down();
}

void increment_xcoursor() {
  if (check_font_fit_width()) {
    fb.xcoursor += FONT_WIDTH;
    return;
  }
  new_line();
}

void reset_xcoursor() {
  fb.xcoursor = 0;
}

bool check_font_fit_width() {
  return ((FONT_WIDTH + fb.xcoursor) < fb.xres);
}

bool check_font_fit_height() {
  return ((FONT_HEIGHT + fb.ycoursor) < fb.yres);
}

void new_line() {
  reset_xcoursor();
  increment_ycoursor();
}

void backspace() {
  if(fb.xcoursor >= PROMPT_WIDTH + FONT_WIDTH) {
    fb.xcoursor -= FONT_WIDTH;
    hdmi_erase_char(fb.xcoursor, fb.ycoursor);
  }
}

void hdmi_erase_char(u32 xpos, u32 ypos) {
  for (u32 i = 0; i < FONT_HEIGHT; i++) {
    for (u32 j = 0; j < FONT_WIDTH; j++) {
      hdmi_draw_pixel(xpos + j, ypos + i, 0x00000000);
    }
  }
}

void erease_last_line() {
  for(u32 i = 0; i < fb.xres; i += FONT_WIDTH) {
    hdmi_erase_char(i, fb.ycoursor);
  }
}

void scroll_down() {
  u32 line_font_bytes = (FONT_HEIGHT * fb.pitch);
  memcpy(fb.frame_buffer, (fb.frame_buffer + line_font_bytes), (fb.yres * fb.pitch) - line_font_bytes);
  erease_last_line();
}