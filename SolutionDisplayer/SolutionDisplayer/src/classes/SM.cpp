#include "SM.h"

SM::SM() {
	this->visibilityMatrix = NULL;
	this->solution = NULL;
}

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
			solution[i].setModel(sm.solution[i].getModel());
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

int SM::getHeight()
{
	return height;
}

int SM::getWidth()
{
	return width;
}

double& SM::operator[](int j)
{
	int i = j / C_OFFSET;
	int k = j % C_OFFSET;

	return solution[i][k];
}

void SM::genVisibilityMatrix(int * env) {
	int i, j;
	//Cleaning the matrix from a previous iteration
	for (i = 0; i < width * height; i++)
		visibilityMatrix[i] = 0;

	for (i = 0; i < maxCams; i++) {
		if((int)round(solution[i].getUsed()) == 1)
			solution[i].calcFOV( env, height, width, visibilityMatrix);
	}
}

double SM::fitSM( int* priorityMatrix,
	double(*func)(Camera* solution, int maxCams, int height, int width, int* visibilityMatrix, int* priorityMatrix))
{
	fitness = func(solution, maxCams,  height,  width, visibilityMatrix, priorityMatrix);
	return fitness;
}

void SM::calProb(double maxFitness)
{
	prob = 0.9 * fitness / maxFitness + 0.1;
}

void SM::exportSolution(const char* fileName) {
	int i;
	std::ofstream saveFile(fileName, std::ios::out | std::ios::binary);
	if (!saveFile) {
		std::cout << "Cannot access file " << fileName << std::endl;
		return;
	}
	//saving height and width
	saveFile.write((char*)&height, sizeof(int));
	saveFile.write((char*)&width, sizeof(int));
	//saving other properties
	saveFile.write((char*)&maxCams, sizeof(int));
	saveFile.write((char*)&numCams, sizeof(int));
	saveFile.write((char*)&fitness, sizeof(double));
	saveFile.write((char*)&prob, sizeof(double));

	//saving the solution
	for (i = 0; i < maxCams; i++) {
		saveFile.write((char *) &solution[i][0], sizeof(double));
		saveFile.write((char *) &solution[i][1], sizeof(double));
		saveFile.write((char *) &solution[i][2], sizeof(double));
		saveFile.write((char *) &solution[i][3], sizeof(double));
		saveFile.write((char *) &solution[i][4], sizeof(double));
		saveFile.write((char *) solution[i].getModel(), sizeof(CameraModel));
	}

	//saving the visibility matrix that contains the coverage of the solution
	saveFile.write((char*)visibilityMatrix, sizeof(int) * (std::streamsize) height * (std::streamsize) width);

	saveFile.close();

	if (!saveFile.good()) {
		std::cout << "Error at saving file " << fileName << std::endl;
	}
}

void SM::loadSolution(const char* fileName)
{
	int i;
	std::ifstream openedFile(fileName, std::ios::out | std::ios::binary);
	if (!openedFile) {
		std::cout << "Cannot access file " << fileName << std::endl;
		return;
	}
	//saving height and width
	openedFile.read((char*)&height, sizeof(int));
	openedFile.read((char*)&width, sizeof(int));
	//reading other properties
	openedFile.read((char*)&maxCams, sizeof(int));
	openedFile.read((char*)&numCams, sizeof(int));
	openedFile.read((char*)&fitness, sizeof(double));
	openedFile.read((char*)&prob, sizeof(double));

	//reading the solution
	solution = new Camera[maxCams];

	for (i = 0; i < maxCams; i++) {
		solution[i].setModel(new CameraModel);
		openedFile.read((char*)&solution[i][0], sizeof(double));
		openedFile.read((char*)&solution[i][1], sizeof(double));
		openedFile.read((char*)&solution[i][2], sizeof(double));
		openedFile.read((char*)&solution[i][3], sizeof(double));
		openedFile.read((char*)&solution[i][4], sizeof(double));
		openedFile.read((char*)solution[i].getModel(), sizeof(CameraModel));
	}
	visibilityMatrix = new int[width * height];
	//reading the visibility array that contains the info of the solution
	openedFile.read((char*)visibilityMatrix, sizeof(int) * (std::streamsize) height * (std::streamsize) width);

	openedFile.close();

	if (!openedFile.good()) {
		std::cout << "Error at reading file " << fileName << std::endl;
		return;
	}
}

SM& SM::operator=(const SM& sm)
{
	int i;
	maxCams = sm.maxCams;
	prob = sm.prob;
	fitness = sm.fitness;
	numCams = sm.numCams;
	CameraModel* temp;

	//Making a copy of the solution
	for (i = 0; i < maxCams; i++) {
		solution[i][0] = sm.solution[i][0];
		solution[i][1] = sm.solution[i][1];
		solution[i][2] = sm.solution[i][2];
		solution[i][3] = sm.solution[i][3];
		solution[i][4] = sm.solution[i][4];
		temp = sm.solution[i].getModel();
		solution[i].setModel(temp);
		if (solution[i].getModel() == NULL) {
			std::cout << "Error setting camera model" << std::endl;
		}
	}
	
	//making a copy of the visibility matrix
	for (i = 0; i < width * height; i++) {
		visibilityMatrix[i] = sm.visibilityMatrix[i];
	}

	return *this;
}
