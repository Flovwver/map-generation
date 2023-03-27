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

	Color** map = Colorize(perlineNoise);
	
	//DrawDot(map, { 250,500 }, 10);
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

int ikjl(int i, int k, int tyk, int yx) {
	int ik, jl;
	if (i / tyk * tyk + k * tyk < 0)
		ik = Height / tyk * tyk + i / tyk * tyk + k * tyk;
	else if (i / tyk * tyk + k * tyk >= Height)
		ik = (i / tyk * tyk + k * tyk) % Height;
	else
		ik = i / tyk * tyk + k * tyk;
	if (i / tyk * tyk + k * tyk < 0)
		jl = Width / tyk * tyk + i / tyk * tyk + k * tyk;
	else if (i / tyk * tyk + k * tyk >= Width)
		jl = (i / tyk * tyk + k * tyk) % Width;
	else
		jl = i / tyk * tyk + k * tyk;
	if (!yx)
		return ik;
	else
		return jl;
}

int* ikjl2(int i, int k, int j, int l, int tyk) {
	int* xy = new int[2];

	int ik, jl;
	if (i / tyk * tyk + k * tyk < 0)
		ik = Height / tyk * tyk + i / tyk * tyk + k * tyk;
	else if (i / tyk * tyk + k * tyk >= Height)
		ik = (i / tyk * tyk + k * tyk) % Height;
	else
		ik = i / tyk * tyk + k * tyk;

	if (j / tyk * tyk + l * tyk < 0) {
		ik = Height / tyk * tyk - ik - tyk;
		jl = Width / tyk * tyk + j / tyk * tyk + l * tyk;
	}
	else if (j / tyk * tyk + l * tyk >= Width) {
		if (tyk == 8 && i == 504 && j == 1) {
			int flower = j / tyk * tyk + l * tyk;
			flower = 0;
		}
		ik = Height / tyk * tyk - ik - tyk;
		ik = ikjl(ik, 1, tyk, 0);
		jl = (j / tyk * tyk + l * tyk) % Width;
	}
	else
		jl = j / tyk * tyk + l * tyk;

	xy[0] = jl;
	xy[1] = ik;
	return xy;
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
			if (heightsMap[i * Width + j] < 0.66f)
				colorMap[i][j] = sea;
			else if (heightsMap[i * Width + j] < 0.69f)
				colorMap[i][j] = sand;
			else if (heightsMap[i * Width + j] < 0.97f)
				colorMap[i][j] = grass;
			else
				colorMap[i][j] = snow;
			/*int height = fabs(heightsMap[i * Width + j]) * 254;
			colorMap[i][j] = Color(height, height, height, 255);*/
			
		}
	DrawRivers(colorMap, heightsMap);
	return colorMap;
}


float Distance(Coordinate dot1, Coordinate dot2) {
	return pow(pow(dot1.x - dot2.x, 2) + pow(dot1.y - dot2.y, 2), 0.5f);
}

float Length(Vectorfloat vec) {
	return pow(pow(vec.x, 2) + pow(vec.y, 2), 0.5f);
}

Vectorfloat Normalize(Vectorfloat vec) {
	Vectorfloat vecF = vec;
	float length = Length(vec);
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

Vectorfloat Direction(Coordinate firstDot, Coordinate secondDot) {
	Vectorfloat direction = { secondDot.x - firstDot.x, secondDot.y - firstDot.y };
	direction = Normalize(direction);
	return direction;
}

float* GetGradientOfThickness(float* heights, Coordinate firstDot, Coordinate secondDot) {
	Vectorfloat direction = Direction(firstDot, secondDot);
	float* gradient = new float[int(Distance(firstDot, secondDot))];
	Coordinate currentDot = { -1, -1 };
	for (int i = 0; i < int(Distance(firstDot, secondDot)); i++) {
		currentDot.x = firstDot.x + direction.x * i;
		currentDot.y = firstDot.y + direction.y * i;
		gradient[i] = (1 - heights[currentDot.y * Width + currentDot.x]) * 3 + 3;
	}
	return gradient;
}

void DrawRivers(Color** colorMap, float* heights) {
	list<Coordinate> coordsLocalMaxima;
	Coordinate coords = {0, 0};
	for (int i = 0; i < Height; i++)
		for (int j = 0; j < Width; j++) {
			if (IsLocalMax(heights, j, i)) {
				coords.y = i;
				coords.x = j;
				coordsLocalMaxima.push_back(coords);
			}
		}

	/*for (Coordinate localMaximum : coordsLocalMaxima) {
		DrawDot(colorMap, localMaximum, 10);
	}*/

	for (auto iter = coordsLocalMaxima.begin(); iter != coordsLocalMaxima.end(); iter++) {
		if (coordsLocalMaxima.size() < 3)
			break;
		list <Coordinate>::iterator nearestDot = SearchNearestCoordinate(coordsLocalMaxima, *iter);
		float* gradentOfThikness = GetGradientOfThickness(heights, *nearestDot, *iter);
		DrawLine(colorMap, *nearestDot, *iter, gradentOfThikness);
		if (iter != coordsLocalMaxima.begin()) coordsLocalMaxima.pop_front();
	}

}


list <Coordinate>::iterator SearchNearestCoordinate(list<Coordinate> &coordinates, Coordinate currentCoordinate) {
	float min = Height * Width;
	list <Coordinate>::iterator nearestCoordinate;
	for (list <Coordinate>::iterator iter = coordinates.begin(); iter != coordinates.end(); iter++) {
		float distance = Distance(*iter, currentCoordinate);
		if (distance < min && distance > 0.000001f) {
			min = distance;
			nearestCoordinate = iter;
		}
	}
	return nearestCoordinate;
}

void DrawDot(Color** colorMap, Coordinate dot, float brushSize) {
	Color sea = Color(66, 170, 255);
	for (int i = -brushSize / 2; i <= brushSize / 2; i++)
		for (int j = -brushSize / 2; j <= brushSize / 2; j++) 
		if (dot.y + i >= 0 && dot.x + j >= 0 && dot.y + i < Height && dot.x + j < Width){
			Coordinate dinamDot = { dot.x + j, dot.y + i };
			float distance = Distance(dot, dinamDot);
			if (distance < brushSize / 2)
				colorMap[dot.y + i][dot.x + j] = sea;
		}
}

void DrawLine(Color** colorMap, Coordinate firstDot, Coordinate secondDot, float* gradientOfThikness) {
	Vectorfloat direction = Direction(firstDot, secondDot);
	Coordinate currentDot = { 0, 0 };

	for (int i = 0; i < Distance(firstDot, secondDot); i++) {
		currentDot.x = firstDot.x + direction.x * i;
		currentDot.y = firstDot.y + direction.y * i;
		DrawDot(colorMap, currentDot, gradientOfThikness[i]);
	}
}

