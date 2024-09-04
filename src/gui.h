#ifndef __GUI_H__
#define __GUI_H__

#include "object.h"
#include <raylib.h>

#define SIDEBAR_WIDTH 300

// is pos inside rect?
bool _rect_contains(Rectangle rect, Vector2 pos);

int16_t draw_gui(DA *objects, int16_t selection);
void _draw_state(Object *obj);
// returns selected id. NO_SELECTION (-1) on none clicked
int16_t _draw_list(uint16_t amount);

// meta
// returns if its clicked on not
bool draw_button(Rectangle rect, const char *label);

#endif // __GUI_H__
