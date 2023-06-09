#pragma once
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


void GenerateHeightsMap(float* perlineNoise, Color** map, Texture& tex, int seed);
bool IsLocalMax(float* matrix, int x, int y);










