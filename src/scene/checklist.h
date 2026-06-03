#ifndef CUTIE_SCENE_CHECKLIST
#define CUTIE_SCENE_CHECKLIST

#include <stdint.h>
#include <string.h>

#include "util/tui.h"
#include "type/bool.h"

/** Create the checklist scene and handle it
 * @param title The title of the checklist
 * @returns 0 on success; -1 on error
 */
static inline int32_t checklist_init(const char* const title) {
    /* Alloc the memory for option names */
    int32_t names_off = 0, names_size = 128;
    char* names = malloc(names_size);
    if (names == NULL) {
        seterr("Error: can't allocate the memory");
        return -1;
    }

    /* Alloc the memory for option descs */
    int32_t descs_off = 0, descs_size = 128;
    char* descs = malloc(descs_size);
    if (descs == NULL) {
        seterr("Error: can't allocate the memory");
        return -1;
    }

    /* Alloc the memory for option descs */
    int16_t stats_off= 0, stats_size = 8;
    bool* stats = malloc(names_size);
    if (stats == NULL) {
        seterr("Error: can't allocate the memory");
        return -1;
    }

    /* Init buffers to store options */
    char name_buffer[128];
    char desc_buffer[1024];
    char stat_buffer[8];

    /* Get choose options from the STDIN */
    uint16_t opts_num = 0;
    for (;;) {

        /* Read options */
        if (fgets(name_buffer, sizeof(name_buffer), stdin) == NULL) break;
        if (fgets(desc_buffer, sizeof(desc_buffer), stdin) == NULL) break;
        if (fgets(stat_buffer, sizeof(stat_buffer), stdin) == NULL) break;

        /* Get the pointer to the end of the every buffer */
        char* name_buffer_end =
            memchr(name_buffer, '\n', sizeof(name_buffer));
        char* desc_buffer_end =
            memchr(desc_buffer, '\n', sizeof(desc_buffer));
        char* stat_buffer_end =
            memchr(stat_buffer, '\n', sizeof(stat_buffer));
        if (name_buffer_end == NULL ||
            desc_buffer_end == NULL ||
            stat_buffer_end == NULL) {
            fprintf(stderr, RED"Error: too large input:\n"
                    "%s\n",
                    name_buffer_end == NULL ? name_buffer :
                    desc_buffer_end == NULL ? desc_buffer :
                    stat_buffer);
            return -1;
        }

        /* Remove \n chars */
        *name_buffer_end = '\0';
        *desc_buffer_end = '\0';
        *stat_buffer_end = '\0';

        /* Save the name to the dynamic array */
        const uint16_t name_len = name_buffer_end - name_buffer;
        while (name_len + names_off > names_size) {
            names_size *= 2;
            names = realloc(names, names_size);
            if (names == NULL) {
                fprintf(stderr, RED"Error: can't allocate the memory\n");
                return -1;
            }
        }
        memcpy(names + names_off, name_buffer, name_len + 1);
        names_off += name_len + 1;

        /* Save the desc to the dynamic array */
        const uint16_t desc_len = desc_buffer_end - desc_buffer;
        while (desc_len + descs_off > descs_size) {
            descs_size *= 2;
            descs = realloc(descs, descs_size);
            if (descs == NULL) {
                fprintf(stderr, RED"Error: can't allocate the memory\n");
                return -1;
            }
        }
        memcpy(descs + descs_off, desc_buffer, desc_len + 1);
        descs_off += desc_len + 1;

        /* Check the stat */
        bool enabled = strcmp(stat_buffer, "ON") == 0;

        /* Save the stat to the dynamic array */
        if (stats_off >= stats_size) {
            stats_size *= 2;
            stats = realloc(stats, stats_size);
            if (stats == NULL) {
                fprintf(stderr, RED"Error: can't allocate the memory\n");
                return -1;
            }
        }
        stats[stats_off] = enabled;
        ++stats_off;

        /* Increase the number of options */
        ++opts_num;
    }

    /* Create an array for storing all the option names */
    const char* names_org[opts_num];
    const char* names_ptr = names;
    for (uint16_t i = 0; i < opts_num; ++i) {
        names_org[i] = names_ptr;
        names_ptr = strchr(names_ptr, '\0');
        if (names_ptr == NULL) {
            fprintf(stderr, RED"Error: memory corrupt\n");
            return -1;
        } else ++names_ptr;
    }

    /* Create an array for storing all the option descs */
    const char* descs_org[opts_num];
    const char* descs_ptr = descs;
    for (uint16_t i = 0; i < opts_num; ++i) {
        descs_org[i] = descs_ptr;
        descs_ptr = strchr(descs_ptr, '\0');
        if (descs_ptr == NULL) {
            fprintf(stderr, RED"Error: memory corrupt\n");
            return -1;
        } else ++descs_ptr;
    }

    /* If there is no options */
    if (opts_num == 0) return 0;

    /* Get the number of options for show on the screen */
    const uint16_t free_height = tuiheight() - 4;
    const uint16_t cur_opts_num = free_height < opts_num ?
                                  free_height : opts_num;

    /* Print the title */
    if (title == NULL) tuiprintf(T(MAGENTA)"Choose:\n");
    else tuiprintf(T(MAGENTA)"%s:\n", title);

#define REDRAW_LINE(S)                                                         \
    tuiprintf("\r  %s %s - %s"S,                                               \
              stats[current_index] ? T(RESET,BLUE)"✓" : T(RESET,WHITE)"•",     \
              names_org[current_index], descs_org[current_index]);
#define REDRAW_CLINE(S)                                                        \
    tuiprintf(T(BOLD, ITALIC, BLUE)"\r> %s %s - %s"S,                          \
              stats[current_index] ? "✓" : "•",                                \
              names_org[current_index], descs_org[current_index]);

    /* Print the options */
    int16_t current_index = 0;
    REDRAW_CLINE("\n");
    ++current_index;
    for (int16_t i = 1; i < cur_opts_num; ++i) {
        REDRAW_LINE("\n");
        ++current_index;
    }

    /* Print the key bindings */
    tuiprintf(
        "\n"
        T(BOLD, BRIGHT_BLACK)"X/Space "T(RESET, BRIGHT_BLACK)"toggle   "
        T(BOLD, BRIGHT_BLACK)"hjkl "T(RESET, BRIGHT_BLACK)"navigate   "
        T(BOLD, BRIGHT_BLACK)"Enter "T(RESET, BRIGHT_BLACK)"submit   "
        T(BOLD, BRIGHT_BLACK)"Ctrl+A "T(RESET, BRIGHT_BLACK)"toggle all\n"
        T(BOLD, BRIGHT_BLACK)"/       "T(RESET, BRIGHT_BLACK)"find     "
        T(BOLD, BRIGHT_BLACK)"N    "T(RESET, BRIGHT_BLACK)"next       "
        T(BOLD, BRIGHT_BLACK)"P     "T(RESET, BRIGHT_BLACK)"prev     "
        T(BOLD, BRIGHT_BLACK)"F      "T(RESET, BRIGHT_BLACK)"filter"
    );

    /* Get the scrollable part */
    const int16_t bottom = tuiy() - 3;
    const int16_t top = bottom - cur_opts_num + 1;

    /* Make only this part scrollable */
    tuisetsc(top, bottom);

    /* Flush */
    tuiflush();

    uint8_t input;
    current_index = 0;
    for (;;) {
        /* Get the input */
        const char input = ttygetchar();

/* Macro to clear the scene */
#define CLEAR_SCENE() {                                                        \
        tuiressc();                                                            \
        tuigoto(1, top - 1);                                                   \
        tuicldown();                                                           \
}

        /* Check for <Escape> and 'q' */
        if (input == 27 || input == 'q') {
            /* Clear the scene */
            CLEAR_SCENE();
            return -1;
        }

        /* Check for Enter */
        if (input == '\n') {
            /* Clear the scene */
            CLEAR_SCENE();
            return 0;
        };

        /* Handle navigation keys */
        if (input == 'j') {
            /* Check for the current index */
            if (current_index + 1 >= opts_num) continue;

            /* Do not point to the old option and go down */
            REDRAW_LINE();

            /* Increase the current index */
            ++current_index;

            /* Align the cursor */
            if (tuiy() >= bottom) tuiscup(1);
            else tuieddown(1);
        } else if (input == 'k') {
            /* Check for the current index */
            if (current_index <= 0) continue;

            /* Do not point to the old option and go up */
            REDRAW_LINE();

            /* Decrease the current index */
            --current_index;

            /* Align the cursor */
            if (tuiy() <= top) tuiscdown(1);
            else tuiedup(1);
        }

        /* Redraw the current line */
        REDRAW_CLINE();
    }

    seterr("Unknown error");
    return -1;
}

#endif
