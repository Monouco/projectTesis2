
#include "src/SMO.h"
#include <ctime>

#define ALPHA 2.5f
#define PRIORITY_THRESHOLD 0.4f
#define NON_PRIORITY_THRESHOLD 0.2f

//cost function
double objectiveFunction1(Camera* solution, int maxCams) {
	double cost = 0;
	int i;
	for (i = 0; i < maxCams; i++) {
		if(round(solution[i].getUsed()))
			cost += (solution[i].getModel()->cost * round(solution[i].getUsed()));
	}
	if (cost == 0)
		cost = INFINITY;
	return cost;
}

//camera coverage metric
double objectiveFunction2(int height, int width, int* visibilityMatrix, int* priorityMatrix) {
	int i, priorityCount = 0, nonPriorityCount = 0, fullPriority = 0, fullNonPriority = 0;
	double priorityC, nonPriorityC;
	for (i = 0; i < width * height; i++) {
		if (priorityMatrix[i] == 1) {
			fullNonPriority++;
		}
		if (priorityMatrix[i] == 2) {
			fullPriority++;
		}
		if (visibilityMatrix[i] >= priorityMatrix[i]) {
			if (priorityMatrix[i] == 1) {
				nonPriorityCount++;
			}
			if (priorityMatrix[i] == 2) {
				fullPriority++;
			}
		}
	}
	if (fullPriority * PRIORITY_THRESHOLD > priorityCount) {
		if (priorityCount != 0)
			priorityC = 1 - (double)1 / priorityCount;
		else
			priorityC = 0;
	}
	else {
		priorityC = priorityCount;
	}
	if (fullNonPriority * NON_PRIORITY_THRESHOLD > nonPriorityCount) { 
		if (nonPriorityCount != 0)
			nonPriorityC = 1 - (double)1 / nonPriorityCount;
		else
			nonPriorityC = 0;
	}
	else {
		nonPriorityC = nonPriorityCount;
	}

	return (double)priorityC * ALPHA + nonPriorityC;
}

//not covered metric
/*double objectiveFunction2(int height, int width, int* visibilityMatrix, int* priorityMatrix) {
	int i, priorityCount = 0, nonPriorityCount = 0, fullPriority = 0, fullNonPriority = 0, redundancyVal = 0;;
	double priorityC, nonPriorityC;
	for (i = 0; i < width * height; i++) {
		if (priorityMatrix[i] == 1) {
			fullNonPriority++;
		}
		if (priorityMatrix[i] == 2) {
			fullPriority++;
		}
		if (visibilityMatrix[i] >= priorityMatrix[i]) {
			if (priorityMatrix[i] == 1) {
				if (visibilityMatrix[i] > priorityMatrix[i])
					redundancyVal++;
				nonPriorityCount++;
			}
			if (priorityMatrix[i] == 2) {
				fullPriority++;
			}
		}
	}
	/*if (fullNonPriority == 0) {
		nonPriorityC = 1;
	}
	else {
		nonPriorityC = 1- (double)nonPriorityCount / fullNonPriority;
	}
	if (fullPriority == 0) {
		priorityC = 1;
	}
	else {
		priorityC = 1-(double)priorityCount / fullPriority;
	}*/
	/*nonPriorityC = nonPriorityCount;
	//return  1- nonPriorityC / fullNonPriority;
	return  nonPriorityC - (double)redundancyVal/2;

}*/

// trying to maximize the whole objective function
FitnessStruct objectiveFunction(Camera* solution, int maxCams, int height, int width, int* visibilityMatrix, int* priorityMatrix) {
	FitnessStruct fit;
	fit.FO1 = objectiveFunction1(solution, maxCams);
	fit.FO2 = objectiveFunction2(height, width, visibilityMatrix, priorityMatrix);
	//fit.mainFO =  fit.FO2 / fit.FO1;
	//fit.mainFO =  1/fit.FO2 ;
	fit.mainFO =  fit.FO2 ;
	return fit;
}



int main() {
	srand(80);
	int* env = NULL, *feasible = NULL, *priority = NULL, height, width, numModels = 0;
	Node* wIndex = NULL, * hIndex = NULL;
	CameraModel* models = NULL;

	int nIter = 50, popSize = 50, maxCam = 5, globalLeaderLimit = popSize / 2;
	int maxDim = maxCam * C_OFFSET;
	int localLeaderLimit = 20;
	//int localLeaderLimit = maxDim * popSize / 80;
	double pr = 0.9;

	double* minVal = new double[maxDim];
	double* maxVal = new double[maxDim];

	//reading camera models from file
	readModels("models.csv", models, numModels);
	//reading the enviroment from the file
	readEnv("level.pz", env, feasible, priority, height, width, wIndex, hIndex, minVal, maxVal, C_OFFSET, maxCam, numModels);


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

	//const double minVal[5] = { 0,0,0,0,0 };
	//const double maxVal[5] = { 1,numModels -1,width,height,360 };

	std::cout << "MinVal: " << minVal[0] << ", " << minVal[1] << ", " << minVal[2] << ", " << minVal[3] << ", " << minVal[4] << std::endl;
	std::cout << "MaxVal: " << maxVal[0] << ", " << maxVal[1] << ", " << maxVal[2] << ", " << maxVal[3] << ", " << maxVal[4] << std::endl;

	SMOAlgorithm(population, maxDim, minVal, maxVal, C_OFFSET, models, height, width, env, priority, nIter, wIndex, hIndex, objectiveFunction);

	std::cout << "Best Leader Fitness  : " << population.bestSolution->getFitness() << std::endl;
	std::cout << "Best Leader Cameras Count  : " << population.bestSolution->getNumCams() << std::endl;
	std::cout << "Best Leader FO1  : " << population.bestSolution->getFullFitness().FO1 << std::endl;
	std::cout << "Best Leader FO2  : " << population.bestSolution->getFullFitness().FO2 << std::endl;
	std::cout << "Best Leader FO   : " << population.bestSolution->getFullFitness().mainFO << std::endl;

	for (int i = 0; i < maxCam; i++) {
		std::cout << "Camera "<< i<<" : " << population.bestSolution->getSolution()[i].getUsed() << std::endl;
		std::cout << "Pos    "<< i<<" : " << population.bestSolution->getSolution()[i].getX() << ", " << population.bestSolution->getSolution()[i].getY() <<
			", " << population.bestSolution->getSolution()[i].getOffset()<< std::endl;
	}
	std::cout << std::endl;

	population.bestSolution->genVisibilityMatrix(env);

	population.bestSolution->exportSolution("bestSM.sm");
	

	return 0;
}