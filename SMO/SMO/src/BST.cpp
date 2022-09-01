#include "BST.h"

int operator < (vector3d a, vector3d b) {
	return a[0] < b[0];
}
int operator > (vector3d a, vector3d b) {
	return a[0] > b[0];
}
int operator < (vector3d a, double b) {
	return a[0] < b;
}
int operator > (vector3d a, double b) {
	return a[0] > b;
}
int operator == (vector3d a, double b) {
	return a[0] == b;
}
double operator - (vector3d a, double b) {
	return abs(a[0] - b);
}

void addChild(Node* node, vector3d value) {

	if (node == NULL) {
		node = new Node;
		node->index = value;
		node->left = NULL;
		node->right = NULL;
		return;
	}
	if (node->index < value) {
		addChild(node->right, value);
	}
	if (node->index > value) {
		addChild(node->left, value);
	}
	return;
}

vector3d searchValue(Node* node, double value) {
	if (node == NULL) {
		std::cout << "BST must be initialize" << std::endl;
		exit(1);
	}
	if (node->index == value) return node->index;
	if (node->index < value) {
		if (node->right == NULL) return node->index;
		if (node->index - value <= node->right->index - value) return node->index;
		else searchValue(node->right, value);
	}
	if (node->index > value) {
		if (node->left == NULL) return node->index;
		if (node->index - value <= node->left->index - value) return node->index;
		else searchValue(node->left, value);
	}
	std::cout << "Cannot find the elemnt on BST" << std::endl;
	exit(1);
}
