#include <raylib.h>
#include <raymath.h>
#include <string.h>
#include <float.h>

#include <stdio.h>

#include "object.h"
#include "helper.h"
#include "gui.h"

// TODO: could just keep delta selected in ev loop instead
void set_selected(DA *objects, int16_t *selected, DynShader *shader, Object **selected_object, int16_t selection) {
    *selected = selection;
    *selected_object = &objects->array[*selected];
    // TODO: could just do `object_dynamic_assignment` and make the previously selected object static
    *shader = object_map(objects, *selected, false);
}

int main(void) {
    SetTraceLogLevel(LOG_ERROR);
    SetExitKey(KEY_NULL);
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(800, 400, "ShapeDown");
    SetTargetFPS(60);

    // TODO: fix camera beinga bit offset (for gizmos) (might be a shader camera ray offset)
    Camera camera = {
        .position   = (Vector3) {10.0f, 10.0f, 10.0f},
        .target     = (Vector3) {0.0f, 0.0f, 0.0f},
        .up         = (Vector3) {0.0f, 1.0f, 0.0f},
        .fovy       = 45.0f,
        .projection = CAMERA_PERSPECTIVE
    };
    DA objects;
    DA_init(&objects, 50);
    DynShader shader = object_map(&objects, NO_SELECTION, false);

    // TODO: a state struct which contains selected, object da, etc.
    int16_t selected = NO_SELECTION;
    Object *selected_object = NULL;
    struct Control mouse_control = {0};
    ControlKind state_control = CONTROL_NONE;

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

            if(!mouse_control.kind)
                set_selected(&objects, &selected, &shader, &selected_object, object_at_pos(&objects, mpos, &camera));
        } else if(IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
            mouse_control.kind = CONTROL_NONE;
            state_control = CONTROL_NONE;
        }
        if(IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
            if(mouse_control.kind && mpos.x > SIDEBAR_WIDTH)
                apply_manipulation(&mouse_control, selected_object, GetMouseRay(mpos, camera));
            else if(state_control) {
                if(state_control <= 44)
                    *((float *)selected_object + state_control/sizeof(float)) += GetMouseDelta().x / 100.0;
                else // colour
                    *((uint8_t *)selected_object + state_control) += GetMouseDelta().x / 100.0;
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
                    draw_gizmos(selected_object, mouse_control);
            EndMode3D();

            // gui hud
            int16_t clicked = draw_gui(&objects, selected);
            if(clicked != NO_SELECTION) {
                if(clicked < objects.amount)
                    set_selected(&objects, &selected, &shader, &selected_object, clicked);
                else
                    state_control = (clicked - objects.amount) + 1;
            }

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
