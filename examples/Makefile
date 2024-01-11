.PHONY: all clean
MAKEFLAGS += --no-print-directory

SUB_DIRS = log util

all:
	@for dir in $(SUB_DIRS); do \
		$(MAKE) -C $$dir; \
	done

clean:
	@for dir in $(SUB_DIRS); do \
		$(MAKE) -C $$dir clean; \
	done
