#define _USE_MATH_DEFINES 

#include <iostream>
#include <SFML/Graphics.hpp>
#include "ctime"
#include <cmath>
#include <stdlib.h>
#include <fstream>
#include <list>

using namespace std;
using namespace sf;

const int Height = 1024, Width = 1024;
const float SEA_LEVEL = 0.6f, SAND_LEVEL = 0.62f, GRASS_LEVEL = 0.97f;

struct Coordinate
{
	int x;
	int y;
};
struct Vectorfloat
{
	float x;
	float y;
};

Color** Colorize(float* masColor);
void NoiseToImage(Color** noise, string img);
void NoiseFromFile(float* noise, string fileName);
void OutputArray(float* arrayOfElements, int firsOtputElement, int lastOtputElement);
void OutputArray(Color** arrayOfElements, int countOfOtputElements);
bool IsLocalMax(float* matrix, int x, int y);
void DrawRivers(Color** colorMap, float* heights);
list <Coordinate>::iterator SearchNearestCoordinate(list<Coordinate> &coordinates, Coordinate currentCoordinate);
void DrawLine(Color** colorMap, Coordinate firstDot, Coordinate secondDot, float* gradientOfThikness);
void DrawDot(Color** colorMap, Coordinate dot, float brushSize);
void IncreaseContrast(float* perlineNoise);


int main()
{

	double zoom = 1.f;
	RenderWindow window(VideoMode(Width, Height), "Random generation"); 
	Event win_event;

	float speed = 0.4f;

	setlocale(LC_ALL, "RUS");
	srand(time(0));

	float* perlineNoise = new float [Height * Width];

	NoiseFromFile(perlineNoise, "perlineNoise.txt");

	IncreaseContrast(perlineNoise);

	Color** map = Colorize(perlineNoise);
	
	NoiseToImage(map, "map.png");

	window.clear(Color(255, 255, 255));

	sf::Texture tex;
	tex.create(Width, Height);
	tex.loadFromFile("map.png");
	sf::Sprite spr(tex);

	Clock clk;
	clk.restart(); // start the timer

	bool mapIsChange = false;

	while (window.isOpen())
	{
		Event event;
		while (window.pollEvent(event))
		{
			if (event.type == Event::Closed)
				window.close();
		}

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
		{
			window.close();
		}

		window.draw(spr);

		window.display();
	}

	return 0;
}

void NoiseToImage(Color** noise, string img) {
	Image image;
	image.create(Width, Height, Color::Black);
	for (int i = 0; i < Height; i++)
		for (int j = 0; j < Width; j++)
			image.setPixel(i, j, noise[i][j]);
	image.saveToFile(img);
}

void NoiseFromFile(float* noise, string fileName) {
	if (noise == NULL) {
		cout << "В функцию передан пустой указатель\n";
		exit(-1);
	}

	ifstream file(fileName);
	if (!file)
	{
		cout << "Error, Couldn't find the file\n";
		exit(-1);
	}

	for (int i = 0; i < Height*Width; i++) {
		file >> noise[i];
	}
	
}

void OutputArray(float* arrayOfElements, int firsOtputElement, int lastOtputElement) {
	for (int i = firsOtputElement; i <= lastOtputElement; i++) {
		cout << arrayOfElements[i] << "\t";
	}
	cout << "\n";
}

void OutputArray(Color** arrayOfElements, int countOfOtputElements) {
	for (int i = 0; i < countOfOtputElements; i++) {
		cout << int(arrayOfElements[0][i].r) << "\t";
	}
	cout << "\n";
}

void IncreaseContrast(float* perlineNoise) {
	float max = 0.f, min = 1.f;
	for (int i = 0; i < Height * Width; i++) {
		if (perlineNoise[i] > max)
			max = perlineNoise[i];
		if (perlineNoise[i] < min)
			min = perlineNoise[i];
	}

	for (int i = 0; i < Height * Width; i++) {
		perlineNoise[i] = (perlineNoise[i] - min) / (max - min);
	}

}

float GetShadow(float* heightsMap, int x, int y) {
	float shadow = 0;
	int shadowLength = 8;
	for (int i = 1; i <= shadowLength; i++) {
		if (y - i >= 0 && x - i >= 0) {
			if (heightsMap[(y - i) * Width + (x - i)] >= heightsMap[y * Width + x] && heightsMap[(y - i) * Width + (x - i)] >= SEA_LEVEL)
				shadow += heightsMap[(y - i) * Width + (x - i)] - heightsMap[y * Width + x];
		}
	}
	return pow(1.f - shadow, 2);
}

Color makeDarker(Color color, float dark) {
	return Color(color.r * dark, color.g * dark, color.b * dark);
}

Color** Colorize(float* heightsMap) {
	Color grass, sea, sand, snow;
	snow = Color(230, 230, 255);
	grass = Color(68, 148, 74);
	sea = Color(66, 170, 255);
	sand = Color(255, 204, 51);

	Color** colorMap = new Color* [Height];
	for (int i = 0; i < Height; i++)
		colorMap[i] = new Color[Width];

	for (int i = 0; i < Height; i++)
		for (int j = 0; j < Width; j++) {
			if (heightsMap[i * Width + j] < SEA_LEVEL)
				colorMap[i][j] = sea;
			else if (heightsMap[i * Width + j] < SAND_LEVEL)
				colorMap[i][j] = sand;
			else if (heightsMap[i * Width + j] < GRASS_LEVEL)
				colorMap[i][j] = grass;
			else
				colorMap[i][j] = snow;

			/*int height = fabs(heightsMap[i * Width + j]) * 254;
			colorMap[i][j] = Color(height, height, height, 255);*/
			
		}

	DrawRivers(colorMap, heightsMap);

	for (int i = 0; i < Height; i++)
		for (int j = 0; j < Width; j++) {
			colorMap[i][j] = makeDarker(colorMap[i][j], GetShadow(heightsMap, j, i));
		}

	return colorMap;
}


float GetDistance(Coordinate dot1, Coordinate dot2) {
	return pow(pow(dot1.x - dot2.x, 2) + pow(dot1.y - dot2.y, 2), 0.5f);
}

float GetLength(Vectorfloat vec) {
	return pow(pow(vec.x, 2) + pow(vec.y, 2), 0.5f);
}

Vectorfloat Normalize(Vectorfloat vec) {
	Vectorfloat vecF = vec;
	float length = GetLength(vec);
	vecF.x /= length;
	vecF.y /= length;
	return vecF;
}

bool IsLocalMax(float* matrix, int x, int y) {
	if (x == 0 || x == Width - 1 || y == 0 || y == Height - 1)
		return false;

	for (int i = -1; i < 2; i++)
		for (int j = -1; j < 2; j++) {
			if (matrix[(y + i) * Width + (x + j)] > matrix[y * Width + x])
				return false;
		}
	return true;
}

Vectorfloat GetDirection(Coordinate firstDot, Coordinate secondDot) {
	Vectorfloat direction = { secondDot.x - firstDot.x, secondDot.y - firstDot.y };
	direction = Normalize(direction);
	return direction;
}

float GetScalarProduct(Vectorfloat vec1, Vectorfloat vec2) {
	return vec1.x * vec2.x + vec1.y * vec2.y;
}

Coordinate SearchHighestCoordinate(float* heights) {
	Coordinate highestCoordinate = { 0, 0 };
	for (int i = 0; i < Height; i++)
		for (int j = 0; j < Width; j++) {
			if (heights[i * Width + j] > heights[highestCoordinate.y * Width + highestCoordinate.x]) {
				highestCoordinate.y = i;
				highestCoordinate.x = j;
			}
		}
	return highestCoordinate;
}

Vectorfloat Rotate(Vectorfloat vec, float phi) {
	float cs = cos(phi);
	float sn = sin(phi);
	float x = vec.x;
	float y = vec.y;

	vec.x = x * cs - y * sn;
	vec.y = x * sn + y * cs;
	return vec;
}

float* GetGradientOfThickness(float* heights, Coordinate firstDot, Coordinate secondDot) {
	Vectorfloat direction = GetDirection(firstDot, secondDot);
	float* gradient = new float[int(GetDistance(firstDot, secondDot))];
	Coordinate currentDot = { -1, -1 };
	for (int i = 0; i < int(GetDistance(firstDot, secondDot)); i++) {
		currentDot.x = firstDot.x + direction.x * i;
		currentDot.y = firstDot.y + direction.y * i;
		gradient[i] = (1.f - heights[currentDot.y * Width + currentDot.x]) * 9.f  + 0.5f;
	}
	return gradient;
}

list<Coordinate> GetListOfRiversDots(Coordinate dotOfRiversStart, float* heights, float lengthOfRiversDetalization) {
	list<Coordinate> dotsOfRiver;
	dotsOfRiver.push_back(dotOfRiversStart);

	auto dot = dotsOfRiver.begin();
	Coordinate dotOfFastestDescent = { -1, -1 };
	Vectorfloat directionRiverFirst = { 0.f, 1.f }, directionRiverSecond = { 0.f, -1.f }, directionRiverSecondSaved = { 0.f, -1.f };

	int countOfRiverTurns = 32;
	float angleOfRiverTurn = 2 * M_PI / countOfRiverTurns;
	float maximumCosineOfRiverTurn = 0.5f;

	for (int k = 0; k < 100; k++) {
		float max = -1,
			phi;
		for (int i = 0; i < countOfRiverTurns; i++) {
			phi = angleOfRiverTurn * i;
			directionRiverSecond = Rotate(directionRiverSecond, phi);

			if (GetScalarProduct(directionRiverFirst, directionRiverSecond) < maximumCosineOfRiverTurn)
				continue;

			int xShtrih = ((*dot).x + directionRiverSecond.x * lengthOfRiversDetalization),
				yShtrih = ((*dot).y + directionRiverSecond.y * lengthOfRiversDetalization);

			if (yShtrih < Height && xShtrih < Width && yShtrih >= 0 && xShtrih >= 0) {
				float amountOfDescent = heights[(*dot).y * Width + (*dot).x] - heights[yShtrih * Width + xShtrih];
				if (amountOfDescent > max) {
					max = heights[(*dot).y * Width + (*dot).x] - heights[yShtrih * Width + xShtrih];
					dotOfFastestDescent = { xShtrih, yShtrih };
					directionRiverSecondSaved = directionRiverSecond;
				}
			}

		}
		directionRiverFirst = directionRiverSecondSaved;

		dotsOfRiver.push_back(dotOfFastestDescent);
		++dot;

		if (heights[dotOfFastestDescent.y * Width + dotOfFastestDescent.x] < SEA_LEVEL)
			break;
	}
	return dotsOfRiver;
}

void DrawRivers(Color** colorMap, float* heights) {
	list<Coordinate> dotsOfRiver;

	dotsOfRiver = GetListOfRiversDots(SearchHighestCoordinate(heights), heights, 5.f);//{600,512}

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
		if (dot.y + i >= 0 && dot.x + j >= 0 && dot.y + i < Height && dot.x + j < Width){
			Coordinate dinamDot = { dot.x + j, dot.y + i };
			float distance = GetDistance(dot, dinamDot);
			if (distance < brushSize / 2)
				colorMap[dot.y + i][dot.x + j] = sea;
		}
}

void DrawLine(Color** colorMap, Coordinate firstDot, Coordinate secondDot, float* gradientOfThikness) {
	Vectorfloat direction = GetDirection(firstDot, secondDot);
	Coordinate currentDot = { 0, 0 };

	for (int i = 0; i < GetDistance(firstDot, secondDot); i++) {
		currentDot.x = firstDot.x + direction.x * i;
		currentDot.y = firstDot.y + direction.y * i;
		DrawDot(colorMap, currentDot, gradientOfThikness[i]);
	}
}

