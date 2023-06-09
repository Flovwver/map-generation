#pragma once

#define _USE_MATH_DEFINES 

#include <cmath>

using namespace std;

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

Vectorfloat GetDirection(Coordinate firstDot, Coordinate secondDot) {
	Vectorfloat direction = { secondDot.x - firstDot.x, secondDot.y - firstDot.y };
	direction = Normalize(direction);
	return direction;
}

float GetScalarProduct(Vectorfloat vec1, Vectorfloat vec2) {
	return vec1.x * vec2.x + vec1.y * vec2.y;
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

float Modu(float a, float b) {
	int intaonb = a / b;
	return a - b * intaonb;
}
