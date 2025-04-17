#pragma once

#include "Vectors.h"
#include "Constants.h"
#include <list>

bool IsLocalMax(float* matrix, int x, int y) {
	if (x == 0 || x == WIDTH - 1 || y == 0 || y == HEIGHT - 1)
		return false;

	for (int i = -1; i < 2; i++)
		for (int j = -1; j < 2; j++) {
			if (matrix[(y + i) * WIDTH + (x + j)] > matrix[y * WIDTH + x])
				return false;
		}
	return true;
}

Coordinate SearchHighestCoordinate(float* heights) {
	Coordinate highestCoordinate = { 0, 0 };
	for (int i = 0; i < HEIGHT; i++)
		for (int j = 0; j < WIDTH; j++) {
			if (heights[i * WIDTH + j] > heights[highestCoordinate.y * WIDTH + highestCoordinate.x]) {
				highestCoordinate.y = i;
				highestCoordinate.x = j;
			}
		}
	return highestCoordinate;
}


list<Coordinate> ListOfRiversDots(Coordinate dotOfRiversStart, float* heights, float lengthOfRiversDetalization) {
	list<Coordinate> dotsOfRiver;
	dotsOfRiver.push_back(dotOfRiversStart);

	auto dot = dotsOfRiver.begin();
	Coordinate dotOfFastestDescent = { -1, -1 };
	Vectorfloat directionRiverFirst = { 0.f, 1.f }, directionRiverSecond = { 0.f, -1.f }, directionRiverSecondSaved = { 0.f, -1.f };

	int countOfRiverTurns = 32;
	float angleOfRiverTurn = 2 * M_PI / countOfRiverTurns;
	float maximumCosineOfRiverTurn = 0.5f;

	for (int k = 0; k < HEIGHT * WIDTH * 0.33; k++) {
		float max = -1, phi;

		for (int i = 0; i < countOfRiverTurns; i++) {
			phi = angleOfRiverTurn * i;
			directionRiverSecond = Rotate(directionRiverSecond, phi);

			if (GetScalarProduct(directionRiverFirst, directionRiverSecond) < maximumCosineOfRiverTurn)
				continue;

			int xShtrih = ((*dot).x + directionRiverSecond.x * lengthOfRiversDetalization),
				yShtrih = ((*dot).y + directionRiverSecond.y * lengthOfRiversDetalization);

			if (yShtrih < HEIGHT && xShtrih < WIDTH && yShtrih >= 0 && xShtrih >= 0) {
				float amountOfDescent = heights[(*dot).y * WIDTH + (*dot).x] - heights[yShtrih * WIDTH + xShtrih];
				if (amountOfDescent > max) {
					max = heights[(*dot).y * WIDTH + (*dot).x] - heights[yShtrih * WIDTH + xShtrih];
					dotOfFastestDescent = { xShtrih, yShtrih };
					directionRiverSecondSaved = directionRiverSecond;
				}
			}

		}

		directionRiverFirst = directionRiverSecondSaved;

		dotsOfRiver.push_back(dotOfFastestDescent);
		++dot;
		if (dotOfFastestDescent.y * WIDTH + dotOfFastestDescent.x >= 0 && dotOfFastestDescent.y * WIDTH + dotOfFastestDescent.x < HEIGHT * WIDTH)
		{
			if (heights[dotOfFastestDescent.y * WIDTH + dotOfFastestDescent.x] < SEA_LEVEL)
				break;
		}
		else
			break;
	}
	return dotsOfRiver;
}


list<Coordinate> GetListOfRiversDots(Coordinate dotOfRiversStart, float* heights, float lengthOfRiversDetalization) {
	list<Coordinate> listOfDotsOfRiver;
	unordered_set<Coordinate> dotsOfRiver{ {dotOfRiversStart} };
	listOfDotsOfRiver.push_back(dotOfRiversStart);

	auto dot = listOfDotsOfRiver.begin();
	Coordinate dotOfFastestDescent = { -1, -1 };
	Vectorfloat directionRiverFirst = { 0.f, 1.f }, directionRiverSecond = { 0.f, -1.f }, directionRiverSecondSaved = { 0.f, -1.f };

	int countOfRiverTurns = 32;
	float angleOfRiverTurn = 2 * M_PI / countOfRiverTurns;
	float maximumCosineOfRiverTurn = -1.f;

	for (int k = 0; k < HEIGHT * WIDTH * 0.33; k++) {
		float max = -1, phi;

		for (int i = 0; i < countOfRiverTurns; i++) {
			phi = angleOfRiverTurn * i;
			directionRiverSecond = Rotate(directionRiverSecond, phi);

			if (GetScalarProduct(directionRiverFirst, directionRiverSecond) < maximumCosineOfRiverTurn)
				continue;

			int xShtrih = ((*dot).x + directionRiverSecond.x * lengthOfRiversDetalization),
				yShtrih = ((*dot).y + directionRiverSecond.y * lengthOfRiversDetalization);

			if (yShtrih < HEIGHT && xShtrih < WIDTH && yShtrih >= 0 && xShtrih >= 0 
				&& dotsOfRiver.find({ xShtrih, yShtrih }) == dotsOfRiver.end()) {
				float amountOfDescent = heights[(*dot).y * WIDTH + (*dot).x] - heights[yShtrih * WIDTH + xShtrih];
				if (amountOfDescent > max) {
					max = heights[(*dot).y * WIDTH + (*dot).x] - heights[yShtrih * WIDTH + xShtrih];
					dotOfFastestDescent = { xShtrih, yShtrih };
					directionRiverSecondSaved = directionRiverSecond;
				}
			}

		}

		directionRiverFirst = directionRiverSecondSaved;

		dotsOfRiver.insert(dotOfFastestDescent);
		listOfDotsOfRiver.push_back(dotOfFastestDescent);
		++dot;
		if (dotOfFastestDescent.y * WIDTH + dotOfFastestDescent.x >= 0 && dotOfFastestDescent.y * WIDTH + dotOfFastestDescent.x < HEIGHT * WIDTH)
		{
			if (heights[dotOfFastestDescent.y * WIDTH + dotOfFastestDescent.x] < SEA_LEVEL)
				break;
		}
		else
			break;
	}
	return listOfDotsOfRiver;
}