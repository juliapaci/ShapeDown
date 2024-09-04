#include "gui.h"
#include "helper.h"
#include <raylib.h>

bool _rect_contains(Rectangle rect, Vector2 pos) {
    return rect.x <= pos.x && rect.x + rect.width >= pos.x
        && rect.y <= pos.y && rect.y + rect.height >= pos.y;
}

int16_t draw_gui(DA *objects, int16_t selection) {
    DrawRectangle(0, 0, SIDEBAR_WIDTH, GetScreenHeight(), (Color){61, 61, 61, 255});
    _draw_state(&objects->array[selection]);
    return _draw_list(objects->amount);
}

void _draw_state(Object *obj) {

}

int16_t _draw_list(uint16_t amount) {
    int16_t selection = NO_SELECTION;

    Rectangle rect = {
        .x = 10,
        .y = 10,
        .width = 200,
        .height = 30
    };

    for(uint16_t i = 0; i < amount; i++) {
        if(draw_button(rect, TextFormat("Shape %ld", i + 1)))
            selection = i;
        rect.y += rect.height + 10;
    }

    return selection;
}

bool draw_button(Rectangle rect, const char *label) {
    DrawRectangleRec(rect, (Color){70, 70, 70, 255});
    DrawText(label, rect.x + 2, rect.y + 2, 30, (Color){61, 61, 61, 255});
    return IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && _rect_contains(rect, GetMousePosition());
}
