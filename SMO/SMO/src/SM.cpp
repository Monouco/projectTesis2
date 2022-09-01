#include "SM.h"

SM::SM(int maxCams, int height, int width)
{
	numCams = 0;
	this->maxCams = maxCams;
	solution = new Camera[maxCams];
	fitness = 0;
	prob = 0;
	this->width = width;
	this->height = height;
	visibilityMatrix = new int[width * height];
}

SM::SM(const SM& sm, int mode)
{
	int i;
	maxCams = sm.maxCams;
	width = sm.width;
	height = sm.height;

	solution = new Camera[maxCams];
	visibilityMatrix = new int[width * height];

	if (mode) {
		prob = sm.prob;
		fitness = sm.fitness;
		numCams = sm.numCams;

		//Making a copy of the solution
		for (i = 0; i < maxCams; i++) {
			solution[i][0] = sm.solution[i][0];
			solution[i][1] = sm.solution[i][1];
			solution[i][2] = sm.solution[i][2];
			solution[i][3] = sm.solution[i][3];
			solution[i][4] = sm.solution[i][4];
			solution[i].setModel(solution[i].getModel());
		}

		//making a copy of the visibility matrix
		for (i = 0; i < width * height; i++) {
			visibilityMatrix[i] = sm.visibilityMatrix[i];
		}
	}
	else {
		prob = 0;
		fitness = 0;
		numCams = 0;
	}
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

double SM::getFitness()
{
	return fitness;
}

Camera* SM::getSolution() {
	return solution;
}
double SM::getProb() {
	return prob;
}
int* SM::getVisibilityMatrix() {
	return visibilityMatrix;
}
int SM::getNumCams() {
	return numCams;
}
int SM::getMaxCams() {
	return maxCams;
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

double SM::fitSM(int height, int width, int* priorityMatrix,
	double(*func)(Camera* solution, int maxCams, int height, int width, int* visibilityMatrix, int* priorityMatrix))
{
	fitness = func(solution, maxCams,  height,  width, visibilityMatrix, priorityMatrix);
	return fitness;
}

void SM::calProb(double maxFitness)
{
	prob = 0.9 * fitness / maxFitness + 0.1;
}

SM& SM::operator=(const SM& sm)
{
	int i;
	maxCams = sm.maxCams;
	prob = sm.prob;
	fitness = sm.fitness;
	numCams = sm.numCams;

	//Making a copy of the solution
	for (i = 0; i < maxCams; i++) {
		solution[i][0] = sm.solution[i][0];
		solution[i][1] = sm.solution[i][1];
		solution[i][2] = sm.solution[i][2];
		solution[i][3] = sm.solution[i][3];
		solution[i][4] = sm.solution[i][4];
		solution[i].setModel(solution[i].getModel());
	}
	
	//making a copy of the visibility matrix
	for (i = 0; i < width * height; i++) {
		visibilityMatrix[i] = sm.visibilityMatrix[i];
	}
}
