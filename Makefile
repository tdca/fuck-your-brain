
CC=gcc
OBJS=obj/bfi.o obj/stack.o 
TARGET=bfi
CFLAGS=-Iinclude -Wall
RM=rm -rf


.PHONY: clean env int

int: $(OBJS)
	$(CC) -o $(TARGET) $(OBJS)

$(OBJS): obj/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

env: 
	mkdir -p obj
	mkdir -p bin

clean:
	$(RM) $(OBJS)

