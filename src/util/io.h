#ifndef CUTIE_UTIL_IO
#define CUTIE_UTIL_IO

#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <stdarg.h>
#include <termios.h>

#include "esc/text.h"

FILE* _tty = NULL;
struct termios _old_term;
char* _ebuf = NULL;

/** Save the error to the error buffer to print it later
 * @param format The format string for the printf
 */
static inline void seterr(const char* const format, ...) {
    /* Get the message len */
    va_list args;
    va_start(args, format);
    const int32_t len = vsnprintf(NULL, 0, format, args);
    va_end(args);

    /* Create a buffer for that error message */
    if (_ebuf != NULL) free(_ebuf);
    _ebuf = malloc(len + 1);

    /* Print the error message to the buffer */
    va_start(args, format);
    vsnprintf(_ebuf, len + 1, format, args);
    va_end(args);
}

/* Print the error from the buffer
 */
static inline void printerr(void) {
    if (_ebuf == NULL) seterr("Unknown error");

    /* Print the error message */
    fprintf(stderr, T(BOLD, RED)"Error: %s\n", _ebuf);
}

/** Init the tty device
 * @returns 0 on success; 1 on error
 */
static inline int32_t ttyinit(void) {
    _tty = fopen("/dev/tty", "r+");
    if (_tty == NULL) {
        seterr("Failed to open the /dev/tty device");
        return -1;
    }
    else return 0;
}

/** Free the tty device file desciptor
 */
static inline void ttyfree(void) {
    if (_tty != NULL) fclose(_tty);
}

/** Print the data to the raw tty device using printf
 * @param format The format string for the printf
 */
static inline void ttyprintf(const char* const format, ...) {
    va_list args;
    va_start(args, format);
    vfprintf(_tty, format, args);
    va_end(args);
}

/** Print the data to the raw tty device
 * @param string The seting to print
 */
static inline void ttyputs(const char* const string) {
    fputs(string, _tty);
}

/** Read one char from the tty
 * @return The char from the /dev/tty
 */
static inline char ttygetchar() {
    return fgetc(_tty);
}

/** Flush the tty buffer
 */
static inline void ttyflush(void) {
    fflush(_tty);
    fflush(stdout);
}

/** Get the current position of the curesor
 * @param x The pointer to store the x axis
 * @param y The pointer to store the y axis
 * @returns 0 on success; -1 on error
 */
static inline int32_t ttygetpos(int16_t* const x, int16_t* const y) {
    /* Send the ascii seq to get the pos */
    fprintf(_tty, "\033[6n");
    ttyflush();

    /* Handle the ascii seq response */
    if (fscanf(_tty, "\033[%hd;%hdR", y, x) != 2) {
        seterr("Bad cursor position reply");
        return -1;
    } else return 0;
}

/** Get the terminal size
 * @param width The pointer to the width
 * @param height The pointer to the height
 */
static inline int32_t ttygetsize(int16_t* const width, int16_t* const height) {
    /* Get the terminal size */
    struct winsize ws;
    if (ioctl(fileno(_tty), TIOCGWINSZ, &ws) == -1) {
        seterr("Failed to get the tty size");
        return -1;
    }

    /* Set the terminal size */
    *width = ws.ws_col;
    *height = ws.ws_row;

    /* Return the success code */
    return 0;
}

/** Disable the canonical tty mode
 * @returns 0 on success; -1 on error
 */
static inline int32_t ttycanonoff(void) {
    /* Save the current terminal settings */
    if (tcgetattr(fileno(_tty), &_old_term) == -1) {
        seterr("Failed to get the terminal state");
        return -1;
    }

    /* Disable the canon mode and echo and signal chars */
    struct termios new_term = _old_term;
    new_term.c_lflag &= ~(ICANON | ECHO | ISIG);

    /* Read char by char without any timeout */
    new_term.c_cc[VMIN] = 1;
    new_term.c_cc[VTIME] = 0;

    /* Enable the full buffering */
    setvbuf(_tty, NULL, _IOFBF, BUFSIZ);
    setvbuf(stdout, NULL, _IOFBF, BUFSIZ);

    /* Apply the new terminal settings */
    if (tcsetattr(fileno(_tty), TCSANOW, &new_term)) {
        seterr("Failed to set the terminal state");
        return -1;
    }

    /* Return the success code */
    return 0;
}

/** Restore the terminal setting
 */
static inline void ttyreset(void) {
    tcsetattr(fileno(_tty), TCSANOW, &_old_term);
}

#endif
