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

void readEnv(const char* fileName, int*& env, int*& feasible, int*& priority, int& height, int& width, Node *& wIndex, Node*& hIndex) {
	std::ifstream openedFile(fileName, std::ios::out | std::ios::binary);
	if (!openedFile) {
		std::cout << "Cannot access file " << fileName << std::endl;
		return;
	}
	int* fTemp;
	double i, j;
	int k;
	std::vector<vector3d> wList, hList;
	vector3d wVect, hVect;

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
	
	for (i = 0, k=0; i < height; i = i + 1) {
		for (j = 0; j < width; j = j + 1) {
			//Feasible points
			if (fTemp[k] == 1) {
				wVect[0] = ((double)width - 1 - j) * height + i;
				wVect[1] = j;
				wVect[2] = i;

				hVect[0] = ((double)height - 1 - i) * width + j;
				hVect[1] = j;
				hVect[2] = i;

				wList.push_back(wVect);
				hList.push_back(hVect);
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
			k++;
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

void readModels(const char* fileName, CameraModel*& models, int& numModels)
{
	//Creating one model for now
	models = new CameraModel[1];
	CameraModel newModel;
	
	numModels = 1;
	models[0].cost = 20;
	models[0].focalLength = 10;
	models[0].range = 20;
	models[0].resolution = 720;
	models[0].sensor = 12;
	models[0].afov = 2 * atan(models[0].sensor / (2 * models[0].focalLength)) * 180 / M_PI;
}

void closestFeasiblePoint(SM* newSM, Node* wIndex, Node* hIndex) {
	vector3d xIndex, yIndex;
	double x, y, indexX, indexY;
	int height, width;

	x = round((*newSM)[2]);
	y = round((*newSM)[3]);
	height = newSM->getHeight();
	width = newSM->getWidth();
	indexX = ((double)width - 1 - x) * height + y;
	indexY = ((double)height - 1 - y) * width + x;
	xIndex = searchValue(wIndex, indexX);
	yIndex = searchValue(hIndex, indexY);
	//getting which point is closer
	if (xIndex - indexX < yIndex - indexY) {
		(*newSM)[2] = xIndex[1];
		(*newSM)[3] = xIndex[2];
	}
	else {
		(*newSM)[2] = yIndex[1];
		(*newSM)[3] = yIndex[2];
	}
}

void dimCorrection(int j, int offset, int& numCams, CameraModel * models, SM* newSM, Node* wIndex, Node* hIndex) {
	
	switch (j % offset)
	{
	case 0:
		numCams += round((*newSM)[j]);
		break;
	case 1:
		newSM->setModelCam(&models[(int)round((*newSM)[j])], j);
		break;
	case 3: //setting y, so the closest feasible solution will be selected
		closestFeasiblePoint(newSM, wIndex, hIndex);
		break;
	default:
		break;
	}
}

void initPopulation(Pop & pop, int maxDim,const double* minVal, const double* maxVal, int offset, CameraModel* models, int height, int width, Node* wIndex, Node* hIndex) {
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
			dimCorrection(j, offset, numCams, models, newSM, wIndex, hIndex);
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
		for (j = 0; j < gsk; j++) {
			pop.groups[k].groupMembers[j] = pop.population[i];
			i++;
		}

	}
}

void calcProb(Pop& pop) {
	int i, k, gs, mg = pop.numGroups;
	double maxFit;
	SM** members;
	for (k = 0; k < mg; k++) {
		members = pop.groups[k].groupMembers;
		maxFit = pop.groups[k].maxFitness;
		gs = pop.groups[k].groupSize;
		for (i = 0; i < gs; i++) {			
			members[i]->calProb(maxFit);
		}
	}
}

//Leader Phase
void LocalLeaderPhase(Pop& pop, int maxDim, int offset, CameraModel* models, Node* wIndex, Node* hIndex, int* env, int* priorityMatrix
	, double(*func)(Camera* solution, int maxCams, int height, int width, int* visibilityMatrix, int* priorityMatrix)) {
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
				dimCorrection(j, offset, numCams, models, newSM, wIndex, hIndex);
			}
			newSM->setNumCams(numCams);
			//calculating the fitness value of the new made position
			newSM->genVisibilityMatrix(env);
			newSM->fitSM(priorityMatrix, func);
			//comparing positions
			if (newSM->getFitness() > sm->getFitness()) {
				*sm = *newSM;
			}
		}
	}
}

void GlobalLeaderPhase(Pop& pop, int maxDim, int offset, CameraModel* models, Node* wIndex, Node* hIndex, int* env, int* priorityMatrix
	, double(*func)(Camera* solution, int maxCams, int height, int width, int* visibilityMatrix, int* priorityMatrix)) {
	int k, mg = pop.numGroups, gs, i, j, r, count, numCams, camMut;
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
					numCams = sm->getNumCams();
					//calculating new position
					r = randInt(0, gs, i);
					j = randInt(0, maxDim);
					(*newSM)[j] = (*sm)[j] + rand01(0, 1) * ((*globalLeader)[j] - (*sm)[j]) + rand01(-1, 1) * ((*members[r])[j] - (*sm)[j]);

					//dimension correction
					switch (j % offset)
					{
						//camera usage
					case 0:
						camMut = (int)round((*newSM)[j]) - (int)round((*sm)[j]);
						numCams += camMut;
						newSM->setNumCams(numCams);
						break;
						//camera model
					case 1:
						newSM->setModelCam(&models[(int)round((*newSM)[j])], j);
						break;
						// x position
					case 2:
						closestFeasiblePoint(newSM, wIndex, hIndex);
						break;
						// y position
					case 3:
						closestFeasiblePoint(newSM, wIndex, hIndex);
						break;
					default:
						break;
					}

					//calculating the fitness value of the new made position
					newSM->genVisibilityMatrix(env);
					newSM->fitSM(priorityMatrix, func);
					//comparing positions
					if (newSM->getFitness() > sm->getFitness()) {
						*sm = *newSM;
					}

				}				
				
			}
		}
	}
}

//Selection of new Leaders
void LocalLeaderLearningPhase(Pop & pop, int selection) {
	int mg = pop.numGroups, k, gs, i, bestMember;
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

//Decision phase
void LocalLeaderDecisionPhase(Pop& pop, int maxDim, const double* minVal, const double* maxVal, int offset, CameraModel* models, Node* wIndex, Node* hIndex) {
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
					dimCorrection(j, offset, numCams, models, sm, wIndex, hIndex);
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
		/*for (i = 0; i < pop.numGroups; i++)
			delete pop.groups[i].groupMembers;*/
		
		if (pop.numGroups < pop.MG) {
			//Spliting into more groups
			pop.numGroups = pop.numGroups + 1;
		}
		else {
			//Merging into one group
			pop.numGroups = 1;
		}
		std::cout << "Spliting into more groups" << std::endl;
		splitGroups(pop);

		std::cout << "Getting new Local Leaders" << std::endl;
		LocalLeaderLearningPhase(pop, 1);
		std::cout << "Getting new Global Leader" << std::endl;
		GlobalLeaderLearningPhase(pop, 1);
	}
}

//main algorithm
void SMOAlgorithm(Pop& pop, int maxDim, const double* minVal, const double* maxVal, int offset, CameraModel* models, int height, int width, int* env, int * priority, int nIter, Node* wIndex, Node* hIndex
	, double(*objectiveFunction)(Camera* solution, int maxCams, int height, int width, int* visibilityMatrix, int* priorityMatrix)) {
	int i;
	pop.bestSolution = NULL;
	pop.globalLeader = NULL;

	std::cout << "Initializing population" << std::endl;
	initPopulation(pop, maxDim, minVal, maxVal, offset, models, height, width, wIndex, hIndex);
	//spliting groups
	std::cout << "Spliting population in one group" << std::endl;
	splitGroups(pop);
	//generating the visibility matrix for each member
	std::cout << "Calculating fitness" << std::endl;
	for (i = 0; i < pop.popSize; i++) {
		pop.population[i]->genVisibilityMatrix(env);
		pop.population[i]->fitSM( priority, objectiveFunction);
	}
	std::cout << "Selecting local leaders" << std::endl;
	LocalLeaderLearningPhase(pop, 1);
	std::cout << "Selecting global leader" << std::endl;
	GlobalLeaderLearningPhase(pop, 1);
	for (i = 0; i < nIter; i++) {
		std::cout << "Global Leader Fitness for iteration "<< i  << " : " <<pop.globalLeader->getFitness() << std::endl;
		LocalLeaderPhase(pop, maxDim, offset, models, wIndex, hIndex, env, priority, objectiveFunction);

		//Calculate prob for each SM
		calcProb(pop);

		GlobalLeaderPhase(pop, maxDim, offset, models, wIndex, hIndex, env, priority, objectiveFunction);

		LocalLeaderLearningPhase(pop);
		GlobalLeaderLearningPhase(pop);

		LocalLeaderDecisionPhase(pop, maxDim, minVal, maxVal, offset, models, wIndex, hIndex);
		GlobalLeaderDecisionPhase(pop);
	}
}
