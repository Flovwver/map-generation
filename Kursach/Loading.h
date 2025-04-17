#pragma once

#define _USE_MATH_DEFINES 

#include "Constants.h"
#include "Colorizing.h"
#include <iostream>
#include <fstream>
#include <SFML/Graphics.hpp>
#include <SFML/System/Vector2.hpp>

using namespace std;
using namespace sf;

void NoiseToImage(Color** noise, string img);
void NoiseFromFile(float* noise, string fileName);
void OutputArray(float* arrayOfElements, int firsOtputElement, int lastOtputElement);
void OutputArray(Color** arrayOfElements, int countOfOtputElements);
void NoiseToImage(float* noise, string img);

extern int GenerateAndSavePerlineNoise(int height, int width, int seed);

void GenerateHeightsMap(float* perlineNoise, Color** map, Texture& texMap, Texture& texHeights, int seed, MapDisplayMode mode) {

	cout << "Generating Height's Map on seed: " << seed << "..." << endl;
	GenerateAndSavePerlineNoise(HEIGHT, WIDTH, seed);
	perlineNoise = new float[HEIGHT * WIDTH];
	cout << "load from file... " << endl;
	NoiseFromFile(perlineNoise, "perlineNoise.txt");
	cout << "successful" << endl;
	cout << "Increasing contrast... " << endl;
	IncreaseContrast(perlineNoise);
	cout << "successful" << endl;
	cout << "Conning contrast... " << endl;
	Cone(perlineNoise);
	cout << "successful" << endl;
	cout << "Increasing contrast... " << endl;
	IncreaseContrast(perlineNoise);
	cout << "successful" << endl;
	cout << "colorizing..." << endl;
	map = Colorize(perlineNoise, mode);
	cout << "successful" << endl;

	cout << "load to image..." << endl;
	NoiseToImage(map, "map.png");
	texMap.loadFromFile("map.png");
	cout << "successful" << endl;

	cout << "load heights to image..." << endl;
	NoiseToImage(perlineNoise, "perlineNoise.png");
	texHeights.loadFromFile("perlineNoise.png");
	cout << "successful" << endl;
}

void NoiseToImage(Color** noise, string img) {
	Image image({ WIDTH, HEIGHT }, Color::Black);
	for (int i = 0; i < HEIGHT; i++)
		for (int j = 0; j < WIDTH; j++)
			image.setPixel(Vector2u(i, j), noise[i][j]);
	image.saveToFile(img);
}

void NoiseToImage(float* noise, string img) {
	Image image({ WIDTH, HEIGHT }, Color::Black);
	for (int i = 0; i < HEIGHT; i++)
		for (int j = 0; j < WIDTH; j++)
			image.setPixel(Vector2u(i, j), Color(int(noise[i * WIDTH + j] * 255)));
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

	for (int i = 0; i < HEIGHT * WIDTH; i++) {
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