#define _POSIX_C_SOURCE 200809L
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <sys/ioctl.h>

#include "type/bool.h"
#include "esc/font.h"

/* All available scene types */
enum Scene {
    CHECKLIST
};

/* Init the tty descriptor, the terminal size
 * and the current cursor position */
int32_t tty;
uint16_t x, y;
uint16_t width, height;

int32_t main(const int argc, const char* const* argv) {
    printf(ESC BLUE M "PENIS");
    return 0;
}
