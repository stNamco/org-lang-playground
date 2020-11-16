CFLAGS=-std=c11 -g -static
SRCS=$(wildcard *.c)
OBJS=$(SRCS:.c=.o)

calc: $(OBJS)
	$(CC) -o calc $(OBJS) $(LDFLAGS)

$(OBJS): calc.h

test: calc
	./test.sh

clean:
	rm -f calc *.o *~ tmp*

.PHONY: test clean