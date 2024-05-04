#include <raylib.h>

int main(void) {
    InitWindow(800, 400, "ShapeDown");

    while(!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BLACK);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
