#include "Camera.h"

Camera::Camera()
{
	model = NULL;
	x = 0;
	y = 0;
	used = 0;
	modelId = 0;
	offset = 0;
}

Camera::~Camera()
{
}


//setters
void Camera::setUsed(double used) {
	this->used = used;
}

void Camera::setX(double x) {
	this->x = x;
}

void Camera::setY(double y) {
	this->y = y;
}

void Camera::setOffset(double offset) {
	this->offset = offset;
}

void Camera::setModel(CameraModel* model)
{
	this->model = model;
}

//getters
double Camera::getUsed() {
	return used;
}

double Camera::getX() {
	return x;
}

double Camera::getY() {
	return y;
}

double Camera::getOffset() {
	return offset;
}

CameraModel* Camera::getModel()
{
	return model;
}

double& Camera::operator[](int j)
{
	switch(j)
	{
	case 0:
		return used;
		break;
	case 1:
		return modelId;
		break;
	case 2:
		return x;
		break;
	case 3:
		return y;
		break;
	case 4:
		return offset;
		break;
	default:
		std::cout << "Array out of bounds" << std::endl;
		exit(0);
		break;
	}
}

void Camera::calcFOV(int * env, int height, int width, int* visibilityMatrix) {
	calculateTriangle(x, y, model->range, model->afov, offset, env, NULL, -1, 0, height, width, &drawLineBresenham, visibilityMatrix);
}