#ifndef CUTIE_UTIL_IO
#define CUTIE_UTIL_IO

#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <stdarg.h>
#include <termios.h>

#include "esc/text.h"

FILE* tty = NULL;
struct termios old_term;

/** Print the fatal message
 * @param format The format string for the printf
 */
static inline void fatalf(const char* const format, ...) {
    /* Make the error message bold red and print the prefix */
    fputs(T(BOLD, RED)"Error: ", stderr);

    /* Print the message */
    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);

    /* End by the new line char */
    fputc('\n', stderr);
}

/** Init the tty device
 * @returns 0 on success; 1 on error
 */
static inline int32_t ttyinit(void) {
    tty = fopen("/dev/tty", "r+");
    if (tty == NULL) return -1;
    else return 0;
}

/** Free the tty device file desciptor
 */
static inline void ttyfree(void) {
    if (tty != NULL) fclose(tty);
}

/** Print the data to the raw tty device
 * @param format The format string for the printf
 */
static inline void ttyprintf(const char* const format, ...) {
    va_list args;
    va_start(args, format);
    vfprintf(tty, format, args);
    va_end(args);
}

/** Flush the tty buffer
 */
static inline void ttyflush(void) {
    fflush(tty);
    fflush(stdout);
}

/** Get the current position of the curesor
 * @param x The pointer to store the x axis
 * @param y The pointer to store the y axis
 * @returns 0 on success; -1 on error
 */
static inline int32_t ttygetpos(int16_t* const x, int16_t* const y) {
    /* Send the ascii seq to get the pos */
    fprintf(tty, "\033[6n");
    ttyflush();

    /* Handle the ascii seq response */
    if (fscanf(tty, "\033[%hd;%hdR", y, x) != 2) {
        fatalf("Bad cursor position reply");
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
    if (ioctl(fileno(tty), TIOCGWINSZ, &ws) == -1) return -1;

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
    if (tcgetattr(fileno(tty), &old_term) == -1) {
        fatalf("Failed to get the terminal state");
        return -1;
    }

    /* Disable the canon mode and echo */
    struct termios new_term = old_term;
    new_term.c_lflag &= ~(ICANON | ECHO);

    /* Read char by char without any timeout */
    new_term.c_cc[VMIN] = 1;
    new_term.c_cc[VTIME] = 0;

    /* Enable the full buffering */
    setvbuf(tty, NULL, _IOFBF, BUFSIZ);
    setvbuf(stdout, NULL, _IOFBF, BUFSIZ);

    /* Apply the new terminal settings */
    if (tcsetattr(fileno(tty), TCSANOW, &new_term)) {
        fatalf("Failed to set the terminal state");
        return -1;
    }

    /* Return the success code */
    return 0;
}

/** Restore the terminal setting
 */
static inline void ttyreset(void) {
    tcsetattr(fileno(tty), TCSANOW, &old_term);
}

#endif
