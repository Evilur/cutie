#ifndef CUTIE_UTIL_IO
#define CUTIE_UTIL_IO

#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include <stdarg.h>
#include <termios.h>

#include "esc/text.h"

static int32_t tty = -1;
static struct termios old_term;

/** Print the fatal message
 * @param format The format string for the printf
 */
static inline void fatalf(const char* const format, ...) {
    /* Make the error message bold red and print the prefix */
    fprintf(stderr, T(BOLD, RED)"Error: ");

    /* Print the message */
    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);

    /* End the new line char */
    fprintf(stderr, "\n");
}

/** Init the tty device
 * @returns 0 on success; 1 on error
 */
static inline int32_t ttyinit(void) {
    tty = open("/dev/tty", O_RDWR);
    if (tty == -1) return -1;
    else return 0;
}

/** Free the tty device file desciptor
 */
static inline void ttyfree(void) {
    close(tty);
}

/** Print the data to the raw tty device
 * @param format The format string for the printf
 */
static inline void ttyprintf(const char* const format, ...) {
    va_list args;
    va_start(args, format);
    vdprintf(tty, format, args);
    va_end(args);
}

/** Disable the cannonical tty mode
 */
static inline void ttycannonoff(void) {
    /* Save the current terminal settings */
    tcgetattr(tty, &old_term);

    /* Disable the cannon mode and echo */
    struct termios new_term = old_term;
    new_term.c_lflag &= ~(ICANON | ECHO);

    /* Read char by char without any timeout */
    new_term.c_cc[VMIN] = 1;
    new_term.c_cc[VTIME] = 0;

    /* Apply the new terminal settings */
    tcsetattr(tty, TCSANOW, &new_term);
}

/** Restore the terminal setting
 */
static inline void ttyreset(void) {
    tcsetattr(tty, TCSANOW, &old_term);
}

#endif
