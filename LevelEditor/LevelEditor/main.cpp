/*
 * File:   main.cpp
 * Author: Mauricio Ochoa
 * Code: 20170640
 *
 * Created on July 18, 2022, 5:47 PM
 */


#include <iostream>
#include <iomanip>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

using namespace std;

/*
 *
 */
int main(int argc, char** argv) {

    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 800, "Test", NULL, NULL);
    if (window == NULL) {
        cout << "Failed to create GLFW window" << endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}

