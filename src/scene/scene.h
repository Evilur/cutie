#ifndef CUTIE_SCENE_SCENE
#define CUTIE_SCENE_SCENE

#include "checklist.h"
#include "util/term.h"

/* All available scene types */
enum Scene {
    CHECKLIST,
    UNDEFINED
};

/** Create the tui scene and handle user's input
 * @param type The scene type
 * @param title The title if the scene (can be null)
 * @returns scene result on success; -1 on error
 */
static inline int32_t scene_init(enum Scene type, const char* title);

static inline int32_t scene_init(const enum Scene type,
                                 const char* const title) {
    /* Create the tui */
    if (tinit() == -1) return -1;

    /* Create an handle the specific scene */
    int32_t result = -1;
    switch (type) {
        case CHECKLIST:
            result = checklist_init(title);
            break;

        default:
            seterr("Invalid type passed");
            break;
    }

    /* Close the tui */
    tfree();
    return result;
}

#endif
