#include "Chromosome.h"

Chromosome::Chromosome(Position* positions, int numPos, int height, int width, int maxCams)
{
	numCams=0;
	solution = new Camera[maxCams];
	fitness = 0;
	this->maxCams = maxCams;
	this->width = width;
	this->height = height;
	visibilityMatrix = new int[width * height];
	fullFitness.mainFO = 0;
	fullFitness.FO1 = 0;
	fullFitness.FO2 = 0;
	this->positions = new Position[numPos];
	this->numPos = numPos;
	int i;
	for (i = 0; i < numPos; i++) {
		this->positions[i].x = positions[i].x;
		this->positions[i].y = positions[i].y;
		this->positions[i].index = positions[i].index;
		this->positions[i].offset = positions[i].offset;
		this->positions[i].cam = NULL;
	}
}

Chromosome::Chromosome(const Chromosome& base)
{
	int i, j;
	maxCams = base.maxCams;
	fitness = base.fitness;
	numCams = base.numCams;
	numPos = base.numPos;
	height = base.height;
	width = base.width;
	CameraModel* temp;

	solution = new Camera[maxCams];
	positions = new Position[numPos];

	//Making a copy of the solution and positions
	for (i = 0, j = 0; i < maxCams && j < numPos; j++) {
		positions[j].x = base.positions[j].x;
		positions[j].y = base.positions[j].y;
		positions[j].index = base.positions[j].index;
		positions[j].offset = base.positions[j].offset;
		positions[j].cam = NULL;
		if (base.positions[j].cam != NULL) {
			solution[i][0] = (*base.positions[j].cam)[0];
			solution[i][1] = (*base.positions[j].cam)[1];
			solution[i][2] = (*base.positions[j].cam)[2];
			solution[i][3] = (*base.positions[j].cam)[3];
			solution[i][4] = (*base.positions[j].cam)[4];
			solution[i][5] = (*base.positions[j].cam)[5];
			solution[i][6] = (*base.positions[j].cam)[6];
			temp = base.solution[i].getModel();
			solution[i].setModel(temp);
			if (solution[i].getModel() == NULL) {
				std::cout << "Error setting camera model" << std::endl;
				exit(1);
			}
			positions[j].cam = &solution[i];
			i++;
		}

	}

	if (j < numPos) {
		std::cout << "Error at coping solution, the number of cameras exceed the maximum amount allowed" << std::endl;
		exit(1);
	}

	//making a copy of the visibility matrix
	for (i = 0; i < width * height; i++) {
		visibilityMatrix[i] = base.visibilityMatrix[i];
	}

	fullFitness.mainFO = base.fullFitness.mainFO;
	fullFitness.FO1 = base.fullFitness.FO1;
	fullFitness.FO2 = base.fullFitness.FO2;
}

Chromosome::~Chromosome()
{
	delete visibilityMatrix;
	delete positions;
	delete[] solution;
}

double Chromosome::getFitness()
{
	//return fitness;
	return fullFitness.mainFO;
}

FitnessStruct Chromosome::getFullFitness()
{
	return fullFitness;
}

Camera* Chromosome::getSolution() {
	return solution;
}
int* Chromosome::getVisibilityMatrix() {
	return visibilityMatrix;
}
int Chromosome::getNumCams() {
	return numCams;
}
int Chromosome::getMaxCams() {
	return maxCams;
}

int Chromosome::getHeight()
{
	return height;
}

int Chromosome::getWidth()
{
	return width;
}

int Chromosome::getNumPos()
{
	return numPos;
}

Position& Chromosome::operator[](int j)
{
	if (j >= numPos) {
		std::cout << "Position out of bounds" << std::endl;
		exit(1);
	}
	return positions[j];
}

Chromosome& Chromosome::operator=(const Chromosome& base)
{
	int i,j;
	maxCams = base.maxCams;
	fitness = base.fitness;
	numCams = base.numCams;
	numPos = base.numPos;
	CameraModel* temp;

	//Making a copy of the solution and positions
	for (i = 0, j=0; i < maxCams && j<numPos; j++) {
		positions[j].x = base.positions[j].x;
		positions[j].y = base.positions[j].y;
		positions[j].index = base.positions[j].index;
		positions[j].offset = base.positions[j].offset;
		positions[j].cam = NULL;
		if(base.positions[j].cam != NULL){
			solution[i][0] = (*base.positions[j].cam)[0];
			solution[i][1] = (*base.positions[j].cam)[1];
			solution[i][2] = (*base.positions[j].cam)[2];
			solution[i][3] = (*base.positions[j].cam)[3];
			solution[i][4] = (*base.positions[j].cam)[4];
			solution[i][5] = (*base.positions[j].cam)[5];
			solution[i][6] = (*base.positions[j].cam)[6];
			temp = base.solution[i].getModel();
			solution[i].setModel(temp);
			if (solution[i].getModel() == NULL) {
				std::cout << "Error setting camera model" << std::endl;
				exit(1);
			}
			positions[j].cam = &solution[i];
			i++;
		}

	}

	if (j < numPos) {
		std::cout << "Error at coping solution, the number of cameras exceed the maximum amount allowed" << std::endl;
		exit(1);
	}

	//making a copy of the visibility matrix
	for (i = 0; i < width * height; i++) {
		visibilityMatrix[i] = base.visibilityMatrix[i];
	}

	fullFitness.mainFO = base.fullFitness.mainFO;
	fullFitness.FO1 = base.fullFitness.FO1;
	fullFitness.FO2 = base.fullFitness.FO2;

	return *this;
}

int Chromosome::operator<(const Chromosome& b)
{
	//this is so the list sorts ascending
	return this->fullFitness.mainFO > b.fullFitness.mainFO;
}

void Chromosome::setPos(Position pos, int j)
{
	if (j >= numPos) {
		std::cout << "Position out of bounds" << std::endl;
		exit(1);
	}
	//since positions are static, the only thing that changes is the camera pointer of the position
	Camera* newCam = pos.cam;
	Camera* curCam = this->positions[j].cam;
	int i;
	if (curCam != NULL) {
		//removing the camera from the solution
		if (newCam == NULL) {
			curCam->setUsed(0);
			this->positions[j].cam = NULL;
			numCams--;
		}
		else {
			//assigning new cam
			curCam->setModel(newCam->getModel());
			(*curCam)[0] = 1;
			(*curCam)[1] = (*newCam)[1];
			(*curCam)[2] = pos.x;
			(*curCam)[3] = pos.y;
			(*curCam)[4] = pos.offset;
			(*curCam)[5] = pos.x;
			(*curCam)[6] = pos.y;
		}
	}
	else {
		if (newCam != NULL) {
			for (i = 0; i < maxCams; i++) {
				//empty space
				if (solution[i][0] == 0) {
					solution[i].setModel(newCam->getModel());
					solution[i][0] = 1;
					solution[i][1] = (*newCam)[1];
					solution[i][2] = pos.x;
					solution[i][3] = pos.y;
					solution[i][4] = pos.offset;
					solution[i][5] = pos.x;
					solution[i][6] = pos.y;
					this->positions[j].cam = &solution[i];
					numCams++;
					break;
				}
			}
			//no empty space
			if (i == maxCams) {
				curCam->setModel(newCam->getModel());
				curCam = new Camera();
				(*curCam)[0] = 1;
				(*curCam)[1] = (*newCam)[1];
				(*curCam)[2] = pos.x;
				(*curCam)[3] = pos.y;
				(*curCam)[4] = pos.offset;
				(*curCam)[5] = pos.x;
				(*curCam)[6] = pos.y;
				this->positions[j].cam = curCam;
				cameraArray.push_back(*curCam);
				numCams++;
			}
		}
	}
}

void Chromosome::deleteCam(int pos)
{
	for (int i = 0, j=0; i < numCams; i++) {

		if (i >= maxCams) {
			CameraArray::iterator it = cameraArray.begin();
			std::advance(it, i - maxCams);
			j += (*it).getUsed();
			if (j == pos) (*it).setUsed(0);
		}
		else {
			j += solution[i].getUsed();
			if (j == pos) solution[i].setUsed(0);
		}

		if (j == pos) {
			numCams--;
			return;
		}
	}
}

void Chromosome::correctSolution() {
	int i = 0, j;
	CameraArray::iterator it;
	for (i = 0; i < numPos; i++) {
		if (positions[i].cam != NULL) {
			if (positions[i].cam->getUsed() == 0)
				positions[i].cam = NULL;
		}
	}

	for (it = cameraArray.begin(); it != cameraArray.end(); ++it) {
		if ((*it).getUsed() == 0)
			cameraArray.erase(it);
	}

	//one of the main solution cameras has been cleared
	if (cameraArray.size() != 0) {
		for (i = 0; i < numPos && cameraArray.size() > 0; i++) {
			if (positions[i].cam != NULL) {
				for (it = cameraArray.begin(); it != cameraArray.end(); ++it) {
					//is the same memory space
					if (positions[i].cam == &(*it)) {
						//find an empty space on chromosome solution
						for (j = 0; j < maxCams; j++) {
							if (solution[j].getUsed() == 0) {
								solution[j].setModel((*it).getModel());
								solution[j][0] = 1;
								solution[j][1] = (*it)[1];
								solution[j][2] = positions[i].x;
								solution[j][3] = positions[i].y;
								solution[j][4] = positions[i].offset;
								solution[j][5] = positions[i].x;
								solution[j][6] = positions[i].y;
								positions[i].cam = &solution[j];

								cameraArray.erase(it);
								break;
							}
						}
					}
				}
			}
		}
	}

}

void Chromosome::genVisibilityMatrix(int* env) {
	int i, j;
	//Cleaning the matrix from a previous iteration
	for (i = 0; i < width * height; i++)
		visibilityMatrix[i] = 0;

	for (i = 0; i < maxCams; i++) {
		if ((int)round(solution[i].getUsed()))
			solution[i].calcFOV(env, height, width, visibilityMatrix);
	}
}

double Chromosome::fit(int* priorityMatrix,
	FitnessStruct(*func)(Camera* solution, int maxCams, int height, int width, int* visibilityMatrix, int* priorityMatrix))
{
	fullFitness = func(solution, maxCams, height, width, visibilityMatrix, priorityMatrix);
	fitness = fullFitness.mainFO;
	return fitness;
}