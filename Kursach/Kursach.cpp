#include <iostream>
#include <SFML/Graphics.hpp>
#include "ctime"
#include <cmath>
#include <stdlib.h>
#include <fstream>

using namespace std;
using namespace sf;

const int Height = 1024, Width = 1024;

Color** Colorize(float* masColor);
void NoiseToImage(Color** noise, string img);
void NoiseFromFile(float* noise, string fileName);


int main()
{

	double zoom = 1.f;
	Vector2f playercoord;
	playercoord = Vector2f(1 / 2, 1 / 2);
	RenderWindow window(VideoMode(Width, Height), "Random generation"); 
	sf::Event win_event;

	float speed = 0.4f;

	setlocale(LC_ALL, "RUS");
	srand(time(0));

	float* perlineNoise = new float [Height* Width];

	NoiseFromFile(perlineNoise, "perlineNoise.bin");

	Color** map = Colorize(perlineNoise);
	NoiseToImage(map, "map.png");		//эти 5 строк

	window.clear(Color(255, 255, 255));

	sf::Texture tex;
	tex.create(Width, Height);
	sf::Texture tex1;
	tex1.create(Width, Height);
	tex.loadFromFile("map.png");
	sf::Sprite spr(tex1);
	sf::Texture tex2;
	tex2.create(Width, Height);
	tex2.loadFromFile("map2.png");

	Shader shader;
	shader.loadFromFile("noise.frag", sf::Shader::Fragment);

	shader.setUniform("colormap", tex);
	shader.setUniform("colormap2", tex2);

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
		shader.setUniform("time", clk.getElapsedTime().asMilliseconds() / 1000.f);
		shader.setUniform("resolution", Vector2f(Width, Height));
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
		shader.setUniform("zoom", float(zoom));

		shader.setUniform("playercoord", playercoord);

		window.draw(spr, &shader);

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
		cout << "В функцию передан пустой указатель";
		exit(-1);
	}		
	ifstream file(fileName, ios::binary);
	vector<float> v(file.seekg(0, ios::end).tellg());
	file.seekg(0, ios::beg);
	file.read((char*)v.data(), v.size());

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

Color** Colorize(float** heightsMap) {
	Color grass, sea, sand, snow;
	snow = Color(230, 230, 255);
	grass = Color(68, 148, 74);
	sea = Color(66, 170, 255);
	sand = Color(255, 204, 51);

	Color** colorMap = new Color* [Height];
	for (int i = 0; i < Height; i++)
		colorMap[i] = new Color[Width];

	for (int i = 0; i < Height / 2; i++)
		for (int j = 0; j < Width; j++) {
			int t = heightsMap[i][j] * i / Height / 2.05;
		}

	for (int i = 0; i < Height; i++)
		for (int j = 0; j < Width; j++) {
			/*if (heightsMap[i][j] < 0.5f) 
				colorMap[i][j] = sea;
			else if (heightsMap[i][j] < 0.53f)
				colorMap[i][j] = sand;
			else if (heightsMap[i][j] < 0.97f)
				colorMap[i][j] = grass;
			else
				colorMap[i][j] = snow;*/
			int height = heightsMap[i][j] * 255;
			colorMap[i][j] = Color(height, height, height);
		}
	return colorMap;
}