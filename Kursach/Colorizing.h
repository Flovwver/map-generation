#pragma once

#define _USE_MATH_DEFINES 

#include "Vectors.h"
#include "Rivers.h"
#include <SFML/Graphics.hpp>

using namespace std;
using namespace sf;

enum MapDisplayMode {
	Topographic,
	TopographicContinent,
	Geographic
};

Color** Colorize(float* masColor, MapDisplayMode mode);
void DrawRivers(Color** colorMap, float* heights);
void DrawLine(Color** colorMap, Coordinate firstDot, Coordinate secondDot, float* gradientOfThikness);
void DrawDot(Color** colorMap, Coordinate dot, float brushSize);
void IncreaseContrast(float* perlineNoise);
void DrawRiver(Color** colorMap, float* heights, Coordinate StartDot);

int RoundCooeficient = 50;

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

	for (int i = 0; i < HEIGHT; i++)
		for (int j = 0; j < WIDTH; j++) {
			float corrector = 1.f - (((float)j / WIDTH - 0.5f) * ((float)j / WIDTH - 0.5f) + ((float)i / HEIGHT - 0.5f) * ((float)i / HEIGHT - 0.5f));
			if (corrector < 0) corrector = 0.f;
			perlineNoise[i * WIDTH + j] = pow(corrector, 2) * perlineNoise[i * WIDTH + j];
		}
}

void Cone(float* perlineNoise) {
	for (int i = 0; i < HEIGHT; i++)
		for (int j = 0; j < WIDTH; j++) {
			float corrector = 1.f - (((float)j / WIDTH - 0.5f) * ((float)j / WIDTH - 0.5f) + ((float)i / HEIGHT - 0.5f) * ((float)i / HEIGHT - 0.5f));
			if (corrector < 0) corrector = 0.f;
			perlineNoise[i * WIDTH + j] = pow(corrector, 2) * perlineNoise[i * WIDTH + j];
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

Color SeaColor(float height, Color sea) {
	return sea * ((int(5 * height / (SEA_LEVEL)) / 5.f + 1.f) / 1.5f);
}

Color** Colorize(float* heightsMap, MapDisplayMode mode) {
	Color grass, sea, sand, snow, mountain, hill, elevation, plain;
	snow = Color(230, 230, 255);
	grass = Color(68, 148, 74);
	sea = Color(66, 170, 255);
	sand = Color(255, 204, 51);
	mountain = Color(204, 121, 40);
	hill = Color(255, 192, 1);
	elevation = Color(223, 224, 0);
	plain = Color(109, 174, 52);

	Color** colorMap = new Color * [HEIGHT];
	for (int i = 0; i < HEIGHT; i++)
		colorMap[i] = new Color[WIDTH];

	for (int i = 0; i < HEIGHT; i++)
		for (int j = 0; j < WIDTH; j++) {
			if (heightsMap[i * WIDTH + j] < SEA_LEVEL) {
				colorMap[i][j] = SeaColor(heightsMap[i * WIDTH + j], sea);
			}
			else if (heightsMap[i * WIDTH + j] < SAND_LEVEL)
				colorMap[i][j] = sand;
			else if (heightsMap[i * WIDTH + j] < GRASS_LEVEL)
				colorMap[i][j] = grass;
			else
				colorMap[i][j] = snow;
		}

	for (int i = 0; i < HEIGHT; i++)
		for (int j = 0; j < WIDTH; j++) {
			if (mode == Geographic)
				colorMap[i][j] = (colorMap[i][j] * GetShadow(heightsMap, j, i));
			else if (mode == Topographic) {
				if (heightsMap[i * WIDTH + j] > SEA_LEVEL)
					for (int k = -1; k < 2; k++)
						for (int l = -1; l < 2; l++)
							if (i + k >= 0 && j + l >= 0 && i + k < HEIGHT && j + l < WIDTH)
								if (round(heightsMap[i * WIDTH + j] * RoundCooeficient) / RoundCooeficient <= heightsMap[i * WIDTH + j]
									&& round(heightsMap[i * WIDTH + j] * RoundCooeficient) / RoundCooeficient >= heightsMap[(i + k) * WIDTH + j + l])
									colorMap[i][j] = Color::Black;
			}
			else if (mode == TopographicContinent)
				if (heightsMap[i * WIDTH + j] < SEA_LEVEL) 
					colorMap[i][j] = SeaColor(heightsMap[i * WIDTH + j], sea);
				else if (heightsMap[i * WIDTH + j] < SEA_LEVEL + 0.15f)
					colorMap[i][j] = plain;
				else if (heightsMap[i * WIDTH + j] < SEA_LEVEL + 0.25f)
					colorMap[i][j] = elevation;
				else if (heightsMap[i * WIDTH + j] < SEA_LEVEL + 0.35f)
					colorMap[i][j] = hill;
				else
					colorMap[i][j] = mountain;
				
		}

	DrawRivers(colorMap, heightsMap);

	return colorMap;
}

float* GetGradientOfThickness(float* heights, Coordinate firstDot, Coordinate secondDot) {
	Vectorfloat direction = GetDirection(firstDot, secondDot);
	float* gradient = new float[int(GetDistance(firstDot, secondDot))];
	Coordinate currentDot = { -1, -1 };
	float distance = GetDistance(firstDot, secondDot);
	for (int i = 0; i < distance; i++) {
		currentDot.x = firstDot.x + direction.x * i;
		currentDot.y = firstDot.y + direction.y * i;
		gradient[i] = (1.f - heights[currentDot.y * WIDTH + currentDot.x]) * 9.f + 0.5f;
	}
	return gradient;
}

void DrawRivers(Color** colorMap, float* heights) {
	for (int i = 0; i < 100; i++) {
		int randomI = rand() % HEIGHT;
		int randomJ = rand() % WIDTH;
		if (heights[randomI * WIDTH + randomJ] > (SEA_LEVEL + 0.2f))
			DrawRiver(colorMap, heights, { randomJ, randomI });
	}
	DrawRiver(colorMap, heights, SearchHighestCoordinate(heights));
}

void DrawRiver(Color** colorMap, float* heights, Coordinate StartDot) {
	list<Coordinate> dotsOfRiver;

	dotsOfRiver = GetListOfRiversDots(StartDot, heights, 5.f);

	int itr = 1;
	for (auto currentDot = dotsOfRiver.begin(); currentDot != dotsOfRiver.end(); currentDot++) {
		itr++;
		list <Coordinate>::iterator nextDot = currentDot;
		++nextDot;
		float* gradientOfThikness = GetGradientOfThickness(heights, *nextDot, *currentDot);
		DrawLine(colorMap, *nextDot, *currentDot, gradientOfThikness);
		if (itr == dotsOfRiver.size())
			break;
	}

}

void DrawDot(Color** colorMap, Coordinate dot, float brushSize) {
	Color sea = Color(66, 170, 255);
	Color fiol = Color(255, 0, 255);
	for (int i = -brushSize / 2; i <= brushSize / 2; i++)
		for (int j = -brushSize / 2; j <= brushSize / 2; j++)
			if (dot.y + i >= 0 && dot.x + j >= 0 && dot.y + i < HEIGHT && dot.x + j < WIDTH) {
				Coordinate dinamDot = { dot.x + j, dot.y + i };
				float distance = GetDistance(dot, dinamDot);
				if (distance < brushSize / 2)
					colorMap[dot.y + i][dot.x + j] = SeaColor(SEA_LEVEL-0.01f, sea);
			}
}

void DrawLine(Color** colorMap, Coordinate firstDot, Coordinate secondDot, float* gradientOfThikness) {
	Vectorfloat direction = GetDirection(firstDot, secondDot);
	Coordinate currentDot = { 0, 0 };
	float distance = GetDistance(firstDot, secondDot);
	for (int i = 0; i < distance; i++) {
		currentDot.x = firstDot.x + direction.x * i;
		currentDot.y = firstDot.y + direction.y * i;
		DrawDot(colorMap, currentDot, fabs(gradientOfThikness[i]));
	}
}
