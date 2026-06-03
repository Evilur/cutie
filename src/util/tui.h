#ifndef CUTIE_UTIL_TUI
#define CUTIE_UTIL_TUI

#include "util/io.h"

/* The current tty status */
int16_t _x = 1, _y = 1;
int16_t _width = 0, _height = 0;

/** Init the tui interface
 * @returns 0 on success; -1 on error
 */
static inline int32_t tuiinit(void) {
    /* Open the tty descriptor */
    if (ttyinit() == -1) return -1;

    /* Exit the canonical mode */
    if (ttycanonoff() == -1) return -1;

    /* Get the terminal size */
    if (ttygetsize(&_width, &_height) == -1) return -1;

    /* Update the cursor position */
    if (ttygetpos(&_x, &_y) == -1) return -1;;

    /* Return the success code */
    return 0;
}

/** Increment the y position
 */
static inline void _tuiincy(void) {
    if (_y < _height) ++_y;
}

/** Decrement the y position
 */
static inline void _tuidecy(void) {
    if (_y > 1) --_y;
}

/** Change the y position
 * @param n The number to change the x
 */
static inline void _tuioffy(const int16_t n) {
    _y += n;
    if (_y < 1) _y = 1;
    else while (_y > _height) _y -= _height;
}

/** Set the y position
 * @param y The new position
 */
static inline void _tuisety(const int16_t y) {
    _y = y;
    if (_y > _height) _y = _height;
    else if (_y < 1) _y = 1;
}

/** Increment the x position
 */
static inline void _tuiincx(void) {
    ++_x;
    if (_x > _width) {
        _tuiincy();
        _x = 1;
    }
}

/** Decrement the x position
 */
static inline void _tuidecx(void) {
    --_x;
    if (_x < 1) _x = 1;
}

/** Change the x position
 * @param n The number to change the x
 */
static inline void _tuioffx(const int16_t n) {
    _x += n;
    if (_x < 1) _x = 1;
    else while (_x > _width) { _tuiincy(); _x -= _width; }
}

/** Set the x position
 * @param x The new position
 */
static inline void _tuisetx(const int16_t x) {
    _x = x;
    if (_x > _width) _x = _width;
    else if (_x < 1) _x = 1;
}

/** Print the string and change the terminal states (x, y)
 * @param string The string to print
 */
static inline void _tuiprint(char* const string) {
    /* Replace the tabulation with space */
    for (char* string_ptr = string; *string_ptr != '\0'; ++string_ptr)
        if (*string_ptr == '\t') *string_ptr = ' ';

    /* Print the string */
    ttyprintf("%s", string);

    /* Evaluate the new cursor position */
    for (const char* string_ptr = string; *string_ptr != '\0'; ++string_ptr) {
        switch (*string_ptr) {
            /* New line char */
            case '\n':
                _tuiincy();
                _x = 1;
                break;

            /* Return carriet char */
            case '\r':
                _x = 1;
                break;

            /* Backspace char */
            case '\b':
                _tuidecx();
                break;

            /* An ASCII sequence */
            case '\033':
                ++string_ptr;
                switch(*string_ptr++) {
                    /* CSI sequences */
                    case '[':
                        while (*string_ptr != '\0' &&
                               !(*string_ptr >= '@' && *string_ptr <= '~'))
                               ++string_ptr;
                        break;

                    /* OSC sequences */
                    case ']':
                        while (*string_ptr != '\0' &&
                               !(*string_ptr >= '@' && *string_ptr <= '~'))
                               ++string_ptr;
                        break;
                }
                break;

            /* A regular char */
            default:
                _tuiincx();
                break;
        }
    }
}

/** Print the string using the printf
 * @param format The format string
 *               DO NOT push to the string any ASCII sequences which
 *               rewrites cursor position. Use special methods instead
 */
static inline void tuiprintf(const char* const format, ...) {
    /* Buffer to try to store the first buffer */
    static char buffer[4];

    /* Try to get the size of the formatted string */
    va_list args;
    va_start(args, format);
    const int32_t ssize = vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);

    /* If we doesn't obtain the full string */
    if (ssize >= sizeof(buffer)) {
        char nbuffer[ssize + 1];
        va_start(args, format);
        vsnprintf(nbuffer, sizeof(nbuffer), format, args);
        va_end(args);
        _tuiprint(nbuffer);
    }

    /* If we already have the full string */
    else {
        _tuiprint(buffer);
    }
}

/** Move the cursor up
 * @param n The number of lines to move the cursor
 */
static inline void tuimvup(const uint16_t n) {
    ttyprintf(CSI"%huA", n);
    _tuioffy(n);
}

/** Move the cursor down
 * @param n The number of lines to move the cursor
 */
static inline void tuimvdown(const uint16_t n) {
    ttyprintf(CSI"%huB", n);
    _tuioffy(-n);
}

/** Move the cursor right
 * @param n The number of columns to move the cursor
 */
static inline void tuimvright(const uint16_t n) {
    ttyprintf(CSI"%huC", n);
    _tuioffx(n);
}

/** Move the cursor left
 * @param n The number of columns to move the cursor
 */
static inline void tuimvleft(const uint16_t n) {
    ttyprintf(CSI"%huD", n);
    _tuioffx(-n);
}

/** Move the cursor down and to the first column
 * @param n The number of lines to move
 */
static inline void tuieddown(const uint16_t n) {
    ttyprintf(CSI"%huE", n);
    _tuioffy(n);
    _tuisetx(1);
}

/** Move the cursor up and to the first column
 * @param n The number of lines to move
 */
static inline void tuiedup(const uint16_t n) {
    ttyprintf(CSI"%huF", n);
    _tuioffy(-n);
    _tuisetx(1);
}

/** Move the cursor to the absolute position
 * @param x The x coord
 * @param y The y coord
 */
static inline void tuigoto(const uint16_t x, const uint16_t y) {
    ttyprintf(CSI"%hu;%huH", y, x);
    _tuisetx(x);
    _tuisety(y);
}

/** Set the scrollable terminal part
 * @param top The top part of the scrollable part
 * @param bottom The bottom part of the scrollable part
 */
static inline void tuisetsc(const uint16_t top, const uint16_t bottom) {
    ttyprintf(CSI"%hu;%hur", top, bottom);
    tuigoto(1, top);
}

/** Reset the scrollable terminal part
 */
static inline void tuiressc(void) {
    ttyputs(CSI"r");
}

/** Scroll the terminal up
 * @param n The number of lines to scroll
 */
static inline void tuiscup(const uint16_t n) {
    ttyprintf(CSI"%huS", n);
}

/** Scroll the terminal down
 * @param n The number of lines to scroll
 */
static inline void tuiscdown(const uint16_t n) {
    ttyprintf(CSI"%huT", n);
}

/** Hide the cursor
 */
static inline void tuihidec(void) {
    ttyputs(CSI"?25l");
}

/** Show the cursor
 */
static inline void tuishowc(void) {
    ttyputs(CSI"?25h");
}

/** Clear all lines below
 */
static inline void tuicldown(void) {
    ttyputs(CSI"0J");
}

/** Get the width of the terminal
 * @returns The width of the terminal
 */
static inline uint16_t tuiwidth(void) {
    return _width;
}

/** Get the height of the terminal
 * @returns The height of the terminal
 */
static inline uint16_t tuiheight(void) {
    return _height;
}

/** Get the x position of the cursor
 * @returns The x position
 */
static inline uint16_t tuix(void) {
    return _x;
}

/** Get the y position of the cursor
 * @returns The y position
 */
static inline uint16_t tuiy(void) {
    return _y;
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
