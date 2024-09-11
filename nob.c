#define NOBUILD_IMPLEMENTATION
#include "nob.h"

#define CFLAGS "-Wall", "-Wextra", "-ggdb"
#define LDFLAGS "-I./include/raylib/src",   \
    "-L./include/raylib/src", "-lraylib",   \
    "-Wl,-rpath=./include/raylib/src",      \
    "-lm"

void build_raylib(void) {
    Cstr cwd = GETCWD();
    SETCWD(PATH(cwd, "include", "raylib", "src"));

    CMD("make", "PLATFORM=PLATFORM_DESKTOP");

    SETCWD(cwd);
}

int main(int argc, char *argv[]) {
    GO_REBUILD_URSELF(argc, argv);

    build_raylib();
    CMD("cc", CFLAGS, "-o", "ShapeDown", "src/main.c", "src/object.c", "src/helper.c", "src/gui.c", LDFLAGS);

    return 0;
}
