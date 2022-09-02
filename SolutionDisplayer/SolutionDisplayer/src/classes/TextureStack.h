#ifndef STACK_T_CLASS_H
#define STACK_T_CLASS_H

#include <iostream>

class StackT
{
public:

	StackT(int maxNodes, size_t sizeNodes);
	void push(void * node);
	void* pop();
	~StackT();

private:
	int maxNodes;
	int curNodes;
	size_t sizeNode;
	void** nodes;
	int top;
	int bottom;
};


#endif // !STACK_T_CLASS_H
