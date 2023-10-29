#pragma once

#include "Vectors.h"
#include "Constants.h"
#include <list>



Coordinate FindDesiredCoordinate(float* heights, float desiredheight) {
	Coordinate DesiredCoordinate = { 0, 0 };
	for (int i = 0; i < HEIGHT; i++)
		for (int j = 0; j < WIDTH; j++) {
			if (heights[i * WIDTH + j] - desiredheight < heights[DesiredCoordinate.y * WIDTH + DesiredCoordinate.x] - desiredheight) {
				float flower = heights[i * WIDTH + j];
				DesiredCoordinate.y = i;
				DesiredCoordinate.x = j;
			}
		}
	return DesiredCoordinate;
}

list<Coordinate> GetListOfTopologicalLineDots(Coordinate dotOfTopologicalLinesStart, float* heights, float lengthOfTopologicalLinesDetalization) {
	list<Coordinate> dotsOfTopologicalLine;
	dotsOfTopologicalLine.push_back(dotOfTopologicalLinesStart);

	auto dot = dotsOfTopologicalLine.begin();
	Coordinate dotOfFastestDescent = { -1, -1 };
	Vectorfloat directionTopologicalLineFirst = { 0.f, 1.f }, directionTopologicalLineSecond = { 0.f, -1.f }, directionTopologicalLineSecondSaved = { 0.f, -1.f };

	int countOfTopologicalLineTurns = 32;
	float angleOfTopologicalLineTurn = 2 * M_PI / countOfTopologicalLineTurns;
	float maximumCosineOfTopologicalLineTurn = 0.2f;

	for (int k = 0; k < 10000; k++) {
		float min = 1.f, phi;

		for (int i = 0; i < countOfTopologicalLineTurns; i++) {
			phi = angleOfTopologicalLineTurn * i;
			directionTopologicalLineSecond = Rotate(directionTopologicalLineSecond, phi);

			if (GetScalarProduct(directionTopologicalLineFirst, directionTopologicalLineSecond) < maximumCosineOfTopologicalLineTurn)
				continue;

			int xShtrih = ((*dot).x + directionTopologicalLineSecond.x * lengthOfTopologicalLinesDetalization),
				yShtrih = ((*dot).y + directionTopologicalLineSecond.y * lengthOfTopologicalLinesDetalization);

			if (yShtrih < HEIGHT && xShtrih < WIDTH && yShtrih >= 0 && xShtrih >= 0) {
				float amountOfDescent = fabs(heights[(*dot).y * WIDTH + (*dot).x] - heights[yShtrih * WIDTH + xShtrih]);
				if (amountOfDescent < min) {
					min = heights[(*dot).y * WIDTH + (*dot).x] - heights[yShtrih * WIDTH + xShtrih];
					dotOfFastestDescent = { xShtrih, yShtrih };
					directionTopologicalLineSecondSaved = directionTopologicalLineSecond;
				}
			}

		}

		directionTopologicalLineFirst = directionTopologicalLineSecondSaved;

		dotsOfTopologicalLine.push_back(dotOfFastestDescent);
		++dot;
		if (dotOfFastestDescent.y * WIDTH + dotOfFastestDescent.x >= 0 && dotOfFastestDescent.y * WIDTH + dotOfFastestDescent.x < HEIGHT * WIDTH)
		{
			if (heights[dotOfFastestDescent.y * WIDTH + dotOfFastestDescent.x] < SEA_LEVEL)
				break;
		}
		else
			break;
	}
	return dotsOfTopologicalLine;
}