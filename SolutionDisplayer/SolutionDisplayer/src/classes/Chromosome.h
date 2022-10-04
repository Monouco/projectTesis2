#ifndef CHROMOSOME_H
#define CHROMOSOME_H

#include "Camera.h"

#include <fstream>
#include <iostream>

typedef std::vector<Camera> CameraArray;

#ifndef FITNESS_STRUCT
#define FITNESS_STRUCT
typedef struct FitnessStruct {
	double mainFO;
	double FO1;
	double FO2;
}FitnessStruct;
#endif //FITNESS_STRUCT

typedef struct Position {
	int x;
	int y;
	double offset;
	int index;
	Camera* cam = NULL;
}Position;

class Chromosome {
public:
	Chromosome(Position* positions, int numPos, int height, int width, int maxCams);
	Chromosome(const Chromosome& base, int finalVal = 0);

	~Chromosome();

	//getters
	double getFitness();
	FitnessStruct getFullFitness();
	Camera* getSolution();
	int* getVisibilityMatrix();
	int getNumCams();
	int getMaxCams();
	int getHeight();
	int getWidth();
	int getNumPos();

	Position& operator[](int j);
	Chromosome& operator=(const Chromosome& base);
	int operator<(const Chromosome& b);
	void setPos(const Position& pos, int j);
	void deleteCam(int pos);
	void correctSolution();

	void genVisibilityMatrix(int* env);
	double fit(int* priorityMatrix,
		FitnessStruct(*func)(Camera* solution, int maxCams, int height, int width, int* visibilityMatrix, int* priorityMatrix));

	void exportSolution(const char* fileName);


private:
	int height;
	int width;
	int maxCams;
	int numCams;
	int numPos;
	Position* positions;
	FitnessStruct fullFitness;
	double fitness;
	int* visibilityMatrix;
	Camera* solution;
	CameraArray cameraArray;
};


#endif // !CHROMOSOME_H
