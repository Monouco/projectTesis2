#ifndef GENETICALGORITHM_H
#define GENETICALGORITHM_H

#define _USE_MATH_DEFINES

#include <cstdlib>
#include <cmath>

#include "Chromosome.h"

typedef std::array<Chromosome*, 2> parents;
typedef std::array<Chromosome*, 2> offspring;
typedef std::vector<parents> matingPool;
typedef std::list<Chromosome> sortedPop;

typedef struct Pop {
	int popSize;
	int maxPopSize;
	int maxMutGenes;
	int numPos;
	double mutRate;
	double crossRate;
	double bestRate;

	Chromosome* bestSolution;
	Chromosome** population;
	Chromosome** newPopulation;

	int height;
	int width;
}Pop;

double rand01(double min, double max);
double randInt(double min, double max, double i = -1);

//Reading files
void readEnv(const char* fileName, int*& env, int*& feasible, int*& priority, int& height, int& width, int offset, int maxCam, int numModels,
			int& numPos, Position*& basePositions, int numSectors, double* sectorsOffset);
void readModels(const char* fileName, CameraModel*& models, int& numModels, int& numSectors, double*& sectorsOffset);

//Genetic Algorithm

int roulette(Chromosome** population, int popSize, double totFit, int prev=-1);
sortedPop::iterator roulette(sortedPop population, double totFit);

void initPopulation(Pop& pop, CameraModel* models, int numModels, int height, int width, int maxCams, int numPos, Position* positions);

parents selectParents(Pop pop);
offspring crossover(Chromosome* firstParent, Chromosome* secondParent, int numPos, Position* basePositions);
void mutation(Chromosome* individual, CameraModel* models, int numModels, int  maxMutGenes, int numPos);
void selectSurvivors(Pop& pop, int newPopSize);
void correction(Chromosome* individual, CameraModel* models, int numModels, int maxCam);

void GeneticAlgorithm(Pop& pop, CameraModel* models, int numModels, int height, int width, int* env, int* priority, int nIter,
	int maxCams, Position* positions
	, FitnessStruct(*objectiveFunction)(Camera* solution, int maxCams, int height, int width, int* visibilityMatrix, int* priorityMatrix));

#endif // !GENETICALGORITHM_H

