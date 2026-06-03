#ifndef CUTIE_UTIL_TUI
#define CUTIE_UTIL_TUI

#include "util/io.h"

/* The current tty status */
int16_t x = 1, y = 1;
int16_t width = 0, height = 0;

/** Init the tui interface
 * @returns 0 on success; -1 on error
 */
static inline int32_t tuiinit(void) {
    /* Open the tty descriptor */
    if (ttyinit() == -1) return -1;

    /* Exit the canonical mode */
    if (ttycanonoff() == -1) return -1;

    /* Get the terminal size */
    if (ttygetsize(&width, &height) == -1) return -1;

    /* Update the cursor position */
    if (ttygetpos(&x, &y) == -1) return -1;;

    /* Return the success code */
    return 0;
}

/** Increment the y position
 */
static inline void tuiincy(void) {
    if (y < height) ++y;
}

/** Decrement the y position
 */
static inline void tuidecy(void) {
    if (y > 1) --y;
}

/** Change the y position
 * @param n The number to change the x
 */
static inline void tuioffy(const int16_t n) {
    y += n;
    if (y < 1) y = 1;
    else while (y > height) y -= height;
}

/** Increment the x position
 */
static inline void tuiincx(void) {
    ++x;
    if (x >= width) {
        tuiincy();
        x = 1;
    }
}

/** Decrement the x position
 */
static inline void tuidecx(void) {
    --x;
    if (x < 1) x = 1;
}

/** Change the x position
 * @param n The number to change the x
 */
static inline void tuioffx(const int16_t n) {
    x += n;
    if (x < 1) x = 1;
    else while (x >= width) { tuiincy(); x -= width; }
}

/** Print the string using the printf
 * @param format The format string
 *               DO NOT push to the string '\t' chars or any
 *               ASCII sequences which rewrites cursor position
 *               Use special methods instead
 */
static inline void tuiprintf(const char* const format, ...) {
    /* Buffer to try to store the first buffer */
    static char buffer[128];

    /* Try to get the size of the formatted string */
    va_list args;
    va_start(args, format);
    const int32_t ssize = vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);

/* The macro to print and handle the string */
#define TUI_PRINTF_PRINT(string) {                                             \
    ttyprintf("%s", string);                                                   \
    for (const char* string_ptr = string; *string_ptr != '\0'; ++string_ptr) { \
        switch (*string_ptr) {                                                 \
            /* New line char */                                                \
            case '\n':                                                         \
                tuiincy();                                                     \
                x = 1;                                                         \
                break;                                                         \
                                                                               \
            /* Return carriet char */                                          \
            case '\r':                                                         \
                x = 1;                                                         \
                break;                                                         \
                                                                               \
            /* Backspace char */                                               \
            case '\b':                                                         \
                tuidecx();                                                     \
                break;                                                         \
                                                                               \
            /* An ASCII sequence */                                            \
            case '\033':                                                       \
                ++string_ptr;                                                  \
                while (*string_ptr != '\0' &&                                  \
                       !(*string_ptr >= '@' && *string_ptr <= '~'))            \
                       ++string_ptr;                                           \
                break;                                                         \
                                                                               \
            /* A regular char */                                               \
            default:                                                           \
                tuiincx();                                                     \
                break;                                                         \
        }                                                                      \
    }                                                                          \
}

    /* If we doesn't obtain the full string */
    if (ssize >= sizeof(buffer)) {
        char nbuffer[ssize + 1];
        va_start(args, format);
        vsnprintf(nbuffer, sizeof(nbuffer), format, args);
        va_end(args);
        TUI_PRINTF_PRINT(nbuffer);
    }

    /* If we already have the full string */
    else {
        TUI_PRINTF_PRINT(buffer);
    }
}

/** Flush the output
 */
static inline void tuiflush(void) {
    fflush(stdout);
}

/** Free the tui interface from the memory
 */
static inline void tuifree(void) {
    /* Restore the old terminal mode */
    ttyreset();

    /* Close the tty file desciptor */
    ttyfree();
}

#endif
