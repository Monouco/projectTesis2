#pragma once
#ifndef _mousecallbacks_h_
#define _mousecallbacks_h_
#include <iostream>
#include <GLFW/glfw3.h>

void cursorPositionCallback(GLFWwindow* window, double xPos, double yPos);
void mouseButtonCallback(GLFWwindow* window, int button, int action, int mode);

#endif // !1
