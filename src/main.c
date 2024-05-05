#include <raylib.h>

#include "object.h"

int main(void) {
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(800, 400, "ShapeDown");

    SetTargetFPS(60);

    Camera camera = {
        .position   = (Vector3) {10.0f, 10.0f, 10.0f},
        .target     = (Vector3) {0.0f, 0.0f, 0.0f},
        .up         = (Vector3) {0.0f, 1.0f, 0.0f},
        .fovy       = 45.0f
    };

    Object obj = {0};
    obj.size = (Vector3){2.0f, 2.0f, 2.0f};

    DA objects;
    DA_init(&objects, 100);
    DA_push(&objects, &obj);

    while(!WindowShouldClose()) {
        // camera
        if(IsKeyPressed(KEY_LEFT_CONTROL)) {
            if(IsCursorHidden()) EnableCursor();
            else DisableCursor();
        }
        if(IsCursorHidden()) UpdateCamera(&camera, CAMERA_FIRST_PERSON);

        // keybinds
        if(IsKeyPressed(KEY_SPACE))
            DA_push(&objects, &obj);

        BeginDrawing();
        ClearBackground(BLACK);
        BeginMode3D(camera);
            for(size_t i = 0; i < objects.amount; i++) {
                Object obj = objects.array[i];
                DrawCube(obj.position,
                    obj.size.x,
                    obj.size.y,
                    obj.size.z,
                    GRAY
                );
            }
            DrawGrid(10, 1.0);
        EndMode3D();
        EndDrawing();
    }

    DA_free(&objects);
    CloseWindow();
    return 0;
}
