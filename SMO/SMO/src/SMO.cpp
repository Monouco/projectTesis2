#include "SMO.h"

double rand01(double min, double max) {
	return min + (double)rand() / RAND_MAX * (max - min);
}

double randInt(double min, double max, double i) {
	if (max - min == 0) return 0;
	double randNum = min + rand() % (int)(max-min);
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

void dimCorrection(int j, int offset, int& numCams, CameraModel * models, SM* newSM) {
	switch (j % offset)
	{
	case 0:
		numCams += round((*newSM)[j]);
		break;
	case 1:
		newSM->setModelCam(&models[(int)round((*newSM)[j])], j);
		break;
	default:
		break;
	}
}

void initPopulation(Pop & pop, int maxDim,const double* minVal, const double* maxVal, int offset, CameraModel* models, int height, int width) {
	int i, j, maxCam = maxDim / offset, numCams;
	SM* newSM;
	pop.population = new SM* [pop.popSize];

	//population
	for (i = 0; i < pop.popSize; i++) {
		newSM = new SM(maxCam, height, width);
		(pop.population)[i] = newSM;
		numCams = 0;
		for (j = 0; j < maxDim; j++) {
			//noted that maxDim is a multiple of offset
			(*newSM)[j] = (minVal)[j % offset] + rand01(0, 1) * (maxVal[j % offset] - minVal[j % offset]);
			dimCorrection(j, offset, numCams, models, newSM);
		}
		newSM->setNumCams(numCams);
	}
}

void splitGroups(Pop& pop) {
	int i, popSize, j, k, gs, gsk, error, numGroups;
	popSize = pop.popSize;
	numGroups = pop.numGroups;
	gs = popSize / numGroups;
	error = popSize % numGroups;

	if (gs * numGroups + error != popSize) {
		std::cout << "Error with spliting groups" << std::endl;
		exit(1);
	}
	
	for (k = 0, i=0; k < numGroups; k++) {
		gsk = gs;
		if (error > 0) {
			gsk += 1;
			error--;
		}
		pop.groups[k].groupSize = gsk;
		pop.groups[k].groupMembers = new SM* [gsk];
		pop.groups[k].localLeader = NULL;
		pop.groups[k].localLimitCount = 0;
		pop.groups[k].maxFitness = -1;
		for (j = 0; j < gs; j++) {
			pop.groups[k].groupMembers[j] = pop.population[i];
			i++;
		}

	}
}

//Leader Phase
void LocalLeaderPhase(Pop& pop, int maxDim, int offset, CameraModel* models) {
	int k, mg = pop.numGroups, gs, i, j, r, numCams;
	SM** members, * newSM, *sm, *localLeader;
	newSM = new SM(*pop.globalLeader, 0);
	for (k = 0; k < mg; k++) {
		gs = pop.groups[k].groupSize;
		members = pop.groups[k].groupMembers;
		localLeader = pop.groups[k].localLeader;
		for (i = 0; i < gs; i++) {
			sm = members[i];
			numCams = 0;
			for (j = 0; j < maxDim; j++) {
				if (rand01(0, 1) >= pop.pr) {
					r = randInt(0, gs, i);
					(*newSM)[j] = (*sm)[j] + rand01(0, 1) * ((*localLeader)[j] - (*sm)[j]) + rand01(-1, 1) * ((*members[r])[j] - (*sm)[j]);
				}
				else {
					(*newSM)[j] = (*sm)[j];
				}
				//correcting dimensions
				dimCorrection(j, offset, numCams, models, newSM);
			}
			newSM->setNumCams(numCams);
			//calculating the fitness value of the new made position
			//newSM->fitSM();
			if (newSM->getFitness() > sm->getFitness()) {
				*sm = *newSM;
			}
		}
	}
}

void GlobalLeaderPhase(Pop& pop, int maxDim) {
	int k, mg = pop.numGroups, gs, i, j, r, count;
	SM** members, * newSM, * sm, * globalLeader = pop.globalLeader;
	newSM = new SM(*globalLeader, 0);
	for (k = 0; k < mg; k++) {
		gs = pop.groups[k].groupSize;
		members = pop.groups[k].groupMembers;
		for (count = 0; count < gs; ) {
			for (i = 0; i < gs; i++) {
				sm = members[i];

				if (rand01(0, 1) < sm->getProb()) {
					count++;

					//copying values
					*newSM = *sm;

					//calculating new position
					r = randInt(0, gs, i);
					j = randInt(0, maxDim);
					(*newSM)[j] = (*sm)[j] + rand01(0, 1) * ((*globalLeader)[j] - (*sm)[j]) + rand01(-1, 1) * ((*members[r])[j] - (*sm)[j]);

					//a dimension correction is missing
				}
				
				//calculating the fitness value of the new made position
				//newSM->fitSM();
				if (newSM->getFitness() > sm->getFitness()) {
					*sm = *newSM;
				}
			}
		}
	}
}

//Selection of new Leaders
void LocalLeaderLearningPhase(Pop & pop, int selection) {
	int mg = pop.MG, k, gs, i, bestMember;
	SM** members;
	double bestFit, curFit;
	for (k = 0; k < mg; k++) {
		gs = pop.groups[k].groupSize;
		members = pop.groups[k].groupMembers;

		if (pop.groups[k].localLeader != NULL) {
			bestFit = pop.groups[k].localLeader->getFitness();
		}
		else {
			bestFit = -1;
		}
		bestMember = -1;

		for (i = 0; i < gs; i++) {
			curFit = members[i]->getFitness();
			//if the new individual is better, 
			if (curFit > bestFit) {
				bestFit = curFit;
				bestMember = i;
			}
		}
		if (bestMember != -1) {
			//assigning the localLeader
			pop.groups[k].localLeader = members[bestMember];
			pop.groups[k].maxFitness = members[bestMember]->getFitness();
		}
		else {
			//if the position havent been updated, increment the local limit count
			if (!selection) {
				pop.groups[k].localLimitCount += 1;
			}
		}
	}
}

void GlobalLeaderLearningPhase(Pop& pop, int selection) {
	int popSize = pop.popSize, i, bestMember;
	SM** members;
	double bestFit, curFit;
	members = pop.population;

	if (pop.globalLeader != NULL) {
		bestFit = pop.globalLeader ->getFitness();
	}
	else {
		bestFit = -1;
	}
	bestMember = -1;

	for (i = 0; i < popSize; i++) {
		curFit = members[i]->getFitness();
		//if the new individual is better, 
		if (curFit > bestFit) {
			bestFit = curFit;
			bestMember = i;
		}
	}
	if (bestMember != -1) {
		//assigning the localLeader
		pop.globalLeader = members[bestMember];
		if (pop.bestSolution == NULL) {
			pop.bestSolution = new SM(*pop.globalLeader);
		}
		else {
			if(bestFit > pop.bestSolution->getFitness())
				*pop.bestSolution = *pop.globalLeader;
		}
	}
	else {
		//if the position havent been updated, increment the local limit count
		if (!selection) {
			pop.globalLimitCount += 1;
		}
	}
	
}

void LocalLeaderDecisionPhase(Pop& pop, int maxDim, const double* minVal, const double* maxVal, int offset, CameraModel* models) {
	int i, j, k, mg = pop.numGroups, gs, numCams;
	SM** members, *sm, *localLeader, *globalLeader;
	globalLeader = pop.globalLeader;
	for (k = 0; k < mg; k++) {
		if (pop.groups[k].localLimitCount > pop.localLeaderLimit) {
			pop.groups[k].localLimitCount = 0;
			gs = pop.groups[k].groupSize;
			members = pop.groups[k].groupMembers;
			localLeader = pop.groups[k].localLeader;
			for (i = 0; i < gs; i++) {
				sm = members[i];
				numCams = 0;
				for (j = 0; j < maxDim; j++) {
					if (rand01(0, 1) > pop.pr) {
						(*sm)[j] = (minVal)[j % offset] + rand01(0, 1) * (maxVal[j % offset] - minVal[j % offset]);
					}
					else {
						(*sm)[j] = (*sm)[j] + rand01(0, 1) * ((*globalLeader)[j] - (*sm)[j]) + rand01(0, 1) * ((*sm)[j] - (*localLeader)[j]);
					}

					//correcting dimensions
					dimCorrection(j, offset, numCams, models, sm);
				}
				sm->setNumCams(numCams);
			}
		}
	}
}

void GlobalLeaderDecisionPhase(Pop& pop) {
	int i;
	if (pop.globalLimitCount > pop.globalLeaderLimit) {
		pop.globalLimitCount = 0;

		//freeing memory from previous group
		for (i = 0; i < pop.numGroups; i++)
			delete pop.groups[i].groupMembers;
		
		if (pop.numGroups < pop.MG) {
			//Spliting into more groups
			pop.numGroups = pop.numGroups + 1;
		}
		else {
			//Merging into one group
			pop.numGroups = 1;
		}

		splitGroups(pop);

		LocalLeaderLearningPhase(pop, 1);
	}
}

void SMOAlgorithm(Pop& pop, int maxDim, const double* minVal, const double* maxVal, int offset, CameraModel* models, int height, int width, int* env, int * priority, int nIter
	, double(*objectiveFunction)(Camera* solution, int maxCams, int height, int width, int* visibilityMatrix, int* priorityMatrix)) {
	int i;
	initPopulation(pop, maxDim, minVal, maxVal, offset, models, height, width);
	//generating the visibility matrix for each member
	for (i = 0; i < pop.popSize; i++) {
		pop.population[i]->genVisibilityMatrix(env, height, width);
		pop.population[i]->fitSM(height, width, priority, objectiveFunction);
	}
	LocalLeaderLearningPhase(pop, 1);
	GlobalLeaderLearningPhase(pop, 1);
	for (i = 0; i < nIter; i++) {
		LocalLeaderPhase(pop, maxDim, offset, models);
		//Calculate prob for each SM

		GlobalLeaderPhase(pop, maxDim);
		LocalLeaderLearningPhase(pop);
		GlobalLeaderLearningPhase(pop);
		LocalLeaderDecisionPhase(pop, maxDim, minVal, maxVal, offset, models);
		GlobalLeaderDecisionPhase(pop);
	}
}
