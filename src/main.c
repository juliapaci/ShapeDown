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
    Camera camera = {
        .position   = (Vector3) {10.0f, 10.0f, 10.0f},
        .target     = (Vector3) {0.0f, 0.0f, 0.0f},
        .up         = (Vector3) {0.0f, 1.0f, 0.0f},
        .fovy       = 45.0f
    };

    // TODO: default object thing
    Object obj = {0};
    obj.size = (Vector3){2.0f, 2.0f, 2.0f};
    obj.colour.r = 25;
    obj.colour.g = 100;
    obj.colour.b = 000;

    DA objects;
    DA_init(&objects, 50);
    for(int i = 0; i < 15; i++) {
        obj.position.x += 10;
        shader = add_object(&objects, &obj, NO_SELECTION);
    }

    // TODO: a state struct which contains selected, object da, etc.
    size_t selected = 0;
    Object *selected_object = &objects.array[selected];
    struct Control mouse_control = {0};

    while(!WindowShouldClose()) {
        // camera
        if(IsKeyPressed(KEY_LEFT_SHIFT)) {
            if(IsCursorHidden()) EnableCursor();
            else DisableCursor();
        }
        if(IsCursorHidden()) UpdateCamera(&camera, CAMERA_FREE);

        // keybinds
        if(IsKeyPressed(KEY_SPACE))
            shader = add_object(&objects, &obj, selected);

        if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            Vector2 pos = IsCursorHidden() ?  // TODO: find better way of position when camera stuff
                            (Vector2){GetScreenWidth()/2.0, GetScreenHeight()/2.0} :
                            GetMousePosition();

            mouse_control = control(selected_object, GetMouseRay(pos, camera));

            if(mouse_control.kind)
                apply_manipulation(&mouse_control, selected_object);
            else {
                selected = object_at_pos(pos, &camera, &objects);

                selected_object = &objects.array[selected];
            }
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

            DrawText(TextFormat("s: %ld", selected), 1000, 100, 100, BLUE);
            DrawText(TextFormat("d: %ld", selected_object->position.x), 1000, 900, 100, BLUE);

            BeginMode3D(camera);
                if(selected != NO_SELECTION)
                    draw_gizmos(selected_object);
            EndMode3D();

            // gui hud
            DrawRectangle(0, 0, SIDEBAR_WIDTH, GetScreenHeight(), (Color){61, 61, 61, 255});

            if(IsCursorHidden())
                DrawCircle(GetScreenWidth()/2.0, GetScreenHeight()/2.0, 5.0, RAYWHITE);
        EndDrawing();

        update_shader_uniforms(&shader, &camera);
        if(selected != NO_SELECTION)
            object_dynamic_assignment(&shader, selected_object);
    }

    DA_free(&objects);
    CloseWindow();
    return 0;
}
