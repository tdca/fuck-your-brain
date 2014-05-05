
#ifndef __STACK_H
#define __STACK_H


#define T 65536

typedef struct stack_t {
    unsigned int pos;
    int chunk[T];
} stack_t;

void push(stack_t* stack, int data);
int pop(stack_t* stack);
int top(stack_t* stack);

#endif  // __STACK_H
