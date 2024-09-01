#include "gui.h"
#include <raylib.h>

void draw_gui(DA *objects, int16_t selection) {
    DrawRectangle(0, 0, SIDEBAR_WIDTH, GetScreenHeight(), (Color){61, 61, 61, 255});
    _draw_state(&objects->array[selection]);
    _draw_list(objects->amount);

}

void _draw_state(Object *obj) {

}

void _draw_list(uint16_t amount) {
    Rectangle rect = {
        .x = 10,
        .y = 10,
        .width = 200,
        .height = 30
    };

    for(uint16_t i = 0; i < amount; i++) {
        draw_button(rect, TextFormat("Shape %ld", i + 1));
        rect.y += rect.height + 10;
    }
}

void draw_button(Rectangle rect, const char *label) {
    DrawRectangleRec(rect, (Color){70, 70, 70, 255});
    DrawText(label, rect.x + 2, rect.y + 2, 30, (Color){61, 61, 61, 255});
}
