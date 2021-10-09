#ifndef __MAIN_H__
#define __MAIN_H__

enum color_mode { bw = 1, colored = 3 };

struct jpeg_desc;

extern char *filename_out(const char *filename, enum color_mode cm);

#endif
