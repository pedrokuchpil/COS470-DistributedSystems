TARGETS = sendsignal signalhandler

CC_C = gcc

all: clean $(TARGETS)

$(TARGETS):
	$(CC_C) $@.c ../lib/library.o -o $@

clean:
	rm -f $(TARGETS)