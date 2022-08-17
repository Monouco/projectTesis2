#include "level/TextureEditor.h"

TextureEditor::TextureEditor(GLuint height, GLuint width, GLenum texType, GLenum slot, GLenum format, GLenum pixelType, 
							GLuint screenWidth, GLuint screenHeight, double squareX, double squareY, GLfloat scale) : 
							Texture(height, width, texType, slot, format, pixelType) {
	this->height = height;
	this->width = width;

	arrBuffer = (int*) malloc(sizeof(int) * height * width);
	arrChanged = (int*)malloc(sizeof(int) * height * width);

	//initializing arrays for ghost layer
	arrGhostBuffer = (int*)malloc(sizeof(int) * height * width);
	prevGhostShape = (int*)malloc(sizeof(int) * height * width);


	if (arrBuffer == NULL || arrChanged == NULL || arrGhostBuffer == NULL || prevGhostShape == NULL) {
		std::cout << "Error trying to allocate memory to buffer for Texture: " << id << std::endl;
	}
	else {
		for (int i = 0; i < (int)height * (int)width; i++) {
			arrBuffer[i] = 0;
			arrChanged[i] = 0;
			arrGhostBuffer[i] = -1;
			prevGhostShape[i] = 0;
		}
	}

	mode = 1;

	double squareXScaled = (squareX + squareX * scale);
	double squareYScaled = (squareY + squareY * scale);

	xLowerBound = reverseMinMaxScaler(squareXScaled * -1, screenWidth);
	xUpperBound = reverseMinMaxScaler(squareXScaled, screenWidth);
	yLowerBound = reverseMinMaxScaler(squareYScaled * -1, screenHeight);
	yUpperBound = reverseMinMaxScaler(squareYScaled, screenHeight);

	lastMouseState = GLFW_RELEASE;

	//lets start with drawing lines
	shape.mode = 1;
	shape.toDraw = LINE;
	shape.xIni = -1;
	shape.yIni = -1;
	shape.xEnd = -1;
	shape.yEnd = -1;
	shape.createdBy = -1;

	//Initialize redo buffer	
	redoBuff = new StackT(10, sizeof(int) * height * width);
	prevRedo = 0;
	prevSave = 0;
	prevFill = 0;
}

void TextureEditor::processInput(GLFWwindow* window){
	double xPos, yPos;
	int arrPosX, arrPosY;
	int sizeBuff = width * height, skipRefresh = 0;
	int* tempBuff;
	glfwGetCursorPos(window, &xPos, &yPos);

	//Get inputs that change modes
	//Changing between lines and rectangles
	if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS) {
		shape.toDraw = LINE;
	}
	if (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS) {
		shape.toDraw = RECTANGLE;
	}
	if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS && shape.createdBy == -1) {
		shape.toDraw = FLOOD;
	}
	if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) {
		mode = 1;
	}
	if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS ) {		
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
		saveTexture("level.pz");
		return;
	}

	//open file
	if (prevOpen && glfwGetKey(window, GLFW_KEY_O) == GLFW_RELEASE)
		prevOpen = 0;
	if (!prevOpen && glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
		prevOpen = 1;
		openTexture("level.pz");
		return;
	}

	if ((int)xPos >= (int)xLowerBound && (int)xPos <= (int)xUpperBound && (int)yPos >= (int)yLowerBound && (int)yPos <= (int)yUpperBound) {

		translatePosition(height, width, xPos, yPos, arrPosX, arrPosY);
		
		//Left Mouse Button
		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS) {
			if (shape.toDraw == FLOOD && !prevFill) {
				redoBuff->push(arrBuffer);
				prevFill = 1;
				floodFill(arrPosX, arrPosY, mode, arrBuffer[(height - 1 - arrPosY) * width + arrPosX], arrBuffer, arrChanged, height, width);
			}
			else {
				if (shape.createdBy == -1 || shape.createdBy == GLFW_MOUSE_BUTTON_1) {
					//initialize shape
					if (lastMouseState == GLFW_RELEASE) {
						shape.xIni = arrPosX;
						shape.yIni = arrPosY;
						shape.mode = mode;
						shape.createdBy = GLFW_MOUSE_BUTTON_1;
					}

					if (shape.xEnd != arrPosX || shape.yEnd != arrPosY) {
						shape.xEnd = arrPosX;
						shape.yEnd = arrPosY;
						drawShape(shape.xIni, shape.yIni, shape.xEnd, shape.yEnd, arrBuffer, arrChanged, shape.mode, 0, arrGhostBuffer);
					}
					else {
						skipRefresh = 1;
					}
					lastMouseState = GLFW_PRESS;
				}
			}
		}
		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1) == GLFW_RELEASE) {
			if (shape.toDraw == FLOOD && prevFill) {
				prevFill =  0;
			}
			else {
				if (shape.createdBy == GLFW_MOUSE_BUTTON_1) {
					//clean shape
					if (lastMouseState == GLFW_PRESS) {
						if (shape.xEnd != arrPosX || shape.yEnd != arrPosY) {
							shape.xEnd = arrPosX;
							shape.yEnd = arrPosY;
						}

						//saving the state before change
						redoBuff->push(arrBuffer);

						drawShape(shape.xIni, shape.yIni, shape.xEnd, shape.yEnd, arrBuffer, arrChanged, shape.mode, 1, NULL);

						shape.xIni = -1;
						shape.yIni = -1;
						shape.xEnd = -1;
						shape.yEnd = -1;
						shape.createdBy = -1;
					}
					lastMouseState = GLFW_RELEASE;
				}
			}
		}

		//Right Mouse Button
		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_2) == GLFW_PRESS) {
			if (shape.toDraw == FLOOD && !prevFill) {
				redoBuff->push(arrBuffer);
				prevFill = 1;
				floodFill(arrPosX, arrPosY, 0, arrBuffer[(height - 1 - arrPosY) * width + arrPosX], arrBuffer, arrChanged, height, width);
			}
			else {
				if (shape.createdBy == -1 || shape.createdBy == GLFW_MOUSE_BUTTON_2) {
					if (lastMouseState == GLFW_RELEASE) {
						shape.xIni = arrPosX;
						shape.yIni = arrPosY;
						shape.mode = 0;
						shape.createdBy = GLFW_MOUSE_BUTTON_2;
					}

					if (shape.xEnd != arrPosX || shape.yEnd != arrPosY) {
						shape.xEnd = arrPosX;
						shape.yEnd = arrPosY;
						drawShape(shape.xIni, shape.yIni, shape.xEnd, shape.yEnd, arrBuffer, arrChanged, shape.mode, 0, arrGhostBuffer);
					}
					else {
						skipRefresh = 1;
					}
					lastMouseState = GLFW_PRESS;
				}
			}
		}
		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_2) == GLFW_RELEASE) {
			if (shape.toDraw == FLOOD && prevFill) {
				prevFill = 0;
			}
			else {
				if (shape.createdBy == GLFW_MOUSE_BUTTON_2) {
					if (lastMouseState == GLFW_PRESS) {
						if (shape.xEnd != arrPosX || shape.yEnd != arrPosY) {
							shape.xEnd = arrPosX;
							shape.yEnd = arrPosY;
						}

						//saving the state before change
						redoBuff->push(arrBuffer);

						drawShape(shape.xIni, shape.yIni, shape.xEnd, shape.yEnd, arrBuffer, arrChanged, shape.mode, 1, NULL);

						shape.xIni = -1;
						shape.yIni = -1;
						shape.xEnd = -1;
						shape.yEnd = -1;
						shape.createdBy = -1;
					}
					lastMouseState = GLFW_RELEASE;
				}
			}
		}
	}
	else {
		skipRefresh = 1;
	}
	if(!skipRefresh)
		refresh();
}

void TextureEditor::refresh(int mode) {
	int changed = 0;
	int i = 0;
	for (i = 0; i < (int)width * (int)height; i++) {

		//Look for changes in array or if a shape was drawn on that position
		if (mode || arrChanged[i] || prevGhostShape[i]) {
			//Translate changes into the pixel buffer
			paintPixel(i, arrBuffer[i]);
			arrChanged[i] = 0;
			changed = 1;
			//cleaning prevGhostShape for next iter
			prevGhostShape[i] = 0;
		}
	}
	//Drawing ghost layer on top of the main layer
	for (i = 0; i < (int)width * (int)height; i++) {
		if (arrGhostBuffer[i] != -1) {
			paintPixel(i, arrGhostBuffer[i]);
			arrGhostBuffer[i] = -1; //cleaning the ghost layer for next shape
			prevGhostShape[i] = 1;
		}
	}

	//If changed, allow to refresh the texture
	if (changed) {
		refreshTexture(height, width);
	}
}

void TextureEditor::saveTexture(const char* fileName)
{
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

void TextureEditor::openTexture(const char* fileName)
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

	if (!openedFile.good()) {
		std::cout << "Error at reading file " << fileName << std::endl;
		return;
	}

	refresh(1);

}


GLfloat TextureEditor::minMaxScaler(double x, GLuint width) {
	GLfloat result = (GLfloat)(x - 0) / (width - 0) * 2 - 1;
	return result;
}

double TextureEditor::reverseMinMaxScaler(double x, GLuint width) {
	double result = (x + 1) / 2 * ((double)width - 0);
	return result;
}


void TextureEditor::translatePosition(GLuint height, GLuint width, double x, double y, int& xPosT, int& yPosT) {
	xPosT = ((int)x - xLowerBound) * width / (xUpperBound - xLowerBound);
	yPosT = ((int)y - yLowerBound) * height / (yUpperBound - yLowerBound);
}


void TextureEditor::drawShape(int xIni, int yIni, int xEnd, int yEnd, int* arrayBuffer, int* changedArray, int mode, int save, int* destBuffer)
{
	//auto start = std::chrono::high_resolution_clock::now();
	//auto stop = std::chrono::high_resolution_clock::now();
	//auto time = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
	switch (shape.toDraw)
	{
		case LINE:
			/*start = std::chrono::high_resolution_clock::now();
			drawLineDDA(xIni, yIni, xEnd, yEnd, arrayBuffer, changedArray, mode, save, height, width, destBuffer);
			stop = std::chrono::high_resolution_clock::now();
			time = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
			std::cout << "Time for DDA: " << time.count() << std::endl;*/
			//start = std::chrono::high_resolution_clock::now();
			//drawLineBresenham(xIni, yIni, xEnd, yEnd, arrayBuffer, changedArray, mode, save, height, width, destBuffer);
			//stop = std::chrono::high_resolution_clock::now();
			//time = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
			//std::cout << "Time for Bresenham: " << time.count() << std::endl << std::endl;
			//castRayBresenham(xIni, yIni, xEnd, yEnd, arrayBuffer, changedArray, mode, save, height, width, destBuffer);
			calculateTriangle(xIni, yIni, 20, 200, 0, arrayBuffer, changedArray, mode, save, height, width, destBuffer);
			break;
		case RECTANGLE:
			drawRect(xIni, yIni, xEnd, yEnd, arrayBuffer, changedArray, mode, save, height, width, destBuffer);
			break;
		default:
			break;
	}
}

void TextureEditor::paintPixel(int index, int value)
{
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

		default:
			break;
	}
}



