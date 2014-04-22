#ifndef _STACK_H
#define _STACK_H

#include <stdlib.h>
#include <mem.h>

typedef union{
	int i;
	float f;
	void* p;
} _StackItem;

typedef struct{
	int _head;
	_StackItem* _data;
} _Stack;

typedef _Stack *Stack;

Stack newStack(int size){
	Stack stack = malloc(sizeof(_Stack));
	stack->_head = 0;
	stack->_data = malloc(sizeof(_StackItem)*size);
	return stack;
}

void stackPush1i(Stack stack, int value){
	stack->_data[++stack->_head].i = value;
}
void stackPush1f(Stack stack, float value){
	stack->_data[++stack->_head].f = value;
}
void stackPush1p(Stack stack, void* value){
	stack->_data[++stack->_head].p = value;
}

int stackPop1i(Stack stack){
	return stack->_data[stack->_head--].i;
}
float stackPop1f(Stack stack){
	return stack->_data[stack->_head--].f;
}
void* stackPop1p(Stack stack){
	return stack->_data[stack->_head--].p;
}

int stackHead1i(Stack stack){
	return stack->_data[stack->_head].i;
}
float stackHead1f(Stack stack){
	return stack->_data[stack->_head].f;
}
void* stackHead1p(Stack stack){
	return stack->_data[stack->_head].p;
}

#endif