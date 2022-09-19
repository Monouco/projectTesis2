#include "GeneticAlgorithm.h"

double rand01(double min, double max) {
	return min + (double)rand() / RAND_MAX * (max - min);
}

double randInt(double min, double max, double i) {
	if (max - min == 0) return 0;
	double randNum = min + rand() % (int)(max - min);
	if (i >= 0) {
		if (randNum == i) {
			if (i == min)
				randNum += 1;
			else
				randNum -= 1;
		}
	}
	return randNum;
}

void readEnv(const char* fileName, int*& env, int*& feasible, int*& priority, int& height, int& width, int offset, int maxCam, int numModels,
	int& numPos, Position*& basePositions, int numSectors, double* sectorsOffset) {

	std::ifstream openedFile(fileName, std::ios::out | std::ios::binary);
	if (!openedFile) {
		std::cout << "Cannot access file " << fileName << std::endl;
		return;
	}
	int* fTemp;
	double x, y, xIni = INFINITY, yIni = INFINITY, yEnd = -1, xEnd = -1;
	int k, i;
	std::vector<Position> tempPos;
	Position p;
	//saving height and width
	openedFile.read((char*)&height, sizeof(int));
	openedFile.read((char*)&width, sizeof(int));
	env = (int*)malloc(sizeof(int) * height * width);
	priority = (int*)malloc(sizeof(int) * height * width);
	fTemp = (int*)malloc(sizeof(int) * height * width);
	//saving the array buffer that contains the info of the level
	openedFile.read((char*)env, sizeof(int) * (std::streamsize) height * (std::streamsize) width);
	//creating the feasible points array
	openedFile.read((char*)fTemp, sizeof(int) * (std::streamsize) height * (std::streamsize) width);

	for (y = 0, k = height * width - 1; y < height; y = y + 1) {
		for (x = width - 1; x >= 0; x = x - 1) {
			//Feasible points
			if (env[k] == 1 || env[k] == 2) {

				for (i = 0; i < numSectors; i++) {
					p.x = x;
					p.y = y;
					p.index = (height - 1 - y) * width + x;
					p.offset = sectorsOffset[i];
					p.cam = NULL;
					tempPos.push_back(p);
					numPos++;
				}

			}
			//setting the priority matrix
			switch (env[k])
			{
			case 2:
				priority[k] = 2;
				break;
			case 3:
				priority[k] = 2;
				break;
			case 4:
				priority[k] = 1;
				break;
			default:
				priority[k] = 0;
				break;
			}
			k--;
		}
	}
	
	basePositions = new Position[numPos];
	int i = 0;
	for (std::vector<Position>::iterator it = tempPos.begin(); it != tempPos.end(); ++it) {
		basePositions[i].x = (*it).x;
		basePositions[i].y = (*it).y;
		basePositions[i].index = (*it).index;
		basePositions[i].offset = (*it).offset;
		basePositions[i].cam = NULL;
	}

	openedFile.close();

	if (!openedFile.good()) {
		std::cout << "Error at reading file " << fileName << std::endl;
		exit(1);
	}

}

void readModels(const char* fileName, CameraModel*& models, int& numModels, int& numSectors, double*& sectorsOffset)
{
	//Creating one model for now
	models = new CameraModel[1];
	CameraModel newModel;
	double smallestAfov = INFINITY;

	numModels = 1;
	models[0].cost = 20;
	models[0].focalLength = 10;
	models[0].range = 150;
	models[0].resolution = 720;
	models[0].sensor = 12;
	models[0].afov = 2 * atan(models[0].sensor / (2 * models[0].focalLength)) * 180 / M_PI;

	smallestAfov = models[0].afov;
	numSectors = ceil(360 / smallestAfov);
	sectorsOffset = new double[numSectors];

	int i;
	for (i = 0; i < numSectors; i++) {
		sectorsOffset[i] = i * smallestAfov;
	}

	//TODO: iterate through the different models to find the smallest afov
}

//Genetic Algorithm

int roulette(Chromosome** population, int popSize, double totFit, int prev)
{
	int i = 0;
	double sumFit = 0, pickFit = rand01(0,totFit);
	for (i = 0; i < popSize; i++) {
		sumFit = sumFit + population[i]->getFitness();
		if (i != prev) {
			if (sumFit >= pickFit) {
				return i;
			}
		}
	}
	//if it doesnt pick someone, it means that the last individual
	//is the first parent and got picked again.
	//for this case i equals the value of popSize, so it is not a valid
	//position.
	return i-2;
}

 sortedPop::iterator roulette(sortedPop population, double totFit)
{
	int i = 0;
	double sumFit = 0, pickFit = rand01(0,totFit);
	sortedPop::iterator it;

	for (it = population.begin(); it != population.end(); ++it) {
		sumFit = sumFit + (*it).getFitness();
		if (sumFit >= pickFit) {
			return it;
		}
		
	}
	return it;
}

void initPopulation(Pop& pop, CameraModel* models, int numModels, int height, int width, int maxCams, int numPos, Position* positions) {
	int i = 0, numCams, pos, j, model;
	Chromosome* individual;
	Camera* tempCamera = new Camera();
	for (i = 0; i < pop.popSize; i++) {
		individual = new Chromosome(positions,numPos, height, width, maxCams);
		numCams = randInt(1, maxCams);
		for (j = 0; j < numCams; j++) {
			pos = randInt(0, numPos);
			model = randInt(0, numModels);
			(*tempCamera)[1] = model;
			tempCamera->setModel(& models[model]);
			positions[pos].cam = tempCamera;

			individual->setPos(positions[pos], pos);

			positions[pos].cam = NULL;
		}

		pop.population[i] = individual;
	}
}

parents selectParents(Pop pop) {
	int i, iFirst, iSecond;
	double totFitness = 0;
	for (i = 0; i < pop.popSize; i++) {
		totFitness = totFitness + pop.population[i]->getFitness();
	}
	iFirst = roulette(pop.population, pop.popSize, totFitness);
	iSecond = roulette(pop.population, pop.popSize, totFitness, iFirst);
	parents p;
	p[0] = pop.population[iFirst];
	p[1] = pop.population[iSecond];
	return p;
}

offspring crossover(Chromosome* firstParent, Chromosome* secondParent, int numPos, Position * basePositions) {
	int i, posCut = randInt(0, numPos);
	offspring childs;
	Chromosome* child1 = new Chromosome(basePositions, numPos, firstParent->getHeight(), firstParent->getWidth(), firstParent->getMaxCams());
	Chromosome* child2 = new Chromosome(basePositions, numPos, firstParent->getHeight(), firstParent->getWidth(), firstParent->getMaxCams());
	for (i = 0; i < numPos; i++) {
		if (i <= posCut) {
			child1->setPos((*firstParent)[i], i);
			child2->setPos((*secondParent)[i], i);
		}
		else {
			child1->setPos((*secondParent)[i], i);
			child2->setPos((*firstParent)[i], i);
		}
	}
	childs[0] = child1;
	childs[1] = child2;
	return childs;
}

void mutation(Chromosome* individual, CameraModel* models, int numModels, int  maxMutGenes, int numPos) {
	int i, mutatedGen = randInt(1, maxMutGenes), pos;
	int model;
	Camera* tempCam;
	Position tempPos;
	for (i = 0; i < mutatedGen; i++) {
		pos = randInt(0, numPos);
		tempCam = new Camera();
		tempPos.x = (*individual)[pos].x;
		tempPos.y = (*individual)[pos].y;
		tempPos.offset = (*individual)[pos].offset;
		tempPos.index = (*individual)[pos].index;
		tempPos.cam = (*individual)[pos].cam;
		if (tempPos.cam == NULL) {
			model = randInt(0, numModels);
			tempCam->setModel(&models[model]);
			(*tempCam)[1] = model;
		}
		else {
			model = randInt(0, numModels+1,(int)(*tempPos.cam)[1]);
			if (model == numModels)
				tempCam = NULL;
			else {
				model = randInt(0, numModels);
				tempCam->setModel(&models[model]);
				(*tempCam)[1] = model;
			}
		}
		tempPos.cam = tempCam;
		individual->setPos(tempPos, pos);
	}
}

void selectSurvivors(Pop& pop, int newPopSize) {
	int i, bestVal = pop.bestRate*pop.popSize;
	sortedPop totPop;
	sortedPop::iterator it;
	double totFit=0;

	Chromosome* tempChromosome;
	for (i = 0; i < pop.popSize + newPopSize; i++) {
		if(i < pop.popSize)
			tempChromosome = new Chromosome(*pop.population[i]);
		else
			tempChromosome = new Chromosome(*pop.newPopulation[i-pop.popSize]);
		totPop.push_back(*tempChromosome);
	}

	totPop.sort();

	for (it = totPop.begin(), i = 0; i < bestVal; i++, ++it) {
		pop.population[i] = new Chromosome((*it));
		totPop.erase(it);
	}

	for (sortedPop::iterator it2 = totPop.begin(); it2 != totPop.end(); ++it2) {
		totFit = totFit + (*it2).getFitness();
	}

	for (; i < pop.maxPopSize; i++) {
		it = roulette(totPop, totFit);
		pop.population[i] = new Chromosome((*it));
		//to avoid repeating elements
		totFit -= (*it).getFitness();
		totPop.erase(it);
	}
}

void correction(Chromosome* individual, CameraModel* models, int numModels, int maxCam){
		
	if (individual->getNumCams() > maxCam || individual->getNumCams() == 0) {
		int i, j, numToUse, pos, model, nCams, toDelete;
		Camera* tempCam;
		Position tempPos;
		if (individual->getNumCams() == 0) {
			numToUse = randInt(1, maxCam);
			for (i = 0; i < numToUse; i++) {
				pos = randInt(0, individual->getNumPos());
				model = randInt(0, numModels);
				tempCam = new Camera();
				tempPos.x = (*individual)[pos].x;
				tempPos.y = (*individual)[pos].y;
				tempPos.offset = (*individual)[pos].offset;
				tempPos.index = (*individual)[pos].index;
				tempPos.cam = tempCam;
				tempCam->setModel(&models[model]);
				(*tempCam)[1] = model;
				individual->setPos(tempPos, pos);
			}
		}
		else {
			for (nCams = individual->getNumCams(); nCams > maxCam; nCams--) {
				toDelete = randInt(0, individual->getNumCams());
				individual->deleteCam(toDelete);
			}

			individual->correctSolution();
		}
	}
}

void GeneticAlgorithm(Pop& pop, CameraModel* models, int numModels, int height, int width, int* env, int* priority, int nIter,
	 int maxCams, Position* positions, int numPos
	, FitnessStruct(*objectiveFunction)(Camera* solution, int maxCams, int height, int width, int* visibilityMatrix, int* priorityMatrix)){

	int i, j, numPairs = pop.popSize* pop.crossRate;
	int newPopSize = numPairs * 2;
	double mutProb;
	matingPool pairs;
	parents pair;
	offspring children;

	//first generation
	initPopulation(pop, models, numModels, height, width, maxCams, numPos, positions);
	for (j = 0; j < pop.popSize; j++) {
		pop.population[j]->genVisibilityMatrix(env);
		pop.population[j]->fit(priority, objectiveFunction);
	}

	for (i = 0; i < nIter; i++) {

		//selecting parents
		for (j = 0; j < numPairs; j++) {
			pairs.push_back(selectParents(pop));
		}

		//crossover
		for (j = 0; j < newPopSize; j+=2) {
			pair = pairs.back();
			pairs.pop_back();

			children = crossover(pair[0], pair[1], numPos, positions);

			pop.newPopulation[j] = children[0];
			pop.newPopulation[j+1] = children[1];
		}

		//mutation
		for (j = 0; j < newPopSize; j++) {
			mutProb = rand01(0, 1);
			if (mutProb <= pop.mutRate) {
				mutation(pop.newPopulation[j], models, numModels, pop.maxMutGenes, numPos);
			}
			correction(pop.newPopulation[j], models, numModels, maxCams);
		}

		//calculating fitness of new population
		for (j = 0; j < newPopSize; j++) {
			pop.newPopulation[j]->genVisibilityMatrix(env);
			pop.newPopulation[j]->fit(priority, objectiveFunction);
		}

		//selecting new population
		selectSurvivors(pop, newPopSize);
	}

}