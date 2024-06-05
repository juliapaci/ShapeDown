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

    Object obj = {0};
    obj.size = (Vector3){2.0f, 2.0f, 2.0f};
    // memcpy(&obj.colour, &(struct {uint8_t a,b,c;}){10, 100, 10}, 1);
    obj.colour.r = 100;
    obj.colour.b = 100;
    obj.colour.g = 100;

    DA objects;
    DA_init(&objects, 50);
    DA_push(&objects, &obj);

    object_map(&objects, 0);

    // TODO: a state struct which contains selected, object da, etc.
    size_t selected = -1;

    Ray ray = {0};
    while(!WindowShouldClose()) {
        // camera
        if(IsKeyPressed(KEY_LEFT_CONTROL)) {
            if(IsCursorHidden()) EnableCursor();
            else DisableCursor();
        }
        if(IsCursorHidden()) UpdateCamera(&camera, CAMERA_FREE);

        // keybinds
        if(IsKeyPressed(KEY_SPACE)) {
            DA_push(&objects, &obj);
            shader = object_map(&objects, 0);
        }

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

            update_shader_locations(&shader, camera);

            DrawText(TextFormat("%ld", selected), 1000, 100, 100, BLUE);
            BeginMode3D(camera);
                DrawRay(ray, WHITE);
                DrawGrid(10, 1.0f);
            EndMode3D();

            DrawRectangle(0, 0, SIDEBAR_WIDTH, GetScreenHeight(), GRAY);
        EndDrawing();
    }

    DA_free(&objects);
    CloseWindow();
    return 0;
}
