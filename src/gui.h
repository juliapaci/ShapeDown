#ifndef __GUI_H__
#define __GUI_H__

#include "object.h"
#include <raylib.h>

#define IS_PRESSED(rect) (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && _rect_contains(rect, GetMousePosition()))
#define STATE_SELECTION(rect, obj, m)           \
    rect.x += ADVANCE_LABEL(_string_split(#m, string_contains(#m, '.') ? '.' : '>'));\
    if(IS_PRESSED(rect))                        \
    selection = (uint8_t *)&m - (uint8_t *)obj; \
    rect.x -= ADVANCE_LABEL(_string_split(#m, string_contains(#m, '.') ? '.' : '>'));

#include <stdio.h>
#define ADVANCE_LABEL(label) (strlen(label) != 1) * MeasureTextEx(GetFontDefault(), label, INPUT_TEXT_SIZE, 0).x + INPUT_LABEL_PAD;

#define DRAW_TITLE(rect, label)                                     \
    DrawText(label, rect.x, rect.y, STATE_TEXT_SIZE, ACCENT_COLOUR);\
    rect.y += rect.height + STATE_PAD_Y

// TODO: should advance by string len and have just one generic advance macro
// TODO: could find "->" and truncate string for name instead of `obj` param but it would have a runtime cost
#define ADVANCE_INPUT(rect) rect.x += rect.width + 3*STATE_PAD_X
#define CONCLUDE_STATE(rect)            \
    rect.y += STATE_PAD_Y + rect.height;\
    rect.x = STATE_PAD_X

#define DRAW_SINGLE_INPUT(obj, rect, data)  \
    DRAW_TITLE(rect, #data);                \
    _draw_input(rect, #data, obj->data);    \
    STATE_SELECTION(rect, obj, obj->data);  \
    CONCLUDE_STATE(rect)

#define DRAW_STRUCT_3(obj, rect, struct, f1, f2, f3) \
    DRAW_TITLE(rect, #struct);              \
    _draw_input(rect, #f1, obj->struct.f1); \
    STATE_SELECTION(rect, obj, obj->struct.f1);\
    ADVANCE_INPUT(rect);                    \
    _draw_input(rect, #f2, obj->struct.f2); \
    STATE_SELECTION(rect, obj, obj->struct.f2);\
    ADVANCE_INPUT(rect);                    \
    _draw_input(rect, #f3, obj->struct.f3); \
    STATE_SELECTION(rect, obj, obj->struct.f3);\
    CONCLUDE_STATE(rect)

#define DRAW_VEC3(obj, rect, vec3) DRAW_STRUCT_3(obj, rect, vec3, x, y, z)
#define DRAW_COLOUR_RGB(obj, rect, colour) DRAW_STRUCT_3(obj, rect, colour, r, g, b)

#define CHECKBOX(rect, label, value) if(_draw_checkbox(rect, label, value)) value = !value
#define ADVANCE_CHECKBOX(rect) rect.x += rect.width + INPUT_PAD_X
#define DRAW_CHECKBOX(obj, rect, data)  \
    DRAW_TITLE(rect, #data);            \
    CHECKBOX(rect, #data, obj->data);
#define DRAW_TRI_CHECKBOX(obj, rect, data)  \
    DRAW_TITLE(rect, #data);                \
    CHECKBOX(rect, "x", obj->data.x);       \
    ADVANCE_CHECKBOX(rect);                 \
    CHECKBOX(rect, "y", obj->data.y);       \
    ADVANCE_CHECKBOX(rect);                 \
    CHECKBOX(rect, "z", obj->data.z);       \
    ADVANCE_CHECKBOX(rect);                 \
    CONCLUDE_STATE(rect)



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
// second substr seperated by `delim`
const char *_string_split(char *string, char delim);
// `string` contains `c`?
bool string_contains(char *string, char c);

// returns selected shape id or
int16_t draw_gui(DA *objects, int16_t selection);
// returns clicked state fields, NO_SELECTION on none
int8_t _draw_state(Object *obj, uint16_t offset);
// returns selected id. NO_SELECTION on none clicked
int16_t _draw_list(uint16_t amount, int16_t seleciton);

// meta
// returns if its clicked on not
bool _draw_button(Rectangle rect, const char *label);
bool _draw_checkbox(Rectangle rect, const char *label, bool checked);
bool _draw_input(Rectangle rect, const char *label, float value);

#endif // __GUI_H__
