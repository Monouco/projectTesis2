#include "SMO.h"

double rand01(double min, double max) {
	return min + (double)rand() / RAND_MAX * (max - min);
}

double randInt(double min, double max) {
	return min + rand() % (int)max;
}

void readEnv(const char* fileName, int* env, int* feasible, int& height, int& width, Node * wIndex, Node* hIndex) {
	std::ifstream openedFile(fileName, std::ios::out | std::ios::binary);
	if (!openedFile) {
		std::cout << "Cannot access file " << fileName << std::endl;
		return;
	}
	int* fTemp;
	double i, j;
	std::vector<vector3d> wList, hList;
	vector3d wVect, hVect;

	//saving height and width
	openedFile.read((char*)&height, sizeof(int));
	openedFile.read((char*)&width, sizeof(int));
	env = (int*)malloc(sizeof(int) * height * width);
	fTemp = (int*)malloc(sizeof(int) * height * width);
	//saving the array buffer that contains the info of the level
	openedFile.read((char*)env, sizeof(int) * (std::streamsize) height * (std::streamsize) width);
	//creating the feasible points array
	openedFile.read((char*)fTemp, sizeof(int) * (std::streamsize) height * (std::streamsize) width);
	
	for (i = 0; i < height; i = i + 1) {
		for (j = 0; j < width; j = j + 1) {
			wVect[0] = ((double)width - 1 - j) * height + i;
			wVect[1] = j;
			wVect[2] = i;			

			hVect[0] = ((double)height - 1 - i) * width + j;
			hVect[1] = j;
			hVect[2] = i;

			wList.push_back(wVect);
			hList.push_back(hVect);
		}
	}

	//to avoid creating a list with a BST
	std::random_shuffle(wList.begin(), wList.end());
	std::random_shuffle(hList.begin(), hList.end());

	for (std::vector<vector3d>::iterator it = wList.begin(); it != wList.end(); ++it) {
		addChild(wIndex, *it);
	}
	for (std::vector<vector3d>::iterator it = hList.begin(); it != hList.end(); ++it) {
		addChild(hIndex, *it);
	}

	openedFile.close();

	if (!openedFile.good()) {
		std::cout << "Error at reading file " << fileName << std::endl;
		return;
	}
}

void readModels(const char* fileName, CameraModel* models, int& numModels)
{
	//Creating one model for now
	models = new CameraModel[1];
	CameraModel newModel;
	
	numModels = 1;
	newModel.cost = 1;
	newModel.focalLength = 10;
	newModel.range = 20;
	newModel.resolution = 720;
	newModel.sensor = 10;
	newModel.afov = 2 * atan(newModel.sensor / (2 * newModel.focalLength)) * 180 / M_PI;
}

void initPopulation(Pop & pop, int maxDim,const double* minVal, const double* maxVal, int offset, CameraModel* models) {
	int i, j, maxCam = maxDim / offset, numCams;
	SM* newSM;
	pop.population = new SM* [pop.popSize];
	for (i = 0; i < pop.popSize; i++) {
		newSM = new SM(maxCam);
		(pop.population)[i] = newSM;
		numCams = 0;
		for (j = 0; j < maxDim; j++) {
			//noted that maxDim is a multiple of offset
			(*newSM)[j] = (minVal)[j % offset] + rand01(0, 1) * (maxVal[j % offset] - minVal[j % offset]);
			switch (j % offset)
			{
			case 0:
				numCams+= round((*newSM)[j]);
				break;
			case 1:
				newSM->setModelCam(&models[(int)round((*newSM)[j])], j);
				break;
			default:
				break;
			} 
		}
		newSM->setNumCams(numCams);
	}
}