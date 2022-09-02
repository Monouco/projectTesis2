#ifndef TEXTURE_DISPLAYER_CLASS_H
#define TEXTURE_DISPLAYER_CLASS_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb/stb_image.h>
#include <fstream>
#include <chrono>

#include "shaderClass.h"
#include "Texture.h"
#include "TextureStack.h"
#include "SM.h"

#define PRECISION 0.0000000001

class TextureDisplayer : public  Texture {
	public:
		GLuint height;
		GLuint width;
		int mode;
		int* arrChanged;
		GLuint xLowerBound;
		GLuint xUpperBound;
		GLuint yLowerBound;
		GLuint yUpperBound;

		//to redo actions
		StackT* redoBuff;
		//to save inputs
		int prevRedo;
		int prevSave;
		int prevOpen;
		int prevFill;

		SM solutionSM;

		TextureDisplayer(GLuint height, GLuint width, GLenum texType, GLenum slot, GLenum format, GLenum pixelType, GLuint screenWidth, GLuint screenHeight, double squareX, double squareY, GLfloat scale);
		
		void processInput(GLFWwindow* window);
		void refresh(int mode = 0);
		void saveTexture(const char * fileName);
		void openTexture(const char * fileName, const char* solution);

	private:

		void translatePosition(GLuint height, GLuint width, double x, double y, int& xPosT, int& yPosT);

		GLfloat minMaxScaler(double x, GLuint width);
		double reverseMinMaxScaler(double x, GLuint width);


		void paintPixel(int index, int value);
};

#endif // !TEXTURE_EDITOR_CLASS_H
