#ifndef SM_H
#define SM_H
#include "Camera.h"

#include <fstream>
#include <iostream>

#ifndef C_OFFSET
#define C_OFFSET 5
#endif // !C_OFFSET
typedef struct FitnessStruct {
	double mainFO;
	double FO1;
	double FO2;
}FitnessStruct;


class SM
{
public:
	SM(int maxCams, int height, int width);
	SM(const SM& sm, int mode = 1);
	~SM();
	
	//setters
	void setNumCams(int num);
	void setModelCam(CameraModel* cam, int j);

	//getters
	double getFitness();
	FitnessStruct getFullFitness();
	Camera* getSolution();
	double getProb();
	int* getVisibilityMatrix();
	int getNumCams();
	int getMaxCams();
	int getHeight();
	int getWidth();

	double& operator[](int j);
	void genVisibilityMatrix(int* env);
	double fitSM(int* priorityMatrix,
		FitnessStruct(*func)(Camera* solution, int maxCams, int height, int width, int* visibilityMatrix, int* priorityMatrix));
	void calProb(double maxFitness);
	void exportSolution(const char* filename);

	SM& operator=(const SM& sm);

private:
	int numCams;
	Camera* solution;
	double fitness;
	FitnessStruct fullFitness;
	double prob;
	int* visibilityMatrix;
	int maxCams;
	int width;
	int height;
};


#endif // !SM_H
