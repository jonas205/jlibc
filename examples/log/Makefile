.PHONY: all clean
MAKEFLAGS += --no-print-directory

EXAMPLES = colored_io logger

CC = gcc
EXTRA_FLAGS =
CFLAGS = -Wall -Wextra -Werror -pedantic -std=c17 $(EXTRA_FLAGS)

all: colored_io logger

clean:
	rm -rf $(EXAMPLES)
