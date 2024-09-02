#include <raylib.h>
#include <raymath.h>
#include <string.h>

#include <stdio.h>

#include "object.h"
#include "helper.h"
#include "gui.h"

int main(void) {
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    SetTraceLogLevel(LOG_ERROR);
    InitWindow(800, 400, "ShapeDown");

    SetTargetFPS(60);

    DynShader shader;
    Camera camera = {
        .position   = (Vector3) {10.0f, 10.0f, 10.0f},
        .target     = (Vector3) {0.0f, 0.0f, 0.0f},
        .up         = (Vector3) {0.0f, 1.0f, 0.0f},
        .fovy       = 45.0f
    };

    Object obj = DEFAULT_OBJECT;
    obj.size = (Vector3){1.0f, 1.0f, 1.0f};

    DA objects;
    DA_init(&objects, 50);

    // TODO: fix first object is invisible for some reason
    shader = add_object(&objects, &obj, NO_SELECTION);

    // TODO: a state struct which contains selected, object da, etc.
    int16_t selected = NO_SELECTION;
    Object *selected_object = NULL;
    struct Control mouse_control = {0};

    while(!WindowShouldClose()) {
        Vector2 mpos = IsCursorHidden() ?
            (Vector2){GetScreenWidth()/2.0, GetScreenHeight()/2.0} :
            GetMousePosition();

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

        if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && mpos.x > SIDEBAR_WIDTH) {
            if(selected != NO_SELECTION)
                mouse_control = control(selected_object, GetMouseRay(mpos, camera));

            if(!mouse_control.kind) {
                selected = object_at_pos(mpos, &camera, &objects);
                // TODO: check if object with selected id exists or find another qway of prevent picking background colour

                selected_object = &objects.array[selected];
            }
        }

        if(IsMouseButtonDown(MOUSE_BUTTON_LEFT) && mouse_control.kind)
            apply_manipulation(&mouse_control, selected_object, GetMouseRay(mpos, camera));

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
            DrawText(TextFormat("%d", (mouse_control.kind - 1) % 3), 1000, 1000, 30, BLUE);
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
