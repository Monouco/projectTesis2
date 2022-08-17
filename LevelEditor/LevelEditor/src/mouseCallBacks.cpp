#include "level/mouseCallBacks.h"
using namespace std;

void cursorPositionCallback(GLFWwindow* window, double xPos, double yPos) {
	//cout << xPos << "  ,  " << yPos << endl;
}

void mouseButtonCallback(GLFWwindow* window, int button, int action, int mode) {
	cout << "Button :  " << button << endl;
	cout << "Action :  " << action << endl;
	cout << "Mode   :  " << mode << endl <<endl;
}