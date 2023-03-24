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

Color** Colorize(float* masColor);
void NoiseToImage(Color** noise, string img);
void NoiseFromFile(float* noise, string fileName);
void OutputArray(float* arrayOfElements, int firsOtputElement, int lastOtputElement);
void OutputArray(Color** arrayOfElements, int countOfOtputElements);
bool IsLocalMax(float* matrix, int x, int y);


int main()
{

	double zoom = 1.f;
	Vector2f playercoord;
	playercoord = Vector2f(1.f / 2.f, 1.f / 2.f);
	RenderWindow window(VideoMode(Width, Height), "Random generation"); 
	sf::Event win_event;

	float speed = 0.4f;

	setlocale(LC_ALL, "RUS");
	srand(time(0));

	float* perlineNoise = new float [Height * Width];

	NoiseFromFile(perlineNoise, "perlineNoise.txt");

	float sum = 0;
	for (int i = 0; i < Height * Width; i++)
		sum += perlineNoise[i];
	cout << sum/(Height * Width) <<endl;

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

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
			playercoord.x += speed / Width;
			if (playercoord.x > 2.f) {
				playercoord.x = 0.f;
			}
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
			playercoord.x -= speed / Width;
			if (playercoord.x < 0.f) {
				playercoord.x = 2.f;
			}
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
			playercoord.y += speed / Width;
			if (playercoord.y > 1.f)
				playercoord.y = 0.f;

		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
			playercoord.y -= speed / Width;
			if (playercoord.y < 0.f)
				playercoord.y = 1.f;
		}
		while (window.pollEvent(win_event)) {
			if (win_event.type == sf::Event::KeyReleased) 
			{
				if (win_event.key.code == sf::Keyboard::Equal) 
					zoom *= 2.f;
				if (win_event.key.code == sf::Keyboard::Dash)
					zoom /= 2.f;
			}
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
		std::cout << "Error, Couldn't find the file\n";
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
			/*if (heightsMap[i * Width + j] < 0.66f)
				colorMap[i][j] = sea;
			else if (heightsMap[i * Width + j] < 0.69f)
				colorMap[i][j] = sand;
			else if (heightsMap[i * Width + j] < 0.97f)
				colorMap[i][j] = grass;
			else
				colorMap[i][j] = snow;*/
			int height = fabs(heightsMap[i * Width + j]) * 254;
			colorMap[i][j] = Color(height, height, height, 255);
			if (IsLocalMax(heightsMap, j, i))
				colorMap[i][j] = Color(255, 0, 255, 255);
		}
	return colorMap;
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

void DrawRivers(Color** colorMap, float* heights) {
	list<int[2]> coordsLocalMaxima;
	int coords[2] = {0, 0};
	for (int i = 0; i < Height; i++)
		for (int j = 0; j < Width; j++) {
			if (IsLocalMax(heights, j, i)) {
				coords[0] = i;
				coords[1] = j;
				coordsLocalMaxima.push_back(coords);
			}
		}
	

}

int* SearchNearestCoordinate(list<int[2]> coordinates, list<int[2]>::const_iterator coordinateForClosest) {
	for (int[2] coordinate : coordinates)
		std::cout << n << "\t";
}