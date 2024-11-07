#define NOBUILD_IMPLEMENTATION
#include "nob.h"

#define CFLAGS "-Wall", "-Wextra", "-ggdb"
#define LDFLAGS "-I./include/raylib/src",   \
    "-L./include/raylib/src", "-lraylib",   \
    "-Wl,-rpath=./include/raylib/src",      \
    "-lm"
#define LDFLAGS_DELIM "\", \""

void build_raylib(void) {
    Cstr cwd = GETCWD();
    SETCWD(PATH(cwd, "include", "raylib", "src"));

    CMD("make", "PLATFORM=PLATFORM_DESKTOP");

    SETCWD(cwd);
}

// NOTE: very bare bones just so clangd can pick up on stuff
void create_compile_commands(void) {
    FILE *json = fopen("compile_commands.json", "w");

    const char *ldflags_array[] = {LDFLAGS};
    const size_t ldflags_amount = sizeof(ldflags_array)/sizeof(char *);
    size_t ldflags_size = 0;
    for(size_t i = 0; i < ldflags_amount; i++)
        ldflags_size += strlen(ldflags_array[i]) + strlen(LDFLAGS_DELIM);

    char *ldflags_args = malloc(ldflags_size);
    ldflags_args[0] = '\0';
    for(size_t i = 0; i < ldflags_amount; i++) {
        ldflags_args = strcat(ldflags_args, LDFLAGS_DELIM);
        ldflags_args = strcat(ldflags_args, ldflags_array[i]);
    }

    fprintf(json,
        "["                                     "\n"
        "\t"    "{"                             "\n"
        "\t\t"      "\"directory\": \"%s\","    "\n"
        // stupid formatting but works cause order of appending LDFLAGS_DELIM
        "\t\t"      "\"arguments\": [\"/usr/bin/cc%s\"],"   "\n"
        "\t\t"      "\"file\": \"N/A\""         "\n"
        "\t"    "}"                             "\n"
        "]",
        GETCWD(),
        ldflags_args
    );

    free(ldflags_args);
    fclose(json);
}

int main(int argc, char *argv[]) {
    GO_REBUILD_URSELF(argc, argv);

    create_compile_commands();
    build_raylib();
    CMD("cc", CFLAGS, "-o", "ShapeDown", "src/main.c", "src/object.c", "src/helper.c", "src/gui.c", LDFLAGS);

    return 0;
}
