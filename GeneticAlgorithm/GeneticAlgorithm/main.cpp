
#include "src/GeneticAlgorithm.h"
#include <ctime>

#define ALPHA 2.5f
#define PRIORITY_THRESHOLD 0.4f
#define NON_PRIORITY_THRESHOLD 0.2f
#define C_OFFSET 5

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
	int* env = NULL, *feasible = NULL, *priority = NULL, height, width, numModels = 0, numSectors=0;
	CameraModel* models = NULL;

	int nIter = 50, popSize = 50, maxCam = 5, numPos = 0,  maxMutatedGenes= 5;
	
	Position* positions;
	double* sectorsOffset;
	double crossRate = 0.6, mutRate = 0.2, bestRate = 0.8;


	//reading camera models from file
	readModels("models.csv", models, numModels, numSectors, sectorsOffset);
	//reading the enviroment from the file
	readEnv("level.pz", env, feasible, priority, height, width, C_OFFSET, maxCam, numModels, numPos, positions, numSectors, sectorsOffset);


	Pop population;
	population.popSize = popSize;
	population.maxPopSize = popSize;
	population.maxMutGenes = maxMutatedGenes;
	population.numPos = numPos;
	population.mutRate = mutRate;
	population.crossRate = crossRate;
	population.bestRate = bestRate;
	population.population = new Chromosome * [popSize];
	population.width = width;
	population.height = height;
	population.bestSolution = new Chromosome(positions, numPos, height, width, maxCam);

	GeneticAlgorithm(population, models, numModels, height, width, env, priority, nIter, maxCam, positions, numPos, objectiveFunction);


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

	population.bestSolution->exportSolution("bestCh.sm");
	

	return 0;
}