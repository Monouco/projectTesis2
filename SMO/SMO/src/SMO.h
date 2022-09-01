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


typedef struct Group {
	SM* localLeader;
	SM** groupMembers;
	int groupSize;
	int localLimitCount;
	double maxFitness;
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
	int numGroups;
	SM* bestSolution;
	SM** population;
	Group* groups;
	int height;
	int width;
} Pop;

double rand01(double min, double max);
double randInt(double min, double max, double i = -1);

//Reading files
void readEnv(const char* fileName, int* env, int* feasible, int& height, int& width, Node* wIndex, Node* hIndex);
void readModels(const char* fileName, CameraModel* models, int& numModels);

//SMO
void initPopulation(Pop & pop, int maxDim, const double* minVal, const double* maxVal, int offset, CameraModel* models, int height, int width);
void splitGroups(Pop& pop);
void LocalLeaderPhase(Pop& pop, int maxDim, int offset, CameraModel* models);
void GlobalLeaderPhase(Pop& pop, int maxDim);
void LocalLeaderLearningPhase(Pop& pop, int selection = 0);
void GlobalLeaderLearningPhase(Pop& pop, int selection = 0);
void LocalLeaderDecisionPhase(Pop& pop, int maxDim, const double* minVal, const double* maxVal, int offset, CameraModel* models);
void GlobalLeaderDecisionPhase(Pop& pop);

void SMOAlgorithm(Pop& pop, int maxDim, const double* minVal, const double* maxVal, int offset, CameraModel* models, int height, int width);

#endif // !SMO_H
