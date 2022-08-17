#ifndef TEXTURE_EDITOR_CLASS_H
#define TEXTURE_EDITOR_CLASS_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb/stb_image.h>
#include <fstream>
#include <chrono>

#include "shaders/shaderClass.h"
#include "Texture.h"
#include "TextureStack.h"
#include "Shape.h"

#define PRECISION 0.0000000001

typedef struct objectT {
	int xIni;
	int yIni;
	int xEnd;
	int yEnd;
	int mode;
	figure toDraw;
	int createdBy;
} objectT;

class TextureEditor : public  Texture {
	public:
		GLuint height;
		GLuint width;
		int mode;
		int* arrChanged;
		GLuint xLowerBound;
		GLuint xUpperBound;
		GLuint yLowerBound;
		GLuint yUpperBound;

		//To draw shapes, lets use an a
		objectT shape;
		int* arrGhostBuffer; //this will be the layer that contains the shape  to be drawn before converting it to part of the image
		int* prevGhostShape; //this will contain info where the shape was on previous frame

		//to redo actions
		StackT* redoBuff;
		//to save inputs
		int prevRedo;
		int prevSave;
		int prevOpen;
		int prevFill;

		TextureEditor(GLuint height, GLuint width, GLenum texType, GLenum slot, GLenum format, GLenum pixelType, GLuint screenWidth, GLuint screenHeight, double squareX, double squareY, GLfloat scale);
		
		void processInput(GLFWwindow* window);
		void refresh(int mode = 0);
		void saveTexture(const char * fileName);
		void openTexture(const char * fileName);

	private:
		int lastMouseState;

		void translatePosition(GLuint height, GLuint width, double x, double y, int& xPosT, int& yPosT);

		GLfloat minMaxScaler(double x, GLuint width);
		double reverseMinMaxScaler(double x, GLuint width);

		void drawShape(int xIni, int yIni, int xEnd, int yEnd, int* arrayBuffer, int* changedArray, int mode, int save, int* destBuffer);


		void paintPixel(int index, int value);
};

#endif // !TEXTURE_EDITOR_CLASS_H
