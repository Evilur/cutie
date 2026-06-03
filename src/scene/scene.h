#ifndef CUTIE_SCENE_SCENE
#define CUTIE_SCENE_SCENE

#include "checklist.h"
#include "util/tui.h"

/* All available scene types */
enum Scene {
    CHECKLIST
};

/** Create the TUI scene and handle user's input
 * @param type The scene type
 * @param title The title if the scene (can be null)
 * @returns 0 on success; -1 on error
 */
static inline int32_t scene_init(const enum Scene type,
                               const char* const title) {
    /* Create the tui */
    tuiinit();

    /* Create an handle the specific scene */
    int32_t result = -1;
    switch (type) {
        case CHECKLIST:
            result = checklist_init(title);
            break;
    }

    /* Close the tui */
    tuifree();
    return result;
}

#endif
