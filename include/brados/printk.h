#ifndef __brados_printk_h__
#define __brados_printk_h__

#include <video/vga.h>

int printk(struct vgastate *term, const char *fmt, ...);

#endif
