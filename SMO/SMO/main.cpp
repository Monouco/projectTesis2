
#include "src/SMO.h"
#include <ctime>

#define ALPHA 2.5f
#define PRIORITY_THRESHOLD 0.8f
#define NON_PRIORITY_THRESHOLD 0.6f

//cost function
double objectiveFunction1(Camera* solution, int maxCams) {
	double cost = 0;
	int i;
	for (i = 0; i < maxCams; i++) {
		cost += (solution[i].getModel()->cost * solution[i].getUsed());
	}
	return cost;
}

//camera coverage metric
double objectiveFunction2(int height, int width, int* visibilityMatrix, int* priorityMatrix) {
	int i, priorityCount = 0, nonPriorityCount = 0, fullPriority = 0, fullNonPriority = 0;
	for (i = 0; i < width * height; i++) {

		if (visibilityMatrix[i] >= priorityMatrix[i]) {
			if (priorityMatrix[i] == 1) {
				fullNonPriority++;
				nonPriorityCount++;
			}
			if (priorityMatrix[i] == 2) {
				priorityCount++;
				fullPriority++;
			}
		}
	}
	if (fullPriority * PRIORITY_THRESHOLD > priorityCount) priorityCount = priorityCount*-1;
	if (fullNonPriority * NON_PRIORITY_THRESHOLD > nonPriorityCount) nonPriorityCount = 0;

	return (double)priorityCount * ALPHA + nonPriorityCount;
}

// trying to maximize the whole objective function
double objectiveFunction(Camera* solution, int maxCams, int height, int width, int* visibilityMatrix, int* priorityMatrix) {
	return(objectiveFunction2(height, width, visibilityMatrix, priorityMatrix) 
		/ objectiveFunction1(solution, maxCams));
}



int main() {
	int* env = NULL, *feasible = NULL, *priority = NULL, height, width, numModels = 0;
	Node* wIndex = NULL, * hIndex = NULL;
	CameraModel* models = NULL;

	//reading the enviroment from the file
	readEnv("level.pz", env, feasible, priority, height, width, wIndex, hIndex);
	//reading camera models from file
	readModels("models.csv", models, numModels);

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
	population.numGroups = 1;
	population.pr = pr;
	population.groups = new Group[population.MG];
	population.width = width;
	population.height = height;

	const double minVal[5] = { 0,0,0,0,0 };
	const double maxVal[5] = { 1,numModels -1,width,height,360 };

	std::cout << "MinVal: " << minVal[0] << ", " << minVal[1] << ", " << minVal[2] << ", " << minVal[3] << ", " << minVal[4] << std::endl;
	std::cout << "MaxVal: " << maxVal[0] << ", " << maxVal[1] << ", " << maxVal[2] << ", " << maxVal[3] << ", " << maxVal[4] << std::endl;

	SMOAlgorithm(population, maxDim, minVal, maxVal, C_OFFSET, models, height, width, env, priority, nIter, wIndex, hIndex, objectiveFunction);

	population.bestSolution->exportSolution("bestSM.sm");

	return 0;
}