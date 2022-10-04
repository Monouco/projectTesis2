#include "TextureDisplayer.h"

TextureDisplayer::TextureDisplayer(GLuint height, GLuint width, GLenum texType, GLenum slot, GLenum format, GLenum pixelType, 
							GLuint screenWidth, GLuint screenHeight, double squareX, double squareY, GLfloat scale) : 
							Texture(height, width, texType, slot, format, pixelType) {
	this->height = height;
	this->width = width;

	arrBuffer = (int*) malloc(sizeof(int) * height * width);
	arrChanged = (int*)malloc(sizeof(int) * height * width);


	if (arrBuffer == NULL || arrChanged == NULL ) {
		std::cout << "Error trying to allocate memory to buffer for Texture: " << id << std::endl;
	}
	else {
		for (int i = 0; i < (int)height * (int)width; i++) {
			arrBuffer[i] = 0;
			arrChanged[i] = 0;
		}
	}

	mode = 1;

	double squareXScaled = (squareX + squareX * scale);
	double squareYScaled = (squareY + squareY * scale);

	xLowerBound = reverseMinMaxScaler(squareXScaled * -1, screenWidth);
	xUpperBound = reverseMinMaxScaler(squareXScaled, screenWidth);
	yLowerBound = reverseMinMaxScaler(squareYScaled * -1, screenHeight);
	yUpperBound = reverseMinMaxScaler(squareYScaled, screenHeight);

	//Initialize redo buffer	
	redoBuff = new StackT(10, sizeof(int) * height * width);
	prevRedo = 0;
	prevSave = 0;
	prevFill = 0;
}

void TextureDisplayer::processInput(GLFWwindow* window){
	double xPos, yPos;
	int arrPosX, arrPosY;
	int sizeBuff = width * height, skipRefresh = 0;
	int* tempBuff;
	glfwGetCursorPos(window, &xPos, &yPos);

	//Get inputs that change modes
	if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) {
		mode = 1;
	}
	if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) {
		mode = 2;
	}


	//redo
	if (prevRedo && glfwGetKey(window, GLFW_KEY_Z) == GLFW_RELEASE)
		prevRedo = 0;
	if (!prevRedo && glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
		prevRedo = 1;
		tempBuff = (int*)redoBuff->pop();
		if (tempBuff != NULL) arrBuffer = tempBuff;
		refresh(1);
		return;
	}

	//save file
	if (prevSave && glfwGetKey(window, GLFW_KEY_S) == GLFW_RELEASE)
		prevSave = 0;
	if (!prevSave && glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
		prevSave = 1;
		saveTexture("solution.pz");
		return;
	}

	//open file
	if (prevOpen && glfwGetKey(window, GLFW_KEY_O) == GLFW_RELEASE)
		prevOpen = 0;
	if (!prevOpen && glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
		prevOpen = 1;
		openTexture("level.pz", (mode==1) ? "bestSM.sm" : "bestCh.sm");
		return;
	}

	if(!skipRefresh)
		refresh();
}

void TextureDisplayer::refresh(int mode) {
	int changed = 1;
	int i = 0;
	int* visibility = solutionSM.getVisibilityMatrix();
	for (i = 0; i < (int)width * (int)height; i++) {
		
		//Look for changes in array or if a shape was drawn on that position
		if (mode || arrChanged[i]) {
			if (arrBuffer[i] == 0 || arrBuffer[i] == 2 || arrBuffer[i] == 3 || arrBuffer[i] == 4) {
				//Translate changes into the pixel buffer
				paintPixel(i, 0);
			}
			else {
				paintPixel(i, arrBuffer[i]);
			}
			if (visibility[i] > 0) {
				paintPixel(i, visibility[i] + 4);
			}
			arrChanged[i] = 0;
			changed = 1;
		}
	}
	if (solutionSM.getSolution() != NULL) {
		double x, y;
		
		for (i = 0; i < solutionSM.getMaxCams(); i++) {
			if (round(solutionSM.getSolution()[i].getUsed()) == 0) continue;
			x = solutionSM.getSolution()[i].getX();
			y = solutionSM.getSolution()[i].getY();
			//x = 200;
			//y = 300;
			if (y + 4 < height) {
				paintPixel((height - 1 - (int)y + 1) * width + (int)x, 2);
				paintPixel((height - 1 - (int)y + 2) * width + (int)x, 2);
				paintPixel((height - 1 - (int)y + 3) * width + (int)x, 2);
				paintPixel((height - 1 - (int)y + 4) * width + (int)x, 2);
			}
			if (y - 4 > 0) {
				paintPixel((height - 1 - (int)y - 1) * width + (int)x, 2);
				paintPixel((height - 1 - (int)y - 2) * width + (int)x, 2);
				paintPixel((height - 1 - (int)y - 3) * width + (int)x, 2);
				paintPixel((height - 1 - (int)y - 4) * width + (int)x, 2);
			}
			if (x + 4 < width) {
				paintPixel((height - 1 - (int)y) * width + (int)x + 1, 2);
				paintPixel((height - 1 - (int)y) * width + (int)x + 2, 2);
				paintPixel((height - 1 - (int)y) * width + (int)x + 3, 2);
				paintPixel((height - 1 - (int)y) * width + (int)x + 4, 2);
			}
			if (x - 4 > 0) {
				paintPixel((height - 1 - (int)y) * width + (int)x - 1, 2);
				paintPixel((height - 1 - (int)y) * width + (int)x - 2, 2);
				paintPixel((height - 1 - (int)y) * width + (int)x - 3, 2);
				paintPixel((height - 1 - (int)y) * width + (int)x - 4, 2);
			}
			//std::cout << "Index de la camara " << i << ": "<< (height - 1 - (int)y) * width + (int)x << std::endl;
			//std::cout << "x: " << x << " y: " << y << std::endl;
			//paintPixel( (height - 1 - (int)y) * width + (int)x,2);
			paintPixel( (height - 1 - (int)y) * width + (int)x,2);
		}
	}
	

	//If changed, allow to refresh the texture
	if (changed) {
		refreshTexture(height, width);
	}
}


void TextureDisplayer::saveTexture(const char* fileName)
{
	int* feasiblePoints;
	std::ofstream saveFile(fileName, std::ios::out | std::ios::binary);
	if (!saveFile) {
		std::cout << "Cannot access file "<< fileName << std::endl;
		return ;
	}
	//saving height and width
	saveFile.write((char*) &height, sizeof(int));
	saveFile.write((char*)&width, sizeof(int));
	//saving the array buffer that contains the info of the level
	saveFile.write((char*)arrBuffer,  sizeof(int) * (std::streamsize) height * (std::streamsize) width);
	saveFile.close();

	if (!saveFile.good()) {
		std::cout << "Error at saving file " << fileName << std::endl;
	}
}

void TextureDisplayer::openTexture(const char* fileName, const char* solutionName)
{
	std::ifstream openedFile(fileName, std::ios::out | std::ios::binary);
	if (!openedFile) {
		std::cout << "Cannot access file "<< fileName << std::endl;
		return ;
	}
	//saving height and width
	openedFile.read((char*) &height, sizeof(int));
	openedFile.read((char*)&width, sizeof(int));
	//saving the array buffer that contains the info of the level
	openedFile.read((char*)arrBuffer,  sizeof(int) * (std::streamsize) height * (std::streamsize) width);

	openedFile.close();

	solutionSM.loadSolution(solutionName);

	refresh(1);

}


GLfloat TextureDisplayer::minMaxScaler(double x, GLuint width) {
	GLfloat result = (GLfloat)(x - 0) / (width - 0) * 2 - 1;
	return result;
}

double TextureDisplayer::reverseMinMaxScaler(double x, GLuint width) {
	double result = (x + 1) / 2 * ((double)width - 0);
	return result;
}


void TextureDisplayer::translatePosition(GLuint height, GLuint width, double x, double y, int& xPosT, int& yPosT) {
	xPosT = ((int)x - xLowerBound) * width / (xUpperBound - xLowerBound);
	yPosT = ((int)y - yLowerBound) * height / (yUpperBound - yLowerBound);
}

void TextureDisplayer::paintPixel(int index, int value)
{
	if (value <= 4) {
		switch (value)
		{
		case 0:
			arrTexture[index].r = 255;
			arrTexture[index].g = 255;
			arrTexture[index].b = 255;
			arrTexture[index].a = 255;
			break;
		case 1:
			arrTexture[index].r = 0;
			arrTexture[index].g = 0;
			arrTexture[index].b = 0;
			arrTexture[index].a = 255;
			break;
		case 2:
			arrTexture[index].r = 230;
			arrTexture[index].g = 160;
			arrTexture[index].b = 175;
			arrTexture[index].a = 20;
			break;
		case 3:
			arrTexture[index].r = 250;
			arrTexture[index].g = 100;
			arrTexture[index].b = 100;
			arrTexture[index].a = 20;
			break;
		case 4:
			arrTexture[index].r = 110;
			arrTexture[index].g = 110;
			arrTexture[index].b = 230;
			arrTexture[index].a = 20;
			break;

		default:
			break;
		}
	}
	else {
		int increment = 40;
		arrTexture[index].r = 50 + (value - 4)*increment;
		arrTexture[index].g = 220;
		arrTexture[index].b = 50 + (value - 4) * increment;
		arrTexture[index].a = 100;
	}
}



