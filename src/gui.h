#ifndef __GUI_H__
#define __GUI_H__

#include "object.h"
#include <raylib.h>

#define IS_PRESSED(rect) (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && _rect_contains(rect, GetMousePosition()))

#define PRIMARY_COLOUR (Color){61, 61, 61, 255}
#define ACCENT_COLOUR (Color){70, 70, 70, 255}
#define SELECTED_COLOUR WHITE

#define SIDEBAR_WIDTH 300
#define SELECTED_SIZE 500

#define BUTTON_PAD_X 10
#define BUTTON_PAD_Y 10
#define BUTTON_WIDTH 200
#define BUTTON_HEIGHT 30

#define TEXT_PAD_X 2
#define TEXT_PAD_Y 2
#define TEXT_SIZE 30

#define LINE_THICKNESS 1

#define STATE_PAD_X BUTTON_PAD_X
#define STATE_PAD_Y BUTTON_PAD_Y
#define STATE_TEXT_SIZE TEXT_SIZE

#define CHECKBOX_SIZE 20
#define CHECKBOX_TEXT_SIZE 20

#define INPUT_SINGLE_WIDTH 15
#define INPUT_WIDTH 65
#define INPUT_HEIGHT 20
// TODO: get text size from width and ehigth (and maybe calculate precision based on width or something)
#define INPUT_TEXT_SIZE CHECKBOX_TEXT_SIZE
#define INPUT_LABEL_PAD 20
#define INPUT_PRECISION ".4"
#define INPUT_PAD_X 3*STATE_PAD_X

// is pos inside rect?
bool _rect_contains(Rectangle rect, Vector2 pos);

int16_t draw_gui(DA *objects, int16_t selection);
void _draw_state(Object *obj, uint16_t offset);
// returns selected id. NO_SELECTION on none clicked
int16_t _draw_list(uint16_t amount, int16_t seleciton);

// meta
// returns if its clicked on not
bool _draw_button(Rectangle rect, const char *label);
bool _draw_checkbox(Rectangle rect, const char *label, bool checked);
bool _draw_input(Rectangle rect, const char *label, float value);

#endif // __GUI_H__
