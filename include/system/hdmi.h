#ifndef _HDMI_H
#define _HDMI_H

#include "types.h"


void hdmi_init(u32 xres, u32 yres, u32 bits_per_pixel);
void hdmi_draw_pixel(u32 x, u32 y, u32 color);
void hdmi_draw_image(const u32 *img, u32 xres, u32 yres, u32 xpos, u32 ypos);

void hdmi_printf_char(char c);
void hdmi_printf_string(const char *str);

#endif /* _HDMI_H */