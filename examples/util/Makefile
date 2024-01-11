.PHONY: all clean
MAKEFLAGS += --no-print-directory

EXAMPLES = read_lines

CC = gcc
EXTRA_FLAGS =
CFLAGS = -Wall -Wextra -Werror -pedantic -std=c17 $(EXTRA_FLAGS)

all: $(EXAMPLES)

clean:
	rm -rf $(EXAMPLES)
