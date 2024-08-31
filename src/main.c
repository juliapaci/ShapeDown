#include <raylib.h>
#include <raymath.h>
#include <string.h>

#include <stdio.h>

#include "object.h"
#include "helper.h"
#include "gui.h"

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
    obj.size = (Vector3){1.0f, 1.0f, 1.0f};

    DA objects;
    DA_init(&objects, 50);
    for(int i = 0; i < 15; i++) {
        obj.position.x += 10;
        shader = add_object(&objects, &obj, NO_SELECTION);
    }

    // TODO: a state struct which contains selected, object da, etc.
    ssize_t selected = -1;
    Object *selected_object = NULL;
    struct Control mouse_control = {0};

    while(!WindowShouldClose()) {
        // camera
        if(IsKeyPressed(KEY_LEFT_SHIFT)) {
            if(IsCursorHidden()) EnableCursor();
            else DisableCursor();
        }
        if(IsCursorHidden()) UpdateCamera(&camera, CAMERA_FREE);

        // controls
        DynShader tmp_shader = action_keybinds(&objects, selected);

        if(tmp_shader.shader.id != 0)
            shader = tmp_shader;

        if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            Vector2 pos = IsCursorHidden() ?  // TODO: find better way of position when camera stuff
                            (Vector2){GetScreenWidth()/2.0, GetScreenHeight()/2.0} :
                            GetMousePosition();

            mouse_control = control(selected_object, GetMouseRay(pos, camera));

            if(mouse_control.kind) // editing selected object
                apply_manipulation(&mouse_control, selected_object);
            else { // selecting a new object
                selected = object_at_pos(pos, &camera, &objects);
                // TODO: check if object with selected id exists or find another qway of prevent background colour

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

            BeginMode3D(camera);
                if(selected != NO_SELECTION)
                    draw_gizmos(selected_object);
            EndMode3D();

            // gui hud
            draw_gui(&objects, selected);

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
