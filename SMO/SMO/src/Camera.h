#ifndef CAMERA_CLASS_H
#define CAMERA_CLASS_H
#include <iostream>

#include "Shape.h"

typedef struct CameraModel {
	double sensor;
	double focalLength;
	double resolution;
	double range;
	double cost;
	double afov;
} CameraModel;

class Camera
{
public:
	Camera();
	~Camera();

	//setters
	void setUsed(double used);
	void setX(double x);
	void setY(double y);
	void setOffset(double offset);
	void setModel(CameraModel * model); 
	//getters
	double getUsed();
	double getX();
	double getY();
	double getVirtX();
	double getVirtY();
	double getOffset();
	CameraModel* getModel();

	//overload
	double& operator[] (int j);

	//methods
	void calcFOV(int* env, int height, int width, int* visibilityMatrix);

private:
	double used;
	double x;
	double y;
	double virtX;
	double virtY;
	double offset;
	double modelId;
	CameraModel* model;	
};



#endif // !CAMERA_CLASS_H
