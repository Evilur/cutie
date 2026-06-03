#ifndef CUTIE_ESC_TEXT
#define CUTIE_ESC_TEXT

#include "basic.h"

/* Displatcher */
#define T(...)                                                                 \
    T_GET_MACRO(__VA_ARGS__,T9,T8,T7,T6,T5,T4,T3,T2,T1,T0)(__VA_ARGS__)
#define T_GET_MACRO(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, NAME, ...) NAME

/* Basic macro */
#define T0(a)                            CSI a"m"
#define T1(a, b)                         CSI a";"b"m"
#define T2(a, b, c)                      CSI a";"b";"c"m"
#define T3(a, b, c, d)                   CSI a";"b";"c";"d"m"
#define T4(a, b, c, d, e)                CSI a";"b";"c";"d";"e"m"
#define T5(a, b, c, d, e, f)             CSI a";"b";"c";"d";"e";"f"m"
#define T6(a, b, c, d, e, f, g)          CSI a";"b";"c";"d";"e";"f";"g"m"
#define T7(a, b, c, d, e, f, g, h)       CSI a";"b";"c";"d";"e";"f";"g";"h"m"

/* Font style attributes */
#define RESET     "0"
#define BOLD      "1"
#define DIM       "2"
#define ITALIC    "3"
#define UNDERLINE "4"
#define BLINK     "5"
#define REVERSE   "7"
#define HIDDEN    "8"
#define STRIKE    "9"

/* Foreground colors attributes */
#define BLACK   "30"
#define RED     "31"
#define GREEN   "32"
#define YELLOW  "33"
#define BLUE    "34"
#define MAGENTA "35"
#define CYAN    "36"
#define WHITE   "37"
#define DEFAULT "39"

/* Bright foreground colors attributes */
#define BRIGHT_BLACK   "90"
#define BRIGHT_RED     "91"
#define BRIGHT_GREEN   "92"
#define BRIGHT_YELLOW  "93"
#define BRIGHT_BLUE    "94"
#define BRIGHT_MAGENTA "95"
#define BRIGHT_CYAN    "96"
#define BRIGHT_WHITE   "97"

#endif
