#ifndef BST_H
#define BST_H
#include <iostream>
#include <cstdlib>
#include <queue>
#include <array>
#include <list>

typedef std::array<double, 3> vector3d;

typedef struct Node {
	vector3d index;
	struct Node* left;
	struct Node* right;

} Node;

int operator < (vector3d a, vector3d b);
int operator > (vector3d a, vector3d b);
int operator < (vector3d a, double b);
int operator > (vector3d a, double b);
int operator == (vector3d a, double b);
double operator - (vector3d a, double b);

void addChild(Node*& node, vector3d value);
vector3d searchValue(Node* node, double value);



#endif // !BST_H

