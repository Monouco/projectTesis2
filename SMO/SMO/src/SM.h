#ifndef SM_H
#define SM_H
#include "Camera.h"

#ifndef C_OFFSET
#define C_OFFSET 5
#endif // !C_OFFSET


class SM
{
public:
	SM(int maxCams, int height, int width);
	~SM();
	void setNumCams(int num);
	void setModelCam(CameraModel* cam, int j);
	double& operator[](int j);
	void genVisibilityMatrix(int* env, int height, int width);
	void fitSM(int height, int width, int* priorityMatrix,
		double(*func)(Camera* solution, int maxCams, int height, int width, int* visibilityMatrix, int* priorityMatrix));


private:
	int numCams;
	Camera* solution;
	double fitness;
	double prob;
	int* visibilityMatrix;
	int maxCams;
};


#endif // !SM_H
