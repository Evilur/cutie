#ifndef CUTIE_UTIL_TERM
#define CUTIE_UTIL_TERM

#include "util/io.h"

/* The current tty status */
int16_t _x = 1, _y = 1;
int16_t _width = 0, _height = 0;

/** Init the t interface
 * @returns 0 on success; -1 on error
 */
static inline int32_t tinit(void);

/** Increment the y position
 */
static inline void _tincy(void);

/** Decrement the y position
 */
static inline void _tdecy(void);

/** Change the y position
 * @param n The number to change the x
 */
static inline void _toffy(int16_t n);

/** Set the y position
 * @param y The new position
 */
static inline void _tsety(int16_t y);

/** Increment the x position
 */
static inline void _tincx(void);

/** Decrement the x position
 */
static inline void _tdecx(void);

/** Change the x position
 * @param n The number to change the x
 */
static inline void _toffx(int16_t n);

/** Set the x position
 * @param x The new position
 */
static inline void _tsetx(int16_t x);

/** Print the string and change the terminal states (x, y)
 * @param string The string to print
 */
static inline void _tprint(char* string);

/** Print the string using the printf
 * @param format The format string
 *               DO NOT push to the string any ASCII sequences which
 *               rewrites cursor position. Use special methods instead
 */
static inline void tprintf(const char* format, ...);

/** Move the cursor down
 * @param n The number of lines to move the cursor
 */
static inline void tgodown(uint16_t n);

/** Move the cursor up
 * @param n The number of lines to move the cursor
 */
static inline void tgoup(uint16_t n);

/** Move the cursor right
 * @param n The number of columns to move the cursor
 */
static inline void tmvright(uint16_t n);

/** Move the cursor down and to the first column
 * @param n The number of lines to move
 */
static inline void teddown(uint16_t n);

/** Move the cursor up and to the first column
 * @param n The number of lines to move
 */
static inline void tedup(uint16_t n);

/** Move the cursor to the absolute position
 * @param x The x coord
 * @param y The y coord
 */
static inline void tgoto(uint16_t x, uint16_t y);

/** Set the scrollable terminal part
 * @param top The top part of the scrollable part
 * @param bottom The bottom part of the scrollable part
 */
static inline void tsetsc(uint16_t top, uint16_t bottom);

/** Reset the scrollable terminal part
 */
static inline void tressc(void);

/** Scroll the terminal up
 * @param n The number of lines to scroll
 */
static inline void tscup(uint16_t n);

/** Scroll the terminal down
 * @param n The number of lines to scroll
 */
static inline void tscdown(uint16_t n);

/** Clear all lines below
 */
static inline void tcldown(void);

/** Get the width of the terminal
 * @returns The width of the terminal
 */
static inline uint16_t twidth(void);

/** Get the height of the terminal
 * @returns The height of the terminal
 */
static inline uint16_t theight(void);

/** Get the x position of the cursor
 * @returns The x position
 */
static inline uint16_t tx(void);

/** Get the y position of the cursor
 * @returns The y position
 */
static inline uint16_t ty(void);

/** Flush the output
 */
static inline void tflush(void);

/** Free the t interface from the memory
 */
static inline void tfree(void);

static inline int32_t tinit(void) {
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

static inline void _tincy(void) {
    if (_y < _height) ++_y;
}

static inline void _tdecy(void) {
    if (_y > 1) --_y;
}

static inline void _toffy(const int16_t n) {
    _y += n;
    if (_y < 1) _y = 1;
    else while (_y > _height) _y -= _height;
}

static inline void _tsety(const int16_t y) {
    _y = y;
    if (_y > _height) _y = _height;
    else if (_y < 1) _y = 1;
}

static inline void _tincx(void) {
    ++_x;
    if (_x > _width) {
        _tincy();
        _x = 1;
    }
}

static inline void _tdecx(void) {
    --_x;
    if (_x < 1) _x = 1;
}

static inline void _toffx(const int16_t n) {
    _x += n;
    if (_x < 1) _x = 1;
    else while (_x > _width) { _tincy(); _x -= _width; }
}

static inline void _tsetx(const int16_t x) {
    _x = x;
    if (_x > _width) _x = _width;
    else if (_x < 1) _x = 1;
}

static inline void _tprint(char* const string) {
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
                _tincy();
                _x = 1;
                break;

            /* Return carriet char */
            case '\r':
                _x = 1;
                break;

            /* Backspace char */
            case '\b':
                _tdecx();
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
                _tincx();
                break;
        }
    }
}

static inline void tprintf(const char* const format, ...) {
    /* Buffer to try to store the formated string */
    static char buffer[128];

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
        _tprint(nbuffer);
    }

    /* If we already have the full string */
    else {
        _tprint(buffer);
    }
}

static inline void tgodown(const uint16_t n) {
    ttyprintf(CSI"%huB", n);
    _toffy(n);
}

static inline void tgoup(const uint16_t n) {
    ttyprintf(CSI"%huD", n);
    _toffx(n);
}

static inline void tmvright(const uint16_t n) {
    ttyprintf(CSI"%huC", n);
    _toffx(-n);
}

static inline void teddown(const uint16_t n) {
    ttyprintf(CSI"%huE", n);
    _tsetx(1);
    _toffy(n);
}

static inline void tedup(const uint16_t n) {
    ttyprintf(CSI"%huF", n);
    _tsetx(1);
    _toffy(-n);
}

static inline void tgoto(const uint16_t x, const uint16_t y) {
    ttyprintf(CSI"%hu;%huH", y, x);
    _tsetx(x);
    _tsety(y);
}

static inline void tsetsc(const uint16_t top, const uint16_t bottom) {
    ttyprintf(CSI"%hu;%hur", top, bottom);
    tgoto(1, top);
}

static inline void tressc(void) {
    ttyputs(CSI"r");
}

static inline void tscup(const uint16_t n) {
    ttyprintf(CSI"%huS", n);
}

static inline void tscdown(const uint16_t n) {
    ttyprintf(CSI"%huT", n);
}

static inline void tcldown(void) {
    ttyputs(CSI"0J");
}

static inline uint16_t twidth(void) {
    return _width;
}

static inline uint16_t theight(void) {
    return _height;
}

static inline uint16_t tx(void) {
    return _x;
}

static inline uint16_t ty(void) {
    return _y;
}

static inline void tflush(void) {
    fflush(stdout);
}

static inline void tfree(void) {
    /* Restore the old terminal mode */
    ttyreset();

    /* Enable the cursor */
    ttyputs(CSI"?25h");

    /* Close the tty file desciptor */
    ttyfree();
}

#endif
