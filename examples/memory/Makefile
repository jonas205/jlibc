.PHONY: all clean
MAKEFLAGS += --no-print-directory

EXAMPLES = corruption no_free bad_free
CC = gcc
EXTRA_FLAGS =
CFLAGS = -Wall -Wextra -Werror -pedantic -std=c17 $(EXTRA_FLAGS)

all: $(EXAMPLES)

memory.o: memory.c ../../jc_memory.h ../../jc_log.h memory.h

corruption: corruption.c memory.o

no_free: no_free.c memory.o

bad_free: bad_free.c memory.o

clean:
	rm -rf $(EXAMPLES) memory.o
