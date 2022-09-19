#include "Shape.h"

void drawShape(int xIni, int yIni, int xEnd, int yEnd, int* arrayBuffer, int* changedArray, int mode, int save)
{
}

veci2 drawLineDDA(int xIni, int yIni, int xEnd, int yEnd, int* arrayBuffer, int* changedArray, int mode, int save, int height, int width, int* destBuffer)
{
	if (destBuffer == NULL) destBuffer = arrayBuffer;
	veci2 lineCoord;
	std::array<int, 2> coord;

	int x = 0, y = 0, steps, i, index;
	float mx = 0, my = 0, xf, yf;

	//We'll be using DDA to draw lines
	int dy, dx;

	dy = yEnd - yIni;
	dx = xEnd - xIni;

	//slope < 1
	if (abs(dy) < abs(dx)) {
		steps = abs(dx);
	}
	else {
		//slope >=1
		steps = abs(dy);
	}

	//getting the incremental value for each step
	mx = (float)dx / steps;
	my = (float)dy / steps;

	//the algorithm increments the values of x and y for each step
	for (i = 0, x = xIni, y = yIni, xf = xIni, yf = yIni; i < steps; i++) {
		index = (height - 1 - y) * width + x;
		//avoiding that a zone destroys a wall
		if ((mode == 1 || mode == 0) || arrayBuffer[index] != 1) {
			coord[0] = x;
			coord[1] = y;
			lineCoord.push_back(coord);
			//Writing the point of the line
			destBuffer[index] = mode;
			// if the button has been released, this will change the array
			if (save) {
				changedArray[index] = 1;
			}
		}

		xf = (xf + mx);
		yf = (yf + my);
		x = round(xf);
		y = round(yf);
	}
	return lineCoord;
}

veci2 drawLineBresenham(int xIni, int yIni, int xEnd, int yEnd, int* arrayBuffer, int* changedArray, int mode, int save, int height, int width, int* destBuffer, int draw)
{
	if (destBuffer == NULL) destBuffer = arrayBuffer;
	veci2 lineCoord;
	//Slope < 1
	if (abs(yEnd - yIni) < abs(xEnd - xIni)) {

		if (xIni <= xEnd) {
			lineCoord = bresenhamAlgorithm(xIni, yIni, xEnd, yEnd, arrayBuffer, changedArray, mode, save, height, width, 0, destBuffer, draw);
		}
		else {
			lineCoord =  bresenhamAlgorithm(xEnd, yEnd, xIni, yIni, arrayBuffer, changedArray, mode, save, height, width, 0, destBuffer, draw);
		}
		
	}
	else {//Slope >= 1

		if (yIni <= yEnd) {
			lineCoord =  bresenhamAlgorithm(yIni, xIni, yEnd, xEnd, arrayBuffer, changedArray, mode, save, width, height, 1, destBuffer, draw);
		}
		else {
			lineCoord =  bresenhamAlgorithm(yEnd, xEnd, yIni, xIni, arrayBuffer, changedArray, mode, save, width, height, 1, destBuffer, draw);
		}
	}
	return lineCoord;
}

veci2 drawLineBresenham(ShapeFOV lineP, int* arrayBuffer, int* changedArray, int mode, int save, int height, int width, int* destBuffer, int draw) {
	int xEnd = lineP.xEnd, xIni = lineP.xIni, yEnd = lineP.yEnd, yIni = lineP.yIni;
	return drawLineBresenham(xIni, yIni, xEnd, yEnd, arrayBuffer, changedArray, mode, save, height, width, destBuffer, 0);
}

veci2 bresenhamAlgorithm(int xIni, int yIni, int xEnd, int yEnd, int* arrayBuffer, int* changedArray, int mode, int save, int height, int width, int reversed, int* destBuffer, int draw)
{
	veci2 lineCoord;
	std::array<int, 2> coord;
	int dx, dy, yi = 1, pk, x, y, index, outBounds;
	dx = xEnd - xIni;
	dy = yEnd - yIni;
	//draw line with negative gradient
	if (dy < 0) {
		yi = -1;
		dy = -dy;
	}
	//Decision factor
	pk = (dy << 1) - dx;
	y = yIni;
	for (x = xIni; x < xEnd; x++) {

		if (x < 0 || y < 0 || x >= width || y >= height) outBounds = 1;
		else outBounds = 0;

		if(reversed)
			index = (width - 1 - x) * height + y;
		else
			index = (height - 1 - y) * width + x;

		//avoiding that a zone destroys a wall
		if ((mode == 1 || mode == 0) || outBounds || arrayBuffer[index] != 1) {
			if (draw && !outBounds) {
				if (changedArray != NULL) {
					//Writing the point of the line
					if (mode == -1)
						destBuffer[index] += 1;
					else
						destBuffer[index] = mode;
					// if the button has been released, this will change the array
					if (save) {
						changedArray[index] = 1;
					}
				}
			}
			if (reversed) {
				coord[0] = y;
				coord[1] = x;
			}
			else {
				coord[0] = x;
				coord[1] = y;
			}
			lineCoord.push_back(coord);
		}

		//moving on y and calculating next decision factor
		if (pk > 0) {
			y = y + yi;
			pk = pk + ((dy - dx) << 1);

			if (x < 0 || y < 0 || x >= width || y >= height) outBounds = 1;
			else outBounds = 0;

			if (reversed)
				index = (width - 1 - x) * height + y;
			else
				index = (height - 1 - y) * width + x;

			if ((mode == 1 || mode == 0) || outBounds || arrayBuffer[index] != 1) {
				if (draw && !outBounds) {
					//Writing the point of the line
					if (changedArray != NULL) {

						if (mode == -1)
							destBuffer[index] += 1;
						else
							destBuffer[index] = mode;
						// if the button has been released, this will change the array
						if (save) {
							changedArray[index] = 1;
						}
					}
				}
				if (reversed) {
					coord[0] = y;
					coord[1] = x;
				}
				else {
					coord[0] = x;
					coord[1] = y;
				}
				lineCoord.push_back(coord);
			}

		}
		else {
			pk = pk + (dy << 1);
		}
	}
	return lineCoord;
}

int sectorVal(int secRight, int secLeft, int octant, int xIni, int xEnd, int x, int loop) {
	return (
		((secLeft == octant) && (x >= xIni)) && (!loop || (secRight == 0) || !(x <= xEnd)) ||
		((secRight == 0) && (x <= xEnd)) && (!(secLeft == 0) && (x >= xIni) || loop && !(x >= xIni))
		);
}

veci2 bresenhamCircleAlgorithm(int xCent, int yCent, int xIni, int yIni, int xEnd, int yEnd, int radius, double aRight, double aLeft, int* arrayBuffer, int* changedArray, int mode, int save, int height, int width, int* destBuffer, int draw)
{
	if (destBuffer == NULL) destBuffer = arrayBuffer;
	veci2 lineCoord;
	std::array<int, 2> coord;

	//first lets find on which sector is each point
	unsigned int secFlag = 0;
	int secLeft, secRight, secTemp, i, loop = 0;
	secLeft = ((int)ceil(aLeft) / A_OCTANT) % 8;
	secRight = ((int)ceil(aRight) / A_OCTANT) % 8;
	
	secTemp = (secLeft >= secRight) ? secLeft : secLeft + 8;
	//if the FOV made a circle or almost a full circle
	if (aLeft >= 360 && secLeft >= secRight) {
		loop = 1;
		secTemp += 8;
	}
	//rise the flag of the sectors between the two points
	for (i = secRight + 1; i < secTemp; i++) {
		secFlag |= 1 << (i % 8);
	}
	//starting points on the second octant
	int x, y = radius;
	//intial decision factor
	int pk = 3 - 2 * radius;

	for (x = 0; y >= x; x++) {

		//drawing the circle on the 8 octants
		//First Octant
		if (secFlag & FIRST_SEC || sectorVal(secRight, secLeft, 0, xIni, xEnd, xCent + y, loop)) {
			coord[0] = xCent + y;
			coord[1] = yCent + x;
			lineCoord.push_back(coord);
		}
		//Second Octant
		if (secFlag & SECOND_SEC || sectorVal(secRight, secLeft, 1, xIni, xEnd, xCent + x, loop)) {
			coord[0] = xCent + x;
			coord[1] = yCent + y;
			lineCoord.push_back(coord);
		}
		//Third Octant
		if (secFlag & THIRD_SEC || sectorVal(secRight, secLeft, 2, xIni, xEnd, xCent - x, loop)) {
			coord[0] = xCent - x;
			coord[1] = yCent + y;
			lineCoord.push_back(coord);
		}
		//Fourth Octant
		if (secFlag & FOURTH_SEC || sectorVal(secRight, secLeft, 3, xIni, xEnd, xCent - y, loop)) {
			coord[0] = xCent - y;
			coord[1] = yCent + x;
			lineCoord.push_back(coord);
		}
		//Fifth Octant
		if (secFlag & FIFTH_SEC || sectorVal(secLeft, secRight, 4, xEnd, xIni, xCent - y, loop)) {
			coord[0] = xCent - y;
			coord[1] = yCent - x;
			lineCoord.push_back(coord);
		}
		//Sixth Octant
		if (secFlag & SIXTH_SEC || sectorVal(secLeft, secRight, 5, xEnd, xIni, xCent - x, loop)) {
			coord[0] = xCent - x;
			coord[1] = yCent - y;
			lineCoord.push_back(coord);
		}
		//Seventh Octant
		if (secFlag & SEVENTH_SEC || sectorVal(secLeft, secRight, 6, xEnd, xIni, xCent + x, loop)) {
			coord[0] = xCent + x;
			coord[1] = yCent - y;
			lineCoord.push_back(coord);
		}
		//Eight Octant
		if (secFlag & EIGHTH_SEC || sectorVal(secLeft, secRight, 7, xEnd, xIni, xCent + y, loop)){
			coord[0] = xCent + y;
			coord[1] = yCent - x;
			lineCoord.push_back(coord);
		}

		//calculating the decision factor for the next iteration
		if (pk >= 0)
		{
			y--;
			pk = pk + 4 * (x - y) + 10;
		}
		else {
			pk = pk + 4 * x + 6;
		}
	}

	//cleaning the duplicate values
	lineCoord.unique([](std::array<int, 2> a, std::array<int, 2> b) {
			return a[0] == b[0] && a[1] == b[1];
		});

	return lineCoord;
}

veci2 bresenhamCircleAlgorithm(ShapeFOV param, int* arrayBuffer, int* changedArray, int mode, int save, int height, int width,  int* destBuffer, int draw) {
	int xCent = param.xCent, xIni = param.xIni, xEnd = param.xEnd, yCent = param.yCent, yIni = param.yIni, yEnd = param.yEnd, radius = param.radius;
	double aRight = param.aRight, aLeft = param.aLeft;
	return bresenhamCircleAlgorithm(xCent, yCent, xIni, yIni, xEnd, yEnd, radius, aRight, aLeft, arrayBuffer, changedArray, mode, save, height, width, destBuffer, draw);
}

void castRayBresenham(int xIni, int yIni, int xEnd, int yEnd, int* arrayBuffer, int* changedArray, int mode, int save, int height, int width, int* destBuffer, int* checkedCoords)
{
	if (destBuffer == NULL) destBuffer = arrayBuffer;
	//Slope < 1
	if (abs(yEnd - yIni) < abs(xEnd - xIni)) {

		if (xIni <= xEnd) {
			bresenhamCastAlgorithm(xIni, yIni, xEnd, yEnd, arrayBuffer, changedArray, mode, 1, 0, save, height, width, 0, 0, destBuffer, checkedCoords);
		}
		else {
			bresenhamCastAlgorithm(xEnd, yEnd, xIni, yIni, arrayBuffer, changedArray, mode, 1, 0, save, height, width, 0, 1, destBuffer, checkedCoords);
		}
		
	}
	else {//Slope >= 1

		if (yIni <= yEnd) {
			bresenhamCastAlgorithm(yIni, xIni, yEnd, xEnd, arrayBuffer, changedArray, mode, 1, 0, save, width, height, 1, 0, destBuffer, checkedCoords);
		}
		else {
			bresenhamCastAlgorithm(yEnd, xEnd, yIni, xIni, arrayBuffer, changedArray, mode, 1, 0, save, width, height, 1, 1, destBuffer, checkedCoords);
		}
	}
}

void bresenhamCastAlgorithm(int xIni, int yIni, int xEnd, int yEnd, int* arrayBuffer, int* changedArray, int mode, int boundValue, int clearValue, int save, int height, int width, int yReversed, int pointsReversed, int* destBuffer, int* checkedCoords)
{
	int dx, dy, yi = 1, d, x, y, index, outBounds;
	veci3 lineCoords;
	std::array<int, 3> coord;

	dx = xEnd - xIni;
	dy = yEnd - yIni;
	//draw line with negative gradient
	if (dy < 0) {
		yi = -1;
		dy = -dy;
	}
	//Decision factor
	d = (dy << 1) - dx;
	y = yIni;

	
	//this might be optimize if a guardian is added to check if are not pointsReversed so it can skip to add the coords to the list and break the iteration 
	for (x = xIni; x < xEnd; x++) {
		//check for bounds
		if (x < 0 || y < 0 || x >= width || y >= height) outBounds = 1;
		else outBounds = 0;

		//saving point on the list
		coord[0] = x;
		coord[1] = y;

		if (yReversed) {
			index = (width - 1 - x) * height + y;
		}
		else {
			index = (height - 1 - y) * width + x;
		}

		//avoiding that a zone destroys a wall
		if ((mode == boundValue || mode == clearValue) || !outBounds && arrayBuffer[index] != boundValue) {
			//Writing the point of the line by not raising the flag
			coord[2] = 0;
		}
		else {
			//if points have not been reversed, break the iteration
			if (!pointsReversed) break;
			//Bound detected, so mark it with a flag
			coord[2] = 1;
		}

		//adding points to the list
		lineCoords.push_back(coord);


		//moving on y and calculating next decision factor
		if (d > 0) {
			y = y + yi;
			d = d + ((dy - dx) << 1);

			if (x < 0 || y < 0 || x >= width || y >= height) outBounds = 1;
			else outBounds = 0;

			//updating y
			coord[1] = y;

			if (yReversed)
				index = (width - 1 - x) * height + y;
			else
				index = (height - 1 - y) * width + x;

			if ((mode == boundValue || mode == clearValue) || !outBounds && arrayBuffer[index] != boundValue) {
				//Writing the point of the line by not raising the flag
				coord[2] = 0;
			}
			else {
				//if points have not been reversed, break the iteration
				if (!pointsReversed) break;
				//Bound detected, so mark it with a flag
				coord[2] = 1;
			}

			//adding points to the list
			lineCoords.push_back(coord);

		}
		else {
			d = d + (dy << 1);
		}
	}

	if (pointsReversed && !lineCoords.empty()) lineCoords.reverse();
	
	while (!lineCoords.empty()) {
		coord = lineCoords.front();
		lineCoords.pop_front();

		if (coord[2] == 1) break;

		if (yReversed)
			index = (width - 1 - coord[0]) * height + coord[1];
		else
			index = (height - 1 - coord[1]) * width + coord[0];

		//out of bounds
		if (coord[0] < 0 || coord[1] < 0 || coord[0] >= width || coord[1] >= height) break;

		if (checkedCoords[index] == 0) {
			//marking coord as visited
			checkedCoords[index] = 1;
			//Writing the point of the line		
			if (mode == -1)
				destBuffer[index] += 1;
			else
				destBuffer[index] = mode;
			// if the button has been released, this will change the array
			if (save) {
				changedArray[index] = 1;
			}
		}
	}

}

void drawRect(int xIni, int yIni, int xEnd, int yEnd, int* arrayBuffer, int* changedArray, int mode, int save, int height, int width, int* destBuffer)
{
	if (destBuffer == NULL) destBuffer = arrayBuffer;
	int x, y, index;

	//sorting the values on x and y
	x = (xIni < xEnd) ? xEnd : xIni;
	xIni = xEnd + xIni - x;
	xEnd = x;
	y = (yIni < yEnd) ? yEnd : yIni;
	yIni = yEnd + yIni - y;
	yEnd = y;

	for (y = yIni; y <= yEnd; y++) {
		for (x = xIni; x <= xEnd; x++) {
			if (mode == 1 && x != xIni && x != xEnd && y != yIni && y != yEnd) continue;
			index = (height - 1 - y) * width + x;
			//avoiding that a zone destroys a wall
			if ((mode != 1 && mode != 0) && arrayBuffer[index] == 1) continue;
			//Writing the point of the rectangle
			destBuffer[index] = mode;
			// if the button has been released, this will change the array
			if (save) {
				changedArray[index] = 1;
			}
		}
	}
}

void calculateTriangle(int xIni, int yIni, int wd, double afov, double offset, int* arrayBuffer, int* changedArray, int mode, int save, int height, int width,
	veci2(*func)(ShapeFOV, int*, int*, int, int, int, int, int*, int), int* destBuffer)
{
	int y1 = yIni, y2, y3, x1 = xIni, x2, x3, * checkedCoords = new int[height*width];
	double radAFOV = afov * M_PI / 180 ;
	//the offset is the angle to de center to the FOV
	double radOffset = (offset - afov/2) * M_PI / 180;
	if (radOffset < 0) radOffset += 2 * M_PI;
	double radius = wd;
	//double radius = abs(wd / cos(radAFOV / 2));
	veci2 lineCoord;
	ShapeFOV params;
	std::array<int, 2> coord;

	//calculating the other vertices for the triangle
	//since we have the origin point (v1), the offset angle and the angle between of v2 v1 v3 
	//to calculate v3: x = radius * cos(offset), y = radius * sin(offset)
	x3 = x1 + round(radius * cos(radOffset));
	y3 = y1 +round(radius * sin(radOffset));
	
	//to calculate v2: x = radius * cos(offset + afov), y = radius * sin(offset + afov)
	x2 = x1 + round(radius * cos(radOffset + radAFOV));
	y2 = y1 + round(radius * sin(radOffset + radAFOV));

	//assigning the values to shapeFOV
	params.radius = radius;
	params.xCent = x1;
	params.xIni = x2;
	params.xEnd = x3;
	params.yCent = y1;
	params.yIni = y2;
	params.yEnd = y3;
	params.aRight = ((offset - afov / 2) < 0) ? (offset - afov / 2) + 360  : (offset - afov / 2);
	params.aLeft = offset + afov / 2;

	//get the line between v2 and v3 to cast rays
	//lineCoord = drawLineBresenham(params, arrayBuffer, changedArray, mode, save, height, width, destBuffer, 0);
	lineCoord = func(params, arrayBuffer, changedArray, mode, save, height, width, destBuffer, 0);

	//initialize checked coords
	int i;
	for (i = 0; i < height * width; i++)
		checkedCoords[i] = 0;

	//cast ray for each point on the line between v2 and v3
	while (!lineCoord.empty()) {
		coord = lineCoord.front();
		lineCoord.pop_front();
		castRayBresenham(xIni, yIni, coord[0], coord[1], arrayBuffer, changedArray, mode, save, height, width, destBuffer, checkedCoords);
	}
	delete checkedCoords;
}


void floodFill(int x, int y, int newValue, int oldValue, int* arrayBuffer, int* changedArray, int height, int width, int* destBuffer)
{
	if (destBuffer == NULL) destBuffer = arrayBuffer;

	if (newValue == oldValue) return;

	std::queue<std::array<int, 2>> indices;
	int index;
	std::array <int, 2> coord{ x,y };
	//adding the first index for iteration
	indices.push(coord);

	while (!indices.empty()) {
		x = indices.front()[0];
		y = indices.front()[1];
		indices.pop();
		index = (height - 1 - y) * width + x;

		//out of bounds
		if (x < 0 || y < 0 || x >= width || y >= height) continue;
		//if the value is different than the oldValue stop
		if (arrayBuffer[index] != oldValue) continue;

		//changing the old value with the new value
		destBuffer[index] = newValue;
		changedArray[index] = 1;

		if (!changedArray[(height - 1 - y) * width + (x + 1)] || arrayBuffer[(height - 1 - y) * width + (x + 1)] != oldValue) {
			coord[0] = x + 1;
			coord[1] = y;
			indices.push(coord);
		}
		if (!changedArray[(height - 1 - y) * width + (x - 1)] || arrayBuffer[(height - 1 - y) * width + (x - 1)] != oldValue) {
			coord[0] = x - 1;
			coord[1] = y;
			indices.push(coord);
		}
		indices.push({ x - 1, y });
		if (!changedArray[(height - 1 - (y + 1)) * width + x] || arrayBuffer[(height - 1 - (y + 1)) * width + x] != oldValue) {
			coord[0] = x;
			coord[1] = y + 1;
			indices.push(coord);
		}
		indices.push({ x, y + 1 });
		if (!changedArray[(height - 1 - (y - 1)) * width + x] || arrayBuffer[(height - 1 - (y - 1)) * width + x] != oldValue) {
			coord[0] = x;
			coord[1] = y - 1;
			indices.push(coord);
		}

	}
}
