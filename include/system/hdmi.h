#ifndef _HDMI_H
#define _HDMI_H

#include "types.h"

#define XRESOLUTION    640
#define YRESOLUTION    400
#define BITS_PER_PIXEL 32

void hdmi_init(void);
void hdmi_startup_info(void);
void hdmi_end(void);
void hdmi_refresh(void);
void hdmi_draw_image(const u32 *img, u32 xres, u32 yres, u32 xpos, u32 ypos);
void hdmi_printf_char(char c);
void hdmi_printf_string(const char *str);
void hdmi_printf_prompt(const char *str);
void hdmi_change_font_color(u32 color);
void hdmi_change_bg_color(u32 color);
void hdmi_clear(void);
void hdmi_blink_coursor(void);

#endif /* _HDMI_H */