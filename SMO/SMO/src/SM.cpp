#include "SM.h"

SM::SM(int maxCams, int height, int width)
{
	numCams = 0;
	this->maxCams = maxCams;
	solution = new Camera[maxCams];
	fitness = 0;
	prob = 0;
	visibilityMatrix = new int[width * height];
}

SM::~SM()
{
}

void SM::setNumCams(int num) {
	numCams = num;
}
void SM::setModelCam(CameraModel* cam, int j) {
	int i = j / C_OFFSET;
	solution[i].setModel(cam);
}

double& SM::operator[](int j)
{
	int i = j / C_OFFSET;
	int k = j % C_OFFSET;

	return solution[i][k];
}

void SM::genVisibilityMatrix(int * env, int height, int width) {
	int i, j;
	//Cleaning the matrix from a previous iteration
	for (i = 0; i < width * height; i++)
		visibilityMatrix[i] = 0;

	for (i = 0; i < maxCams; i++) {
		solution[i].calcFOV( env, height, width, visibilityMatrix);
	}
}

void SM::fitSM(int height, int width, int* priorityMatrix,
	double(*func)(Camera* solution, int maxCams, int height, int width, int* visibilityMatrix, int* priorityMatrix))
{
	fitness = func(solution, maxCams,  height,  width, visibilityMatrix, priorityMatrix);
	return;
}
