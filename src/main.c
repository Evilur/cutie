#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <sys/ioctl.h>

#include "type/bool.h"
#include "esc/text.h"

int32_t main(const int argc, const char* const* argv) {
    printf(T(BOLD, ITALIC, RED)"TEST");
    return 0;
}
