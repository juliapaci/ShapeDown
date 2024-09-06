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

    // TODO: can maybe do some reflection here

    // pos
    DrawText("position", field.x, field.y, STATE_TEXT_SIZE, ACCENT_COLOUR);
    field.y += field.height + STATE_PAD_Y;
    _draw_input(field, "x", obj->position.x);
    field.x += field.width + 3*STATE_PAD_X;
    _draw_input(field, "y", obj->position.y);
    field.x += field.width + 3*STATE_PAD_X;
    _draw_input(field, "z", obj->position.z);

    field.y += STATE_PAD_Y + field.height;
    field.x = STATE_PAD_X;

    // size
    DrawText("size", field.x, field.y, STATE_TEXT_SIZE, ACCENT_COLOUR);
    field.y += field.height + STATE_PAD_Y;
    _draw_input(field, "x", obj->size.x);
    field.x += field.width + INPUT_PAD_X;
    _draw_input(field, "y", obj->size.y);
    field.x += field.width + INPUT_PAD_X;
    _draw_input(field, "z", obj->size.z);

    field.y += STATE_PAD_Y + field.height;
    field.x = STATE_PAD_X;

    // rotation
    DrawText("rotation", field.x, field.y, STATE_TEXT_SIZE, ACCENT_COLOUR);
    field.y += field.height + STATE_PAD_Y;
    _draw_input(field, "x", obj->rotation.x);
    field.x += field.width + INPUT_PAD_X;
    _draw_input(field, "y", obj->rotation.y);
    field.x += field.width + INPUT_PAD_X;
    _draw_input(field, "z", obj->rotation.z);

    field.y += STATE_PAD_Y + field.height;
    field.x = STATE_PAD_X;

    // radius
    DrawText("radius", field.x, field.y, STATE_TEXT_SIZE, ACCENT_COLOUR);
    field.y += field.height + STATE_PAD_Y;
    _draw_input(field, "r", obj->radius);

    field.y += STATE_PAD_Y + field.height;
    field.x = STATE_PAD_X;

    // blobbyness
    DrawText("blobbyness", field.x, field.y, STATE_TEXT_SIZE, ACCENT_COLOUR);
    field.y += field.height + STATE_PAD_Y;
    _draw_input(field, "b", obj->blobbyness);

    field.y += STATE_PAD_Y + field.height;
    field.x = STATE_PAD_X;

    // colour
    field.width = INPUT_SINGLE_WIDTH;
    DrawText("colour", field.x, field.y, STATE_TEXT_SIZE, ACCENT_COLOUR);
    field.y += field.height + STATE_PAD_Y;
    _draw_input(field, "r", obj->colour.r);
    field.x += field.width + INPUT_PAD_X;
    _draw_input(field, "g", obj->colour.g);
    field.x += field.width + INPUT_PAD_X;
    _draw_input(field, "b", obj->colour.b);

    field.y += STATE_PAD_Y + field.height;
    field.x = STATE_PAD_X;

    Rectangle checkbox = {
        .x = field.x,
        .y = field.y,
        .width = CHECKBOX_SIZE,
        .height = CHECKBOX_SIZE
    };

    // mirror
    DrawText("mirror", checkbox.x, checkbox.y, STATE_TEXT_SIZE, ACCENT_COLOUR);
    checkbox.y += checkbox.height + STATE_PAD_Y;
    obj->mirror.x = _draw_checkbox(checkbox, "x", obj->mirror.x);
    checkbox.x += checkbox.width + INPUT_PAD_X;
    obj->mirror.y = _draw_checkbox(checkbox, "y", obj->mirror.y);
    checkbox.x += checkbox.width + INPUT_PAD_X;
    obj->mirror.z = _draw_checkbox(checkbox, "z", obj->mirror.z);

    checkbox.y += STATE_PAD_Y + checkbox.height;
    checkbox.x = STATE_PAD_X;

    // subtract
    DrawText("subtract", checkbox.x, checkbox.y, STATE_TEXT_SIZE, ACCENT_COLOUR);
    checkbox.y += checkbox.height + STATE_PAD_Y;
    obj->subtract = _draw_checkbox(checkbox, "s", obj->subtract);
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
