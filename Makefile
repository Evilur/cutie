# Compiler and its flags
CC 	   ?= gcc
CFLAGS ?= -Wall -Wextra -Wpedantic -Wshadow -Wformat=2 -Wuninitialized \
-Wconversion -Wlogical-op -Wnull-dereference -Wduplicated-cond \
-Wredundant-decls -Wstrict-prototypes -Wmissing-declarations \
-Wunreachable-code -Wmissing-prototypes -O2
CFLAGS += -std=c99

# Prefix
PREFIX	    ?= /usr/local
BINPREFIX   ?= $(PREFIX)/bin

# Set targets that do not create new files
.PHONY: clean install uninstall

# Build the C binary
cutie:
	mkdir -p build
	$(CC) $(CFLAGS) src/main.c -o build/cutie

# Clean all compiled C binaries
clean:
	rm -rf build
