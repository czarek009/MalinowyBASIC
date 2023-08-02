#ifndef _HDMI_H
#define _HDMI_H

#include "types.h"

#define XRESOLUTION    640
#define YRESOLUTION    400
#define BITS_PER_PIXEL 32




void hdmi_init();
void hdmi_set_resolution();
// void hdmi_draw_image(const u32 *img, u32 xres, u32 yres, u32 xpos, u32 ypos);
// void hdmi_printf_char(char c);
// void hdmi_printf_string(const char *str);

#endif /* _HDMI_H */