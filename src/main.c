#define _POSIX_C_SOURCE 200809L
#include <stdint.h>
#include <string.h>

#include "scene/scene.h"
#include "util/io.h"

int32_t main(const int32_t argc, const char* const* argv) {
    /* Check for the main argument */
    if (argc < 2) {
        fatalf("Not all the arguments have been passed\n"
               "See: %s --help", argv[0]);
        return -1;
    }

    /* Get the title argument (if exists) */
    const char* const title =
        argc == 3 ?
        argv[2] : NULL;

    /* Draw and handle the scene according to the scene type */
    if (strcmp(argv[1], "checklist") == 0)
        return scene_init(CHECKLIST, title);

    /* If the program can't handle the argument */
    fatalf("Invalid first argument\n"
           "See: %s --help", argv[0]);
    return -1;
}
