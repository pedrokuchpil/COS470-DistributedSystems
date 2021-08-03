TARGETS = pipes

CC_C = gcc

all: clean $(TARGETS)

$(TARGETS):
	$(CC_C) $@.c ../lib/library.o -std=c11 -o $@

clean:
	rm -f $(TARGETS)