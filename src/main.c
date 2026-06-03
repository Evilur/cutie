#define _POSIX_C_SOURCE 200809L
#include <stdint.h>
#include <string.h>

#include "scene/scene.h"
#include "util/io.h"

int32_t main(const int32_t argc, const char* const* argv) {
    /* Check for the main argument */
    if (argc < 2) {
        seterr("Not all the arguments have been passed\n"
               "See: %s --help", argv[0]);
        printerr();
        return -1;
    }

    /* Get arguments (if exist) */
    const char* const type_str = argv[1];
    const char* const title =
        argc == 3 ?
        argv[2] : NULL;

    /* Get the scene type */
    const enum Scene scene_type =
        strcmp(type_str, "checklist") == 0 ? CHECKLIST : UNDEFINED;

    /* Draw and handle the scene according to the scene type */
    if (scene_init(scene_type, title) == 0) return 0;

    /* On error */
    printerr();
    return -1;
}
