
#include "src/SMO.h"
#include <ctime>

int main() {
	int* env = NULL, *feasible = NULL, height, width, numModels = 0;
	Node* wIndex = NULL, * hIndex = NULL;
	CameraModel* models = NULL;

	//reading the enviroment from the file
	readEnv("level.pz", env, feasible, height, width, wIndex, hIndex);
	//reading camera models from file

	int nIter = 100, popSize = 50, maxCam = 10, globalLeaderLimit = popSize/2;
	int maxDim = maxCam * C_OFFSET;
	int localLeaderLimit = maxDim * popSize;
	double pr = 0.7;

	Pop population;
	population.popSize = popSize;
	population.MG = popSize / 10;
	population.globalLeaderLimit = globalLeaderLimit;
	population.localLeaderLimit = localLeaderLimit;
	population.globalLimitCount = 0;
	population.pr = pr;

	const double minVal[5] = { 0,0,0,0,0 };
	const double maxVal[5] = { 1,numModels,width,height,360 };

	std::cout << "MinVal: " << minVal[0] << ", " << minVal[1] << ", " << minVal[2] << ", " << minVal[3] << ", " << minVal[4] << std::endl;
	std::cout << "MaxVal: " << maxVal[0] << ", " << maxVal[1] << ", " << maxVal[2] << ", " << maxVal[3] << ", " << maxVal[4] << std::endl;

	initPopulation(population, maxDim, minVal, maxVal, C_OFFSET, models);
	1 + 1;
	SM* a;
	for (int i = population.popSize -1; i > 0; i--) {
		a = population.population[i];
		(*a)[8];
	}
	return 0;
}