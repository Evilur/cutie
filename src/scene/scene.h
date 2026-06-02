#ifndef CUTIE_SCENE_SCENE
#define CUTIE_SCENE_SCENE

#include "util/io.h"
#include "checklist.h"

/* All available scene types */
enum Scene {
    CHECKLIST
};

/** Create the TUI scene and handle user's input
 * @param type The scene type
 * @param title The title if the scene (can be null)
 * @returns 0 on success; -1 on error
 */
static inline int32_t handle_scene(const enum Scene type,
                               const char* const title) {
    /* Open the tty descriptor */
    ttyinit();

    /* Exit the cannonical mode */
    ttycannonoff();

    /* Create an handle the specific scene */
    int32_t result = -1;
    switch (type) {
        case CHECKLIST:
            result = handle_checklist(title);
            break;
    }

    /* Restore the old terminal mode */
    ttyreset();

    /* Close the tty file desciptor */
    ttyfree();
    return 0;
}

#endif
