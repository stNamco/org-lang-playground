CFLAGS=-std=c11 -g -fno-common
SRCS=$(wildcard *.c)
OBJS=$(SRCS:.c=.o)

calc: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

$(OBJS): calc.h


test: calc
	./test.sh

clean:
	rm -f calc *.o *~ tmp*

.PHONY: test clean