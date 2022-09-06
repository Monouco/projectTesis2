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
#include <gl/GL.h>

#include "src/classes/shaderClass.h"
#include "src/classes/VAO.h"
#include "src/classes/EBO.h"
#include "src/classes/VBO.h"
#include "src/classes/TextureDisplayer.h"


#define SCREEN_WIDTH 1000
#define SCREEN_HEIGHT 800
#define SQUARE_X 0.8f
#define SQUARE_Y 0.8f
#define SCALE_VAL 0.0f
#define PERCENT_WIDTH 0.3f
#define PERCENT_HEIGHT 0.3f


//Vertice definition

GLfloat vertices[] = {
    /*    COORDINATES     |||       COLORS       */
    -SQUARE_X,  -SQUARE_Y,  0.0f,      1.0f, 0.0f, 0.0f,   0.0f,   0.0f,       // lower left corner
    -SQUARE_X,   SQUARE_Y,  0.0f,      0.0f, 1.0f, 0.0f,   0.0f,   1.0f,     // lower right corner
     SQUARE_X,   SQUARE_Y,  0.0f,      0.0f, 0.0f, 0.0f,   1.0f,   1.0f,     // upper corner
     SQUARE_X,  -SQUARE_Y,  0.0f,      1.0f, 1.0f, 1.0f,   1.0f,   0.0f      // inner left corner

};

GLuint indices[] = {
    0, 2, 1,    //upper triangle
    0, 3, 2     //lower triangle
};

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
    GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Level Editor", NULL, NULL);
    //Error handling
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    //set mouse events
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);


    //glfwSetDropCallback(window, )
    //add window to context
    glfwMakeContextCurrent(window);

    //Load GLAD to configure OpenGL
    gladLoadGL();

    //Specify the viewport, the window is from 0 to 800
    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);


    //Creating buffers, VAO helps to switch between different VBOs
    Shader shaderProgram("res/shaders/default.vert", "res/shaders/default.frag");

    //VAO has to be binded before VBO
    VAO vao1;
    vao1.bind(); 

    //Binding buffers
    VBO vbo1(vertices, sizeof(vertices));
    EBO ebo1(indices, sizeof(indices));

    //Specify how to use the VBO, this helps to communicate the shader from the outside
    vao1.linkAttrib(vbo1, 0, 3, GL_FLOAT, 8 * sizeof(float), (void*)0);
    //Specify the colors of each vertex
    vao1.linkAttrib(vbo1, 1, 3, GL_FLOAT, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    vao1.linkAttrib(vbo1, 2, 2, GL_FLOAT, 8 * sizeof(float), (void*)(6 * sizeof(float)));

    //To avoid changing VAO and VBO with another function
    vao1.unbind();    
    vbo1.unbind();
    //EBO is unbind after VBO since VBO is in EBO
    ebo1.unbind();

    //Setting uniforms (just if needed)
    GLuint uniID = glGetUniformLocation(shaderProgram.id, "scale");

    //Texture
    //Texture tex0("res/textures/pandaEmoji.png", GL_TEXTURE_2D, GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE);
    TextureDisplayer tex0(SCREEN_HEIGHT * PERCENT_HEIGHT, SCREEN_WIDTH * PERCENT_WIDTH, GL_TEXTURE_2D, GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE, SCREEN_WIDTH, SCREEN_HEIGHT, SQUARE_X, SQUARE_Y, SCALE_VAL);
    tex0.texUnit(shaderProgram, "tex0", 0);

    //Specify the color
    glClearColor(0.07f, 0.13f, 0.17f, 1.0f);

    //Loop where user interacts with the window
    while (!glfwWindowShouldClose(window)) {

        glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        //Activating program
        shaderProgram.activate();
        //Uniforms can only be access after the shader program is activated
        glUniform1f(uniID, SCALE_VAL);
        tex0.bind();
        //Specify which VAO to use
        vao1.bind();

        //glDrawArrays(GL_TRIANGLES, 0, 3);
        /*Valid modes GL_POINTS, GL_LINE_STRIP, GL_LINE_LOOP, GL_LINES, GL_TRIANGLE_STRIP, GL_TRIANGLE_FAN, and GL_TRIANGLES*/
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        //
        tex0.processInput(window);

        glfwSwapBuffers(window);
        //Get events on window
        glfwPollEvents();
    }

    //Deleting buffers and program
    vao1.deleteVAO();
    vbo1.deleteVBO();
    ebo1.deleteEBO();
    shaderProgram.deleteShader();
    tex0.deleteTexture();

    //Destroy and end GLFW
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}

