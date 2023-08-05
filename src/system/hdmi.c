#include "hdmi.h"
#include "mailbox.h"
#include "printf.h"

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
  u8 *frame_buffer;
} typedef hdmiInfoS;

static hdmiInfoS fb;

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
  fb.frame_buffer = (u8 *)(((u64)fb_req.buff.ptr | 0x40000000) & ~0xC0000000);
}

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

void hdmi_draw_image(const u32 *img, u32 xres, u32 yres, u32 xpos, u32 ypos) {
  for (int i = 0; i < xres; ++i) {
    for (int j = 0; j < yres; ++j) {
      hdmi_draw_pixel(xpos+i, ypos+j, img[j*xres + i]);
    }
  }
}