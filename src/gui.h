#ifndef __GUI_H__
#define __GUI_H__

#include "object.h"
#include <raylib.h>

#define SIDEBAR_WIDTH 300

void draw_gui(DA *objects, int16_t selection);

void _draw_state(Object *obj);
void _draw_list(uint16_t amount);

// meta
void draw_button(Rectangle rect, const char *label);

#endif // __GUI_H__
