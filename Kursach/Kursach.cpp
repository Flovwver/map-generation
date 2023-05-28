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

const int HEIGHT = 1024, WIDTH = 1024;
const int HEIGHT_WINDOW = HEIGHT, WIDTH_WINDOW = WIDTH + 300;
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

void GenerateHeightsMap(float* perlineNoise, Color** map, Texture& tex, int seed);
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

extern int GenerateAndSavePerlineNoise(int height, int width, int seed);

int main()
{

	double zoom = 1.f;
	RenderWindow window(VideoMode(WIDTH_WINDOW, HEIGHT_WINDOW), "Random generation");
	Event win_event;

	float speed = 0.4f;

	setlocale(LC_ALL, "RUS");
	srand(time(0));

	float* perlineNoise = new float [HEIGHT * WIDTH];

	Color** map = new Color*[HEIGHT];

	window.clear(Color(255, 255, 255));

	Texture tex;
	tex.create(WIDTH, HEIGHT);
	Sprite spr(tex);

	GenerateHeightsMap(perlineNoise, map, tex, time(0) % 10000);

	bool mapIsChange = false;

	Font font;
	if (!font.loadFromFile("arial.ttf"))
		return EXIT_FAILURE;

	Text newHeightsText;
	newHeightsText.setFont(font);
	newHeightsText.setStyle(Text::Bold);
	newHeightsText.setString("Create new height's map with random seed");
	newHeightsText.setFillColor(Color::Black);
	newHeightsText.setCharacterSize(20);
	newHeightsText.setPosition(WIDTH_WINDOW - 280, HEIGHT_WINDOW / 4);

	RectangleShape rectangleNewHeights(Vector2f(250, 50));
	rectangleNewHeights.setPosition(WIDTH_WINDOW - 280, HEIGHT_WINDOW / 4);
	rectangleNewHeights.setFillColor(Color(100, 250, 50));

	Text inputSeedText;
	inputSeedText.setFont(font);
	inputSeedText.setStyle(Text::Bold);
	inputSeedText.setString("Create new height's map with input seed");
	inputSeedText.setFillColor(Color::Black);
	inputSeedText.setCharacterSize(20);
	inputSeedText.setPosition(WIDTH_WINDOW - 280, 3 * HEIGHT_WINDOW / 4);

	RectangleShape rectangleInputSeed(Vector2f(250, 50));
	rectangleInputSeed.setPosition(WIDTH_WINDOW - 280, 3 * HEIGHT_WINDOW / 4);
	rectangleInputSeed.setFillColor(Color(100, 250, 50));

	while (window.isOpen())
	{
		Event Event;
		while (window.pollEvent(Event))
		{
			switch (Event.type)
			{
			case sf::Event::Closed:
				window.close();
				break;
			case sf::Event::MouseMoved:
			{
				sf::Vector2i mousePos = sf::Mouse::getPosition(window);
				sf::Vector2f mousePosF(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));
				if (rectangleNewHeights.getGlobalBounds().contains(mousePosF))
				{
					rectangleNewHeights.setFillColor(sf::Color(250, 20, 20));
				}
				else
				{
					rectangleNewHeights.setFillColor(sf::Color(100, 250, 50));
				}
				if (rectangleInputSeed.getGlobalBounds().contains(mousePosF))
				{
					rectangleInputSeed.setFillColor(sf::Color(250, 20, 20));
				}
				else
				{
					rectangleInputSeed.setFillColor(sf::Color(100, 250, 50));
				}
			}
			break;
			case sf::Event::MouseButtonPressed:
			{
				sf::Vector2i mousePos = sf::Mouse::getPosition(window);
				sf::Vector2f mousePosF(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));
				if (rectangleNewHeights.getGlobalBounds().contains(mousePosF))
				{
					GenerateHeightsMap(perlineNoise, map, tex, time(0) % 10000);
				}
				if (rectangleInputSeed.getGlobalBounds().contains(mousePosF))
				{
					cout << "Input seed: ";
					int seed = 5;
					cin >> seed;
					GenerateHeightsMap(perlineNoise, map, tex, seed);
				}
			}
			break;
			}
		}

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
		{
			window.close();
		}

		window.clear(Color(255, 255, 255));

		window.draw(rectangleNewHeights);
		window.draw(newHeightsText);
		window.draw(rectangleInputSeed);
		window.draw(inputSeedText);
		window.draw(spr);

		window.display();
	}

	return 0;
}

void GenerateHeightsMap(float* perlineNoise, Color** map , Texture& tex, int seed) {

	cout << "Generating Height's Map on seed: " << seed << "..." << endl;
	GenerateAndSavePerlineNoise(HEIGHT, WIDTH, seed);
	perlineNoise = new float[HEIGHT * WIDTH];
	cout << "load from file... " << endl;
	NoiseFromFile(perlineNoise, "perlineNoise.txt");
	cout << "successful" << endl;
	cout << "Increasing contrast... " << endl;
	IncreaseContrast(perlineNoise);
	cout << "successful" << endl;
	cout << "colorizing..." << endl;
	map = Colorize(perlineNoise);
	cout << "successful" << endl;

	cout << "load to image..." << endl;
	NoiseToImage(map, "map.png");
	tex.loadFromFile("map.png");
	cout << "successful" << endl;
}

void NoiseToImage(Color** noise, string img) {
	Image image;
	image.create(WIDTH, HEIGHT, Color::Black);
	for (int i = 0; i < HEIGHT; i++)
		for (int j = 0; j < WIDTH; j++)
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

	for (int i = 0; i < HEIGHT*WIDTH; i++) {
		string temp;
		file >> temp;
		
		noise[i] = stod(temp);
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

Color** Colorize(float* heightsMap) {
	const float SEA_LEVEL = 0.6f, SAND_LEVEL = 0.62f, GRASS_LEVEL = 0.97f;
	Color grass, sea, sand, snow;
	snow = Color(230, 230, 255);
	grass = Color(68, 148, 74);
	sea = Color(66, 170, 255);
	sand = Color(255, 204, 51);

	Color** colorMap = new Color* [HEIGHT];
	for (int i = 0; i < HEIGHT; i++)
		colorMap[i] = new Color[WIDTH];

	for (int i = 0; i < HEIGHT; i++)
		for (int j = 0; j < WIDTH; j++) {
			if (heightsMap[i * WIDTH + j] < SEA_LEVEL)
				colorMap[i][j] = sea;
			else if (heightsMap[i * WIDTH + j] < SAND_LEVEL)
				colorMap[i][j] = sand;
			else if (heightsMap[i * WIDTH + j] < GRASS_LEVEL)
				colorMap[i][j] = grass;
			else
				colorMap[i][j] = snow;
		}

	DrawRivers(colorMap, heightsMap);

	for (int i = 0; i < HEIGHT; i++)
		for (int j = 0; j < WIDTH; j++) {
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
	if (x == 0 || x == WIDTH - 1 || y == 0 || y == HEIGHT - 1)
		return false;

	for (int i = -1; i < 2; i++)
		for (int j = -1; j < 2; j++) {
			if (matrix[(y + i) * WIDTH + (x + j)] > matrix[y * WIDTH + x])
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
	for (int i = 0; i < HEIGHT; i++)
		for (int j = 0; j < WIDTH; j++) {
			if (heights[i * WIDTH + j] > heights[highestCoordinate.y * WIDTH + highestCoordinate.x]) {
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
		gradient[i] = (1.f - heights[currentDot.y * WIDTH + currentDot.x]) * 9.f  + 0.5f;
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
	float maximumCosineOfRiverTurn = 0.9f;

	for (int k = 0; k < 10000; k++) {
		float max = -1,
			phi;
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

		if (heights[dotOfFastestDescent.y * WIDTH + dotOfFastestDescent.x] < SEA_LEVEL)
			break;
	}
	return dotsOfRiver;
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
		if (dot.y + i >= 0 && dot.x + j >= 0 && dot.y + i < HEIGHT && dot.x + j < WIDTH){
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

