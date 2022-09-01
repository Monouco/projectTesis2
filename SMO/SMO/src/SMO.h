#ifndef SMO
#define SMO

#define _USE_MATH_DEFINES

#include <cstdlib>
#include <cmath>
#include <fstream>
#include <iostream>
#include <algorithm>

#include "SM.h"
#include "BST.h"


/*typedef struct SM {
	int numCams;
	void* solution;
	double fitness;
	double prob;
} SM;*/

typedef struct Group {
	SM* localLeader;
	SM* groupMembers;
	int groupSize;
	int localLimitCount;
} Group;

typedef struct Pop {
	SM* globalLeader;
	int popSize;
	int globalLeaderLimit;
	int localLeaderLimit;
	int MG;
	double pr;
	double maxFitness;
	int globalLimitCount;
	SM* bestSolution;
	SM** population;
	Group* groups;
} Pop;

double rand01(double min, double max);
double randInt(double min, double max);

//Reading files
void readEnv(const char* fileName, int* env, int* feasible, int& height, int& width, Node* wIndex, Node* hIndex);
void readModels(const char* fileName, CameraModel* models, int& numModels);

//SMO
void initPopulation(Pop & pop, int maxDim, const double* minVal, const double* maxVal, int offset, CameraModel* models);




#endif // !SMO_H
