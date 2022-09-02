#ifndef SHAPE_H
#define SHAPE_H

#define _USE_MATH_DEFINES

#include <cmath>
#include <cstdlib>
#include <queue>
#include <array>
#include <list>
#include <iostream>

#define FIRST_SEC 1
#define SECOND_SEC 2
#define THIRD_SEC 4
#define FOURTH_SEC 8
#define FIFTH_SEC 16
#define SIXTH_SEC 32
#define SEVENTH_SEC 64
#define EIGHTH_SEC 128
#define A_OCTANT 45

enum figure { LINE, RECTANGLE, FLOOD, TRIANGLE, CIRCLE };

typedef std::list<std::array<int, 2>> veci2;
typedef std::list<std::array<int, 3>> veci3;


typedef struct ShapeFOV {
	int xCent;
	int yCent;
	int radius;
	int xIni;
	int yIni;
	int xEnd;
	int yEnd;
	double aRight;
	double aLeft;
} ShapeFOV;

//void drawShape(int xIni, int yIni, int xEnd, int yEnd, int* arrayBuffer, int* changedArray, int mode, int save);

veci2 drawLineDDA(int xIni, int yIni, int xEnd, int yEnd, int* arrayBuffer, int* changedArray, int mode, int save, int height, int width, int* destBuffer = NULL);
veci2 drawLineBresenham(int xIni, int yIni, int xEnd, int yEnd, int* arrayBuffer, int* changedArray, int mode, int save, int height, int width, int* destBuffer = NULL, int draw = 0);
veci2 drawLineBresenham(ShapeFOV lineP, int* arrayBuffer, int* changedArray, int mode, int save, int height, int width, int* destBuffer = NULL, int draw = 0);
veci2 bresenhamAlgorithm(int xIni, int yIni, int xEnd, int yEnd, int* arrayBuffer, int* changedArray, int mode, int save, int height, int width, int reversed, int* destBuffer = NULL, int draw = 1);
veci2 bresenhamCircleAlgorithm(int xCent, int yCent, int xIni, int yIni, int xEnd, int yEnd, int radius, double aRight, double aLeft, int* arrayBuffer, int* changedArray, int mode, int save, int height, int width, int* destBuffer = NULL, int draw = 1);
veci2 bresenhamCircleAlgorithm(ShapeFOV param, int* arrayBuffer, int* changedArray, int mode, int save, int height, int width, int* destBuffer, int draw);
void castRayBresenham(int xIni, int yIni, int xEnd, int yEnd, int* arrayBuffer, int* changedArray, int mode, int save, int height, int width, int* destBuffer);
void bresenhamCastAlgorithm(int xIni, int yIni, int xEnd, int yEnd, int* arrayBuffer, int* changedArray, int mode, int boundValue, int clearValue, int save, int height, int width, int yReversed, int pointsReversed, int* destBuffer);

int sectorVal(int secRight, int secLeft, int octant, int xIni, int xEnd, int x, int loop);

void drawRect(int xIni, int yIni, int xEnd, int yEnd, int* arrayBuffer, int* changedArray, int mode, int save, int height, int width, int* destBuffer = NULL);

void calculateTriangle(int xIni, int yIni, int wd, double afov, double offset, int* arrayBuffer, int* changedArray, int mode, int save, int height, int width, 
	veci2(*func)(ShapeFOV, int*, int*, int, int, int, int, int*, int), int* destBuffer = NULL);

//void paintPixel(int index, int value);
void floodFill(int x, int y, int newValue, int oldValue, int* arrayBuffer, int* changedArray, int height, int width, int* destBuffer = NULL);

#endif // !SHAPE_CLASS_H

