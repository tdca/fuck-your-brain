
#include "stack.h"

void push(stack_t* stack,int data){
	stack->pos = (stack->pos+1)%T;
	stack->chunk[stack->pos] = data;
}

int pop(stack_t* stack){
	int data = stack->chunk[stack->pos];
	stack->pos = (stack->pos-1)%T;
	return data;
}

int top(stack_t* stack){
	return stack->chunk[stack->pos];
}

