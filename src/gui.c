#include "gui.h"
#include "helper.h"
#include <raylib.h>

bool _rect_contains(Rectangle rect, Vector2 pos) {
    return rect.x <= pos.x && rect.x + rect.width >= pos.x
        && rect.y <= pos.y && rect.y + rect.height >= pos.y;
}

int16_t draw_gui(DA *objects, int16_t selection) {
    DrawRectangle(0, 0, SIDEBAR_WIDTH, GetScreenHeight(), PRIMARY_COLOUR);
    if(selection != NO_SELECTION)
        _draw_state(&objects->array[selection], (selection + 1) * (BUTTON_PAD_Y + BUTTON_HEIGHT) + BUTTON_PAD_Y);
    return _draw_list(objects->amount, selection);
}

// TODO: could reflect everything to use appropriate draw calls for each type and layout
void _draw_state(Object *obj, uint16_t offset) {
    Rectangle field = {
        .x = STATE_PAD_X,
        .y = offset + STATE_PAD_Y,
        .width = INPUT_WIDTH,
        .height = INPUT_HEIGHT
    };

    DRAW_VEC3(field, obj->position);
    DRAW_VEC3(field, obj->size);
    DRAW_VEC3(field, obj->rotation);
    DRAW_SINGLE_INPUT(field, obj->radius);
    DRAW_SINGLE_INPUT(field, obj->blobbyness);
    field.width = 3*INPUT_SINGLE_WIDTH;
    DRAW_COLOUR_RGB(field, obj->colour);

    Rectangle checkbox = {
        .x = field.x,
        .y = field.y,
        .width = CHECKBOX_SIZE,
        .height = CHECKBOX_SIZE
    };

    DRAW_TRI_CHECKBOX(checkbox, obj->mirror);
    DRAW_CHECKBOX(checkbox, obj->subtract);
}

int16_t _draw_list(uint16_t amount, int16_t selection) {
    int16_t selected = NO_SELECTION;

    Rectangle rect = {
        .x = BUTTON_PAD_X,
        .y = BUTTON_PAD_Y,
        .width = BUTTON_WIDTH,
        .height = BUTTON_HEIGHT
    };

    for(uint16_t i = 0; i < amount; i++) {
        if(_draw_button(rect, TextFormat("Shape %ld", i + 1)))
            selected = i;
        rect.y += rect.height + (i == selection ? SELECTED_SIZE : BUTTON_PAD_Y);
    }

    return selected;
}

bool _draw_button(Rectangle rect, const char *label) {
    DrawRectangleRec(rect, ACCENT_COLOUR);
    DrawText(label, rect.x + TEXT_PAD_X, rect.y + TEXT_PAD_Y, TEXT_SIZE, PRIMARY_COLOUR);

    return IS_PRESSED(rect);
}

bool _draw_checkbox(Rectangle rect, const char *label, bool checked) {
    DrawText(label, rect.x + TEXT_PAD_X, rect.y, CHECKBOX_TEXT_SIZE, ACCENT_COLOUR);
    rect.x += INPUT_LABEL_PAD;

    if(checked)
        DrawRectangleRec(rect, ACCENT_COLOUR);
    DrawRectangleLinesEx(rect, LINE_THICKNESS, ACCENT_COLOUR);

    return IS_PRESSED(rect);
}

bool _draw_input(Rectangle rect, const char *label, float value) {
    DrawText(label, rect.x, rect.y, INPUT_TEXT_SIZE, ACCENT_COLOUR);
    rect.x += INPUT_LABEL_PAD;

    DrawRectangleRec(rect, ACCENT_COLOUR);
    DrawText(TextFormat("%" INPUT_PRECISION "f", value), rect.x + TEXT_PAD_X, rect.y + TEXT_PAD_Y, INPUT_TEXT_SIZE, PRIMARY_COLOUR);

    return IS_PRESSED(rect);
}
