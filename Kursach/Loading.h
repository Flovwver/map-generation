#pragma once

#define _USE_MATH_DEFINES 

#include "Constants.h"
#include "Colorizing.h"
#include <iostream>
#include <fstream>
#include <SFML/Graphics.hpp>

using namespace std;
using namespace sf;

void NoiseToImage(Color** noise, string img);
void NoiseFromFile(float* noise, string fileName);
void OutputArray(float* arrayOfElements, int firsOtputElement, int lastOtputElement);
void OutputArray(Color** arrayOfElements, int countOfOtputElements);

extern int GenerateAndSavePerlineNoise(int height, int width, int seed);

void GenerateHeightsMap(float* perlineNoise, Color** map, Texture& tex, int seed, MapDisplayMode mode) {

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
	map = Colorize(perlineNoise, mode);
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