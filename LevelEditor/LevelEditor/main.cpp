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

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 800

using namespace std;

/*
 *
 */
int main(int argc, char** argv) {

    //Init GLFW
    glfwInit();

    //Specify which version of OpenGL to use (3.3)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    //Specify which profile to use (CORE = modern functions)
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    //Create a window
    GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Sexo", NULL, NULL);
    //Error handling
    if (window == NULL) {
        cout << "Failed to create GLFW window" << endl;
        glfwTerminate();
        return -1;
    }
    //add window to context
    glfwMakeContextCurrent(window);

    //Load GLAD to configure OpenGL
    gladLoadGL();

    //Specify the viewport, the window is from 0 to 800
    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

    //Specify the color
    glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glfwSwapBuffers(window);

    //Loop where user interacts with the window
    while (!glfwWindowShouldClose(window)) {
        //Get events on window
        glfwPollEvents();
    }

    //Destroy and end GLFW
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}

