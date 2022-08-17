#include "level/TextureStack.h"

StackT::StackT(int maxNodes, size_t sizeNodes) {
	this->maxNodes = maxNodes;
	this->sizeNode = sizeNodes;
	this->curNodes = 0;
	nodes = (void**) malloc(sizeof(void*)*maxNodes);
	top = -1;
	bottom = -1;
}

void StackT::push(void* node)
{
	void* tempNode;
	if (maxNodes == 0) return;
	//initialize
	if (bottom == top && top == -1) {
		bottom++;
	}
	//reset top position if the top of the array has been reached
	if (++top == maxNodes) top = 0;

	//if the stack is full, free the bottom and assign it as a free space for the next node
	if (curNodes == maxNodes) {
		free(nodes[bottom]);
		if (++bottom == maxNodes) bottom = 0;
		curNodes--;
	}

	nodes[top] = malloc(sizeNode);

	//save the node at new position
	memcpy(nodes[top], node, sizeNode);

	curNodes++;
}

void* StackT::pop()
{
	void* tempNode;
	//if empty return null
	if (maxNodes == 0 || curNodes == 0) return NULL;

	tempNode = nodes[top];

	//if bottom is reachead, reset position of top
	if (--top < 0) top = maxNodes - 1;

	curNodes--;

	return tempNode;
}

