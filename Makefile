
CC=clang++
OBJS=obj/bfi.o obj/stack.o 
TARGET=bfi
CFLAGS=-Iinclude -Wall -g -pg -g2 -std=c++1z
RM=rm -rf


.PHONY: clean env int

int: $(OBJS)
	$(CC) -o $(TARGET) $(OBJS)

$(OBJS): obj/%.o: src/%.cpp
	$(CC) $(CFLAGS) -c $< -o $@

env: 
	mkdir -p obj
	mkdir -p bin

clean:
	$(RM) $(OBJS)

