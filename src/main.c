#include <raylib.h>

#include "object.h"

#define SIDEBAR_WIDTH 300

int main(void) {
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(800, 400, "ShapeDown");

    SetTargetFPS(60);

    Shader shader = LoadShader(0, "src/shader.glsl");
    int resolution_location = GetShaderLocation(shader, "resolution");
    int view_eye_location = GetShaderLocation(shader, "view_eye");
    int view_center_location = GetShaderLocation(shader, "view_center");

    Camera camera = {
        .position   = (Vector3) {10.0f, 10.0f, 10.0f},
        .target     = (Vector3) {0.0f, 0.0f, 0.0f},
        .up         = (Vector3) {0.0f, 1.0f, 0.0f},
        .fovy       = 45.0f
    };

    Object obj = {0};
    obj.size = (Vector3){2.0f, 2.0f, 2.0f};
    object_map(&obj, "src/shader.glsl");

    DA objects;
    DA_init(&objects, 100);
    DA_push(&objects, &obj);

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
            // object_map(&obj, "src/shader.glsl");
        }

        BeginDrawing();
        ClearBackground(BLACK);

        // main shader
        BeginShaderMode(shader);
            DrawRectangle(SIDEBAR_WIDTH, 0,
                GetScreenWidth() - SIDEBAR_WIDTH, GetScreenHeight(),
                RAYWHITE
            );
        EndShaderMode();

        SetShaderValue(shader, resolution_location,
            (float[2]){(float)GetScreenWidth(), (float)GetScreenHeight()},
            SHADER_UNIFORM_VEC2
        );
        SetShaderValue(shader, view_eye_location,
            &camera.position,
            SHADER_UNIFORM_VEC3
        );
        SetShaderValue(shader, view_center_location,
            &camera.target,
            SHADER_UNIFORM_VEC3
        );

        DrawRectangle(0, 0, SIDEBAR_WIDTH, GetScreenHeight(), GRAY);

        EndDrawing();
    }

    DA_free(&objects);
    CloseWindow();
    return 0;
}
