#pragma once

#define _USE_MATH_DEFINES 

#include "Vectors.h"
#include "Rivers.h"
#include "TopologicalLines.h"
#include <SFML/Graphics.hpp>

using namespace std;
using namespace sf;

enum MapDisplayMode {
	Topographic,
	Geographical
};

Color** Colorize(float* masColor, MapDisplayMode mode);
void DrawRivers(Color** colorMap, float* heights);
void DrawLine(Color** colorMap, Coordinate firstDot, Coordinate secondDot, float* gradientOfThikness, Color lineColor);
void DrawDot(Color** colorMap, Coordinate dot, float brushSize, Color brushColor);
void IncreaseContrast(float* perlineNoise);
bool IsAroundDotsInHeightRangeAndNotFill(float* heightsMap, Color** colorMap, Coordinate dot, float brushSize, float heightRange, float widthOfSearch);
void DrawTopologicalLines(Color** colorMap, float* heights, float heightRange);


Color operator*(Color color, float coeficient) {
	int red, green, blue;
	red = int(color.r * coeficient);
	green = int(color.g * coeficient);
	blue = int(color.b * coeficient);
	if (red > 255) red = 255;
	if (green > 255) green = 255;
	if (blue > 255) blue = 255;
	if (red < 0) red = 0;
	if (green < 0) green = 0;
	if (blue < 0) blue = 0;
	return Color(red, green, blue);
}

void IncreaseContrast(float* perlineNoise) {
	float max = 0.f, min = 1.f;
	for (int i = 0; i < HEIGHT * WIDTH; i++) {
		if (perlineNoise[i] > max)
			max = perlineNoise[i];
		if (perlineNoise[i] < min)
			min = perlineNoise[i];
	}

	for (int i = 0; i < HEIGHT * WIDTH; i++) {
		perlineNoise[i] = (perlineNoise[i] - min) / (max - min);
	}

}

float GetShadow(float* heightsMap, int x, int y) {
	float shadow = 0;
	int shadowLength = 8;
	for (int i = 1; i <= shadowLength; i++) {
		if (y - i >= 0 && x - i >= 0) {
			if (heightsMap[(y - i) * WIDTH + (x - i)] >= heightsMap[y * WIDTH + x] && heightsMap[(y - i) * WIDTH + (x - i)] >= SEA_LEVEL)
				shadow += heightsMap[(y - i) * WIDTH + (x - i)] - heightsMap[y * WIDTH + x];
		}
	}
	return pow(1.f - shadow, 2);
}

Color makeDarker(Color color, float dark) {
	return Color(color.r * dark, color.g * dark, color.b * dark);
}

Color** Colorize(float* heightsMap, MapDisplayMode mode) {
	Color grass, sea, sand, snow;
	snow = Color(230, 230, 255);
	grass = Color(68, 148, 74);
	sea = Color(66, 170, 255);
	sand = Color(255, 204, 51);



	Color** colorMap = new Color * [HEIGHT];
	for (int i = 0; i < HEIGHT; i++)
		colorMap[i] = new Color[WIDTH];

	for (int i = 0; i < HEIGHT; i++)
		for (int j = 0; j < WIDTH; j++) {
			if (heightsMap[i * WIDTH + j] < SEA_LEVEL)
				colorMap[i][j] = sea * (heightsMap[i * WIDTH + j] + (1.f - SEA_LEVEL));
			else if (heightsMap[i * WIDTH + j] < SAND_LEVEL)
				colorMap[i][j] = sand;
			else if (heightsMap[i * WIDTH + j] < GRASS_LEVEL)
				colorMap[i][j] = grass;
			else
				colorMap[i][j] = snow;
		}

	DrawRivers(colorMap, heightsMap);
	if (mode == Geographical)
		for (int i = 0; i < HEIGHT; i++)
			for (int j = 0; j < WIDTH; j++) {
				colorMap[i][j] = (colorMap[i][j] * GetShadow(heightsMap, j, i));
		}
	else if (mode == Topographic) {
		float heightRange = 0.05f;
		DrawTopologicalLines(colorMap, heightsMap, heightRange);
	}
				
	return colorMap;
}

bool IsAroundDotsInHeightRangeAndNotFill(float* heightsMap, Color** colorMap, Coordinate dot, float brushSize, float heightRange, float widthOfSearch) {
	for (int i = -brushSize / 2; i <= brushSize / 2; i++)
		for (int j = -brushSize / 2; j <= brushSize / 2; j++)
			if (dot.y + i >= 0 && dot.x + j >= 0 && dot.y + i < HEIGHT && dot.x + j < WIDTH) {
				Coordinate dinamDot = { dot.x + j, dot.y + i };
				float distance = GetDistance(dot, dinamDot);
				if (distance < brushSize / 2 && Modu(heightsMap[(i + dot.y) * WIDTH + (j + dot.x)], 0.05) < 0.005f && colorMap[i + dot.y][j + dot.x]==Color())
					return true;
			}
	return false;
}

float* GetGradientOfThickness(float* heights, Coordinate firstDot, Coordinate secondDot) {
	Vectorfloat direction = GetDirection(firstDot, secondDot);
	float* gradient = new float[int(GetDistance(firstDot, secondDot))];
	Coordinate currentDot = { -1, -1 };
	
	for (int i = 0; i < int(GetDistance(firstDot, secondDot)); i++) {
		currentDot.x = firstDot.x + direction.x * i;
		currentDot.y = firstDot.y + direction.y * i;
		if (currentDot.x == -1)
			gradient[i] = 2.f;
		else
			gradient[i] = (1.f - heights[currentDot.y * WIDTH + currentDot.x]) * 9.f + 0.5f;
	}
	return gradient;
}

void DrawRivers(Color** colorMap, float* heights) {
	list<Coordinate> dotsOfRiver;

	dotsOfRiver = GetListOfRiversDots(SearchHighestCoordinate(heights), heights, 5.f);

	int itr = 1;
	for (auto currentDot = dotsOfRiver.begin(); currentDot != dotsOfRiver.end(); currentDot++) {
		itr++;
		list <Coordinate>::iterator nextDot = currentDot;
		++nextDot;
		float* gradientOfThikness = GetGradientOfThickness(heights, *nextDot, *currentDot);
		DrawLine(colorMap, *nextDot, *currentDot, gradientOfThikness, Sea);
		if (itr == dotsOfRiver.size())
			break;
	}

}

void DrawTopologicalLines(Color** colorMap, float* heights, float heightRange) {
	float currentHeight = 0.f;
	while (currentHeight < 1.f) {
		list<Coordinate> dotsOfTopologicalLine;

		dotsOfTopologicalLine = GetListOfTopologicalLineDots(FindDesiredCoordinate(heights, currentHeight), heights, 5.f);

		int itr = 1;
		for (auto currentDot = dotsOfTopologicalLine.begin(); currentDot != dotsOfTopologicalLine.end(); currentDot++) {
			itr++;
			list <Coordinate>::iterator nextDot = currentDot;
			++nextDot;
			float* gradientOfThikness = GetGradientOfThickness(heights, *nextDot, *currentDot);
			DrawLine(colorMap, *nextDot, *currentDot, gradientOfThikness, Color::Black);
			if (itr == dotsOfTopologicalLine.size())
				break;
		}

		currentHeight += heightRange;
	}
}

void DrawDot(Color** colorMap, Coordinate dot, float brushSize, Color brushColor = Color(66, 170, 255)) {
	
	for (int i = -brushSize / 2; i <= brushSize / 2; i++)
		for (int j = -brushSize / 2; j <= brushSize / 2; j++)
			if (dot.y + i >= 0 && dot.x + j >= 0 && dot.y + i < HEIGHT && dot.x + j < WIDTH) {
				Coordinate dinamDot = { dot.x + j, dot.y + i };
				float distance = GetDistance(dot, dinamDot);
				if (distance < brushSize / 2)
					colorMap[dot.y + i][dot.x + j] = brushColor;
			}
}

void DrawLine(Color** colorMap, Coordinate firstDot, Coordinate secondDot, float* gradientOfThikness, Color lineColor) {
	Vectorfloat direction = GetDirection(firstDot, secondDot);
	Coordinate currentDot = { 0, 0 };
	

	for (int i = 0; i < GetDistance(firstDot, secondDot); i++) {
		currentDot.x = firstDot.x + direction.x * i;
		currentDot.y = firstDot.y + direction.y * i;
		DrawDot(colorMap, currentDot, gradientOfThikness[i], lineColor);
	}
}
