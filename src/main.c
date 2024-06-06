#include <raylib.h>
#include <raymath.h>
#include <string.h>

#include "object.h"
#include "helper.h"

#define SIDEBAR_WIDTH 300
#define SELECTED 0

int main(void) {
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(800, 400, "ShapeDown");

    SetTargetFPS(60);

    DynShader shader;
    shader.shader = LoadShader(0, "src/shader.glsl");

    Camera camera = {
        .position   = (Vector3) {10.0f, 10.0f, 10.0f},
        .target     = (Vector3) {0.0f, 0.0f, 0.0f},
        .up         = (Vector3) {0.0f, 1.0f, 0.0f},
        .fovy       = 45.0f
    };

    // TODO: default object thing
    Object obj = {0};
    obj.size = (Vector3){2.0f, 2.0f, 2.0f};
    // memcpy(&obj.colour, &(struct {uint8_t a,b,c;}){10, 100, 10}, 1);
    obj.colour.r = 100;
    obj.colour.b = 100;
    obj.colour.g = 100;

    DA objects;
    DA_init(&objects, 50);
    DA_push(&objects, &obj);

    object_map(&objects, NO_SELECTION);

    // TODO: a state struct which contains selected, object da, etc.
    size_t selected = NO_SELECTION;
    size_t selected_last = NO_SELECTION;

    Ray ray = {0};
    while(!WindowShouldClose()) {
        size_t selected_delta = selected - selected_last;
        selected_last = selected;

        // camera
        if(IsKeyPressed(KEY_LEFT_CONTROL)) {
            if(IsCursorHidden()) EnableCursor();
            else DisableCursor();
        }
        if(IsCursorHidden()) UpdateCamera(&camera, CAMERA_FREE);

        // keybinds
        if(IsKeyPressed(KEY_SPACE))
            shader = add_object(&objects, &obj);

        if(selected_delta)
            object_dynamic_assignment(&shader, objects.array + selected);

        if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            // ray = GetMouseRay(GetMousePosition(), camera);
            selected = object_at_pos(GetMousePosition(), &shader);
        }

        BeginDrawing();
            ClearBackground(BLACK);

            // main shader
            BeginShaderMode(shader.shader);
                DrawRectangle(SIDEBAR_WIDTH, 0,
                    GetScreenWidth() - SIDEBAR_WIDTH, GetScreenHeight(),
                    RAYWHITE
                );
            EndShaderMode();

            update_shader_uniforms(&shader, &camera);

            DrawText(TextFormat("%ld", selected), 1000, 100, 100, BLUE);

            DrawRectangle(0, 0, SIDEBAR_WIDTH, GetScreenHeight(), GRAY);
        EndDrawing();
    }

    DA_free(&objects);
    CloseWindow();
    return 0;
}
