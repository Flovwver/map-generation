#include <iostream>
#include <SFML/Graphics.hpp>
#include "ctime"
#include <cmath>

using namespace std;
using namespace sf;

int height = 1024, width = 1024;

Color** noise(int layer);
Color** meganoise(int n, bool kuchnost);
Color** bicubicinterpolation(Color** masColor, int tyk);
Color** bilinearinterpol(Color** masColor, int tyk);
Color** coloring(Color** masColor, Color** masColor2);
Color** coloring(Color** masColor);
void noise_to_img(Color** noise, string img);
int** deleteEl(int** mas, int oldN, int elem);


int main()
{

	double zoom = 1.f;
	Vector2f playercoord;
	playercoord = Vector2f(1 / 2, 1 / 2);

	/*std::vector <VideoMode> hw = VideoMode::getFullscreenModes();
	height = hw[0].height;
	width = hw[0].width;*/												//Чтобы перейти в полноэкранный режим нужно раскомментировать это и
	RenderWindow window(VideoMode(width, height), "Random generation"/*, sf::Style::Fullscreen*/); //и это
	sf::Event win_event;

	float speed = 0.4f;

	setlocale(LC_ALL, "RUS");
	srand(time(0));

	/*Color** masPix = noise(16);
	masPix = bicubicinterpolation(masPix, 64);
	noise_to_img(masPix, "debug image.png");*/


	//masPix = meganoise(10, true);				//Чтобы сгенерировать новый шум 
	//Color** masPix2 = /*mega*/noise(10/*, false*/);   //mapfortests.png

	Image mapForTest;
	if (!mapForTest.loadFromFile("mapfortests.png"))
		exit(93089706);
	//mapForTest.setPixel(256, 256, Color(152, rand() % 256, rand() % 256));

	Color** masPix = noise(4);

	for (int i = 0; i < height; i++)
		for (int j = 0; j < width; j++) {
			masPix[i][j] = mapForTest.getPixel(i, j);
		}
	cout << int(masPix[256][256].r) << endl;
	//masPix = meganoise(10, true);			//Чтобы сгенерировать новый шум 
	//Color** masPix2 = meganoise(10, true);
	noise_to_img(masPix, "map2.png");
	
	for (int i = 0; i < 35; i++) {
		for (int j = 0; j < 35; j++) {
			cout << int(masPix[i + 254][j + 254].r) << "\t";
		}
		cout << endl;
	}
	masPix = coloring(masPix/*, masPix2*/);		//нужно раскомментировать 
	noise_to_img(masPix, "map.png");		//эти 5 строк

	window.clear(Color(255, 255, 255));

	sf::Texture tex;
	tex.create(width, height);
	sf::Texture tex1;
	tex1.create(width, height);
	tex.loadFromFile("map.png");
	sf::Sprite spr(tex1);
	sf::Texture tex2;
	tex2.create(width, height);
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
		shader.setUniform("resolution", Vector2f(width, height));
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
		{
			window.close();
		}

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
			playercoord.x += speed / width;
			if (playercoord.x > 2.f) {
				playercoord.x = 0.f;
			}
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
			playercoord.x -= speed / width;
			if (playercoord.x < 0.f) {
				playercoord.x = 2.f;
			}
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
			playercoord.y += speed / width;
			if (playercoord.y > 1.f)
				playercoord.y = 0.f;

		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
			playercoord.y -= speed / width;
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

void noise_to_img(Color** noise, string img) {
	Image image;
	image.create(width, height, Color::Black);
	for (int i = 0; i < height; i++)
		for (int j = 0; j < width; j++)
			image.setPixel(i, j, noise[i][j]);
	image.saveToFile(img);
}

Color** noise(int layer) {
	Color** masColor = new Color * [height];
	for (int i = 0; i < height; i++)
		masColor[i] = new Color[width];
	int limit = layer * layer * layer;
	for (int k = 0; k < limit; k++) {
		if (k == 5) {
			int flower = 5;
		}
		int color = rand() % 255;
		for (int i = int(float(height) / (layer * height / width) * (k % (layer * height / width))); i < int(float(height) / (layer * height / width) * (k % (layer * height / width) + 1)); i++) {
			if (i >= height)
				break;
			for (int j = width / layer * (k / (layer * height / width)); j < width / layer * (k / (layer * height / width) + 1); j++) {
				if (j >= width)
					break;
				masColor[i][j] = Color(color, color, color);
			}
		}
	}
	return masColor;
}

Color** meganoise(int n, bool kuchnost) {
	int*** t = new int** [height];
	for (int i = 0; i < height; i++) {
		t[i] = new int* [width];
		for (int j = 0; j < width; j++) {
			t[i][j] = new int[3];
			t[i][j][0] = 0;
			t[i][j][1] = 0;
			t[i][j][2] = 0;
		}
	}
	Color*** masColor = new Color * *[n];
	for (int i = 0; i < n; i++) {
		masColor[i] = new Color * [height];
		for (int j = 0; j < height; j++)
			masColor[i][j] = new Color[width];
	}
	for (int k = 1; k < n - 2; k++) {
		masColor[k] = noise(pow(2, k + 1));
		String name;
		name = "octave" + to_string(k + 1) + ".png";
		noise_to_img(masColor[k], name);
		//masColor[k] = bilinearinterpol(masColor[k], (height) / (pow(2, k + 1)));
		masColor[k] = bicubicinterpolation(masColor[k], (height) / (pow(2, k + 1)));
		name = "octave_blured" + to_string(k + 1) + ".png";
		noise_to_img(masColor[k], name);
	}
	for (int k = 1; k < n - 2; k++) {
		for (int i = 0; i < height; i++)
			for (int j = 0; j < width; j++) {
				float temp = 1;
				if (kuchnost)
					temp = k;
				t[i][j][0] += int(masColor[k][i][j].r) / temp;
				t[i][j][1] += int(masColor[k][i][j].g) / temp;
				t[i][j][2] += int(masColor[k][i][j].b) / temp;
			}
	}
	for (int i = 0; i < height; i++)
		for (int j = 0; j < width; j++) {

			masColor[n - 1][i][j] = Color(int(t[i][j][0] / (n - 1)), int(t[i][j][1] / (n - 1)), int(t[i][j][2] / (n - 1)));
		}

	int min = int(masColor[n - 1][0][0].r), max = int(masColor[n - 1][0][0].r);
	for (int i = 0; i < height; i++)
		for (int j = 0; j < width; j++) {
			if (int(masColor[n - 1][i][j].r) < min)
				min = int(masColor[n - 1][i][j].r);
			if (int(masColor[n - 1][i][j].r) > max)
				max = int(masColor[n - 1][i][j].r);
		}

	for (int i = 0; i < height; i++)
		for (int j = 0; j < width; j++) {
			masColor[n - 1][i][j].r = int(((int(masColor[n - 1][i][j].r) - min) / double(max - min)) * 255);
			masColor[n - 1][i][j].g = int(((int(masColor[n - 1][i][j].g) - min) / double(max - min)) * 255);
			masColor[n - 1][i][j].b = int(((int(masColor[n - 1][i][j].b) - min) / double(max - min)) * 255);
		}

	return masColor[n - 1];
}

int ikjl(int i, int k, int tyk, int yx) {
	int ik, jl;
	if (i / tyk * tyk + k * tyk < 0)
		ik = height / tyk * tyk + i / tyk * tyk + k * tyk;
	else if (i / tyk * tyk + k * tyk >= height)
		ik = (i / tyk * tyk + k * tyk) % height;
	else
		ik = i / tyk * tyk + k * tyk;
	if (i / tyk * tyk + k * tyk < 0)
		jl = width / tyk * tyk + i / tyk * tyk + k * tyk;
	else if (i / tyk * tyk + k * tyk >= width)
		jl = (i / tyk * tyk + k * tyk) % width;
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
		ik = height / tyk * tyk + i / tyk * tyk + k * tyk;
	else if (i / tyk * tyk + k * tyk >= height)
		ik = (i / tyk * tyk + k * tyk) % height;
	else
		ik = i / tyk * tyk + k * tyk;

	if (j / tyk * tyk + l * tyk < 0) {
		ik = height / tyk * tyk - ik - tyk;
		jl = width / tyk * tyk + j / tyk * tyk + l * tyk;
	}
	else if (j / tyk * tyk + l * tyk >= width) {
		if (tyk == 8 && i == 504 && j == 1) {
			int flower = j / tyk * tyk + l * tyk;
			flower = 0;
		}
		ik = height / tyk * tyk - ik - tyk;
		ik = ikjl(ik, 1, tyk, 0);
		jl = (j / tyk * tyk + l * tyk) % width;
	}
	else
		jl = j / tyk * tyk + l * tyk;

	xy[0] = jl;
	xy[1] = ik;
	return xy;
}

float CubicPolate(float v0, float v1, float v2, float v3, float fracy) {
	float A = 0.5f * (v3 - v0) + 1.5f * (v1 - v2);
	float B = 0.5f * (v0 + v2) - v1 - A;
	float C = 0.5f * (v2 - v0);
	float D = v1;

	return A * pow(fracy, 3) + B * pow(fracy, 2) + C * fracy + D;
}

Color** bicubicinterpolation(Color** masColor, int tyk) {
	float** ndata = new float* [4];
	for (int i = 0; i < 4; i++) {
		ndata[i] = new float[4];
	}

	/*for (int i = 0; i < float(height) / tyk; i++)
		for (int j = 0; j < float(width) / tyk; j++) {
			int color = rand() % 255;
			masColor[i * tyk][j * tyk] = Color(color, color, color);
		}*/

	for (int i = 0; i < height; i++)
		for (int j = 0; j < width; j++) {
			if (i % tyk != 0 || j % tyk != 0) {
				float t = 0;

				for (int X = 0; X < 4; X++)
					for (int Y = 0; Y < 4; Y++) {
						int* xy = ikjl2(i, Y - 1, j, X - 1, tyk);
						ndata[Y][X] = int(masColor/*[ikjl(i, Y-1, tyk, 0)][ikjl(j, X-1, tyk, 1)]*/[xy[1]][xy[0]].r) / float(255);
					}

				float x1 = CubicPolate(ndata[0][0], ndata[1][0], ndata[2][0], ndata[3][0], (i % tyk) / float(tyk));
				float x2 = CubicPolate(ndata[0][1], ndata[1][1], ndata[2][1], ndata[3][1], (i % tyk) / float(tyk));
				float x3 = CubicPolate(ndata[0][2], ndata[1][2], ndata[2][2], ndata[3][2], (i % tyk) / float(tyk));
				float x4 = CubicPolate(ndata[0][3], ndata[1][3], ndata[2][3], ndata[3][3], (i % tyk) / float(tyk));

				t = CubicPolate(x1, x2, x3, x4, (j % tyk) / float(tyk)) * 255.f;

				if (t < 0)
					t = 0;
				if (t > 255)
					t = 255;

				masColor[i][j] = Color(int(t), int(t), int(t));
			}
		}
	return masColor;
}

Color** bilinearinterpol(Color** masColor, int tyk) {
	float* b = new float[4];
	bool flag = true;

	for (int i = 0; i < height; i++)
		for (int j = 0; j < width; j++) {
			if (i % tyk != 0 || j % tyk != 0) {
				int t = 0, t11 = i / tyk * tyk, t12 = j / tyk * tyk, t21 = (i / tyk + 1) * tyk, t22 = (j / tyk + 1) * tyk;

				b[0] = int(masColor[i / tyk * tyk][j / tyk * tyk].r);
				int* xy = ikjl2(i, 1, j, 0, tyk);
				b[1] = int(masColor/*[ikjl(i, 1, tyk, 0)][ikjl(j, 0, tyk, 1)]*/[xy[1]][xy[0]].r) - int(masColor[i / tyk * tyk][j / tyk * tyk].r);
				int* xy2 = ikjl2(i, 0, j, 1, tyk);
				b[2] = int(masColor/*[ikjl(i, 0, tyk, 0)][ikjl(j, 1, tyk, 1)]*/[xy2[1]][xy2[0]].r) - int(masColor[i / tyk * tyk][j / tyk * tyk].r);
				int* xy3 = ikjl2(i, 1, j, 1, tyk);
				b[3] = int(masColor[i / tyk * tyk][j / tyk * tyk].r) - int(masColor/*[ikjl(i, 1, tyk, 0)][ikjl(j, 0, tyk, 1)]*/[xy[1]][xy[0]].r) - int(masColor/*[ikjl(i, 0, tyk, 0)][ikjl(j, 1, tyk, 1)]*/[xy2[1]][xy2[0]].r) + int(masColor/*[ikjl(i, 1, tyk, 0)][ikjl(j, 1, tyk, 1)]*/[xy3[1]][xy3[0]].r);
				t = b[0] + b[1] * (i % tyk / float(tyk)) + b[2] * (j % tyk / float(tyk)) + b[3] * (i % tyk / float(tyk)) * (j % tyk / float(tyk));

				masColor[i][j] = Color(t, t, t);
			}
		}
	return masColor;
}

int streight123(int x, int x1, int y1, int x2, int y2) {
	return (x - x2) * (y2 - y1) / (x2 - x1) + y2;
}

int streight(int i, int x1, int y1, int x2, int y2) {

	if (i > (height / x2 / 2 - 1) * x2 && i < (height / x2 / 2 + 1) * x2) {
		if (i < (height / x2 / 2) * x2)
			i = int((height / x2 / 2) * x2) - (i % int((height / x2 / 2) * x2));
		else
			i = i % (height / x2 / 2 * x2);
		return (i - x2) * (y1 - y2) / (x1+0.3 - x2) + y2;
	}
	if (i > x2 && i < (height / x2 - 1) * x2) return y2;
	if (i >= (height / x2 - 1) * x2)
		i = height - i;
	return (i - x2) * (y1 - y2) / (x1 - x2) + y2;
}

float shadow1(Color** masColor, int x, int y) {
	int max = int(masColor[x][y].r);
	if (max == 0) {
		max = 1;
		masColor[x][y].r = 1;
	}
	for (int i = 1; i < 6; i++) {
		int* xy = ikjl2(x, -i, y, -i, 1);
		if (max < int(masColor[xy[1]][xy[0]].r) && int(masColor[xy[1]][xy[0]].r) > 180)
			max = int(masColor[xy[1]][xy[0]].r);
	}

	return pow(int(masColor[x][y].r) / float(max), 2);
}

bool dist(int x1, int y1, int* f, int tyk, int r) {
	for (int i = 0; i < tyk; i++) {
		if (pow(x1 - (x1 + i - tyk / 2), 2) + pow(y1 - f[x1 + i - tyk / 2], 2) <= pow(r, 2))
			return true;
	}
	return false;
}

bool dist1(int x1, int y1, int* f, int tyk, int r) {
	if (y1 < tyk / 2)
		for (int i = tyk / 2 - y1; i < tyk; i++) {
			int temp = y1 + i - tyk / 2;
			if (pow(x1 - f[y1 + i - tyk / 2], 2) + pow(y1 - (y1 + i - tyk / 2), 2) <= pow(r, 2))
				return true;
		}
	else
		for (int i = 0; i < tyk; i++) {
			if (pow(x1 - f[y1 + i - tyk / 2], 2) + pow(y1 - (y1 + i - tyk / 2), 2) <= pow(r, 2))
				return true;
		}
	return false;
}

void searchLocMins(Color** masColor, /*int***/Color** locMins, int& n1) {
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			int countOfNumbersBigger = 0;
			for (int X = 0; X < 3; X++)
				for (int Y = 0; Y < 3; Y++) {
					if (X != 1 || Y != 1) {
						int* xy = ikjl2(i, Y - 1, j, X - 1, 1);
						int xxx = xy[0], yyy = xy[1];
						if (int(masColor[i][j].r) >= int(masColor[xy[1]][xy[0]].r)) {
							countOfNumbersBigger++;
							if (i >= 255 && j >= 255) {
								int flower = 0;
							}
						}
					}
				}
			if (countOfNumbersBigger >= 8 && n1 < 10000) {
				/*locMins[n1][0] = i; locMins[n1][1] = j;
				n1++;*/
				locMins[i][j] = Color(255, 255, 255);
			}
		}
	}
}

void addInClusters(Color** masColor2, int** clusters, int &n1, int i, int j) {
	if (masColor2[i][j].r > 250) {
		clusters[n1][0] = j;
		clusters[n1][1] = i;
		n1++;
		masColor2[i][j] = Color(0, 0, 0);
		for (int k = -1; k < 2; k++)
			for (int l = -1; l < 2; l++)
				if (i + k < height && j + l < width && i + k >= 0 && j + l >= 0) {
					//cout << "hello";
					addInClusters(masColor2, clusters, n1, i + k, j + l);
				}
	}

}

int** deleteEl(int** mas, int oldN, int elem) {
	int** temp = new int* [oldN - 1];
	for (int i = 0; i < elem; i++) {
		temp[i] = new int[2];
		temp[i][0] = mas[i][0];
		temp[i][1] = mas[i][1];
	}
	for (int i = elem; i < oldN-1; i++) {
		temp[i] = new int[2];
		temp[i][0] = mas[i + 1][0];
		temp[i][1] = mas[i + 1][1];
	}
	return temp;
}

Color** rivers(Color** masColor) {
	Color** masColor2 = new Color * [height];
	for (int i = 0; i < height; i++) {
		masColor2[i] = new Color[width];
		for (int j = 0; j < width; j++)
			masColor2[i][j] = Color(0, 0, 0);
	}

	/*for (int i = 0; i < height; i++)
		for (int j = 0; j < width; j++) {
			if (int(masColor[i][j].r) >= 240)
				masColor2[i][j] = Color(255, 255, 255);

		}*/
	int n = 0, n1 = 0, oldN = 0;
	searchLocMins(masColor, /*midDots*/masColor2, oldN);
	int*** clusters = new int** [60000];
	int* n1_ = new int[60000];
	for (int i = 0; i < 60000; i++) {
		clusters[i] = new int* [10000];
		for (int j = 0; j < 10000; j++) {
			clusters[i][j] = new int[2];
			for (int k = 0; k < 2; k++) {
				clusters[i][j][k] = 0;
				//cout << clusters[i][j][k] << " ";
			}
		}
		/*cout << endl;*/
	}
	for (int i = 0; i < height; i++)
		for (int j = 0; j < width; j++) {
			if (masColor2[i][j].r >= 240) {
				if (n >= 60000) {
					cout << "pzdc" << endl;
					break;
				}
				n1 = 0;
				//cout << "n = " << n << endl;
				addInClusters(masColor2, clusters[n], n1, i, j);
				/*for (int k = 0; k < n1; k++) {
					cout << "(" << clusters[n][k][0] << ", " << clusters[n][k][1] << "), ";
				}
				cout << endl;*/
				n1_[n] = n1;
				n++;
			}
		}
	int** midDots = new int* [height * width / 9 + 1], * radii = new int[height * width / 9 + 1]; oldN = 0;
	for (int j = 0; j < height * width / 9 + 1; j++) {
		midDots[j] = new int[2];
		midDots[j][0] = -1; midDots[j][1] = -1;
		radii[j] = 0;
	}

	
	//n = oldN;

	for (int k = 0; k < n; k++){
		int sumX = 0, sumY = 0;
		for (int j = 0; j < n1_[k]; j++) {
			sumX += clusters[k][j][0];
			sumY += clusters[k][j][1];
		}
		midDots[k][0] = sumX / n1_[k];
		midDots[k][1] = sumY / n1_[k];
		cout << "(" << midDots[k][0] << ", " << midDots[k][1] << ") ";
		for (int j = 0; j < n1_[k]; j++) {
			if (sqrt(pow(midDots[k][0] - clusters[k][j][0], 2) + pow(midDots[k][1] - clusters[k][j][1], 2)) > radii[k])
				radii[k] = sqrt(pow(midDots[k][0] - clusters[k][j][0], 2) + pow(midDots[k][1] - clusters[k][j][1], 2));
		}
		masColor2[midDots[k][0]][midDots[k][1]] = Color(255, 255, 255);
		int zeroX = midDots[k][0] - /*radii[k]*/2 / 2 - 8, zeroY = midDots[k][1] - /*radii[k]*/2 / 2 - 8;
		for (int i = 0; i < /*radii[k]*/2 + 16; i++)
			for (int j = 0; j < /*radii[k]*/2 + 16; j++) {
				if (i + zeroY >= 0 && j + zeroX >= 0 && i + zeroY < height && j + zeroX < width 
					&& pow(i - /*radii[k]*/2 / 2 - 8, 2) + pow(j - /*radii[k]*/2 / 2 - 8, 2) > pow(/*radii[k]*/2 * 0.9f - 1, 2) 
					&& pow(i - /*radii[k]*/2 / 2 - 8, 2) + pow(j - /*radii[k]*/2 / 2 - 8, 2) < pow(/*radii[k]*/2 * 0.9f + 1, 2))
					masColor2[i + zeroY][j + zeroX] = Color(255, 255, 255);
			}
	}
	//cout << endl;

	int dotStartX = midDots[0][0], dotEndX = midDots[n - 1][0], dotStartY = midDots[0][1], dotEndY = midDots[n - 1][1],
		minDist = height * width, currentDotX = 0, currentDotY = 0, newN = n, delEl = -1, breadthRiv = 3;
	bool exitBool = false;
	for (int i = 0; i < height / 2; i++) {
		for (int j = 0; j < i + 1; j++) {
			if (dotStartY + j > 0 && dotStartY + j < height && dotStartX + j > 0 && dotStartX + j < width)
				if (int(masColor[dotStartY + j][dotStartX + j].r) < 170) {
					dotEndX = dotStartX + j;
					dotEndY = dotStartY + j;
					exitBool = true;
					break;
				}
			if (dotStartY - j > 0 && dotStartY - j < height && dotStartX - j > 0 && dotStartX - j < width)
				if (int(masColor[dotStartY - j][dotStartX - j].r) < 170) {
					dotEndX = dotStartX - j;
					dotEndY = dotStartY - j;
					exitBool = true;
					break;
				}
			if (dotStartY - j > 0 && dotStartY - j < height && dotStartX + j > 0 && dotStartX + j < width)
				if (int(masColor[dotStartY - j][dotStartX + j].r) < 170) {
					dotEndX = dotStartX + j;
					dotEndY = dotStartY - j;
					exitBool = true;
					break;
				}
			if (dotStartY + j > 0 && dotStartY + j < height && dotStartX - j > 0 && dotStartX - j < width)
				if (int(masColor[dotStartY + j][dotStartX - j].r) < 170) {
					dotEndX = dotStartX - j;
					dotEndY = dotStartY + j;
					exitBool = true;
					break;
				}
		}
		if (exitBool)
			break;
	}

	for (int k = 0; k < n; k++) {
		minDist = height * width;
		for (int l = 0; l < n - k; l++) {
			//cout << "(" << midDots[l][0] << ", " << midDots[l][1] << ") ";
			if (sqrt(pow(dotStartX - midDots[l][0], 2) + pow(dotStartY - midDots[l][1], 2)) < minDist) {
				minDist = sqrt(pow(dotStartX - midDots[l][0], 2) + pow(dotStartY - midDots[l][1], 2));
				currentDotX = midDots[l][0];
				currentDotY = midDots[l][1];
				delEl = l;
			}
		}
		//cout << endl;
		if (sqrt(pow(dotStartX - dotEndX, 2) + pow(dotStartY - dotEndY, 2)) < minDist) {
			minDist = sqrt(pow(dotStartX - dotEndX, 2) + pow(dotStartY - dotEndY, 2));
			currentDotX = dotEndX;
			currentDotY = dotEndY;
			delEl = -1;
		}
		if (delEl >= 0)
			midDots = deleteEl(midDots, newN, delEl); newN--;

		int m = currentDotX - dotStartX, p = currentDotY - dotStartY, x = 0, y = 0;
		
		//cout << "(" << dotStartX << ", " << dotStartY << "), (" << currentDotX << ", " << currentDotY << ") " << endl;

		for (int t = 0; t < 1000; t++) {
			x = t / 1000.f * m + dotStartX;
			y = t / 1000.f * p + dotStartY;
			masColor2[y][x] = Color(255, 255, 255);
			breadthRiv += 0/*rand() % 3 - 1*/;
			if (breadthRiv < 1) breadthRiv = 1;
			if (breadthRiv > 3) breadthRiv = 3;
			for (int p = 0; p < breadthRiv; p++) {
				for (int q = 0; q < breadthRiv; q++) {
					if (y + p - breadthRiv / 2 >= 0 && y + p - breadthRiv / 2 < height && x + q - breadthRiv / 2 >= 0 && x + q - breadthRiv / 2 < width)
						masColor2[y + p - breadthRiv / 2][x + q - breadthRiv / 2] = Color(255, 255, 255);
					else if (y + p - breadthRiv / 2 < 0 || y + p - breadthRiv / 2 >= height) {
						if (x + q - breadthRiv / 2 >= 0 && x + q - breadthRiv / 2 < width)
							masColor2[y][x + q - breadthRiv / 2] = Color(255, 255, 255);
						else 
							masColor2[y][x] = Color(255, 255, 255);
					} 
					else
						masColor2[y + p - breadthRiv / 2][x] = Color(255, 255, 255);
				}
			}
		}

		dotStartX = currentDotX;
		dotStartY = currentDotY;
	}
	return masColor2;
}

Color** coloring(Color** masColor/*, Color** masColor2*/) {
	Color** masColor2 = rivers(masColor);

	float** shadow2 = new float* [height];
	for (int i = 0; i < height; i++)
		shadow2[i] = new float[width];

	for (int i = 0; i < height / 2; i++)
		for (int j = 0; j < width; j++) {
			int t = int(masColor[i][j].r) * i / height / 2.05;
		}
	for (int i = 0; i < height; i++)
		for (int j = 0; j < width; j++) {
			shadow2[i][j] = shadow1(masColor, i, j);
		}

	//int constt = 16, count = 10;

	//int x1, y1, x2, y2;//(x - x2) * (y2 - y1) / (x2 - x1) + y2;

	//float* a = new float[10];
	//float* b = new float[10];
	//for (int k = 0; k < 10; k++) {
	//	x1 = 0; y1 = rand() % width; x2 = height - 1; y2 = rand() % width;
	//	if (x2 == x1)
	//		x1++;
	//	a[k] = float(y2 - y1) / (x2 - x1);
	//	b[k] = y2 - x2 * (y2 - y1) / (x2 - x1);
	//}
	//int** x = new int* [count];
	//for (int i = 0; i < count; i++)
	//	x[i] = new int[height];
	//for (int i = 0; i < count; i++) {
	//	x[i][0] = b[i] - rand() % 60 + 30;
	//	for (int j = 0; j < height; j++) {
	//		if (j / constt * constt == j)
	//			x[i][j / constt * constt + constt] = a[i] * j + b[i] - rand() % 60 + 30;
	//		if (j / constt * constt != j)
	//			x[i][j] = streight123(j, j / constt * constt, x[i][j / constt * constt], j / constt * constt + constt, x[i][j / constt * constt + constt]);
	//	}
	//}

	/*int** y = new int* [10 - count];
	for (int i = 0; i < 10 - count; i++)
		y[i] = new int[width];
	for (int i = 0; i < 10 - count; i++) {
		y[i][0] = -b[i + count] / a[i + count] - rand() % 60 + 30;
		for (int j = 0; j < width; j++) {
			if (j / constt * constt == j)
				y[i][j / constt * constt + constt] = 1.f / a[i + count] * j + b[i + count] / a[i + count] - rand() % 60 + 30;
			if (j / constt * constt != j)
				y[i][j] = streight123(j, j / constt * constt, y[i][j / constt * constt], j / constt * constt + constt, y[i][j / constt * constt + constt]);
		}
	}*/

	/*for (int i = 0; i < count; i++) {
		for (int j = 0; j < height; j++) {
			int sum = 0, a = 5;
			if (j < 5)
				a = j;
			for (int k = 0; k < a; k++)
				sum += x[i][j - k];
			x[i][j] = sum / 5;
		}
	}*/

	/*for (int i = 0; i < 10 - count; i++) {
		for (int j = 0; j < width; j++) {
			int sum = 0, a = 5;
			if (j < 5)
				a = j;
			for (int k = 0; k < a; k++)
				sum += y[i][j-k];
			y[i][j] = sum / 5;
		}
	}*/


	for (int i = 0; i < height; i++)
		for (int j = 0; j < width; j++) {
			float shadow = shadow2[i][j];
			Color grass, sea, sand, snow; snow = Color(230, 230, 255); grass = Color(68, 148, 74); sea = Color(66, 170, 255); sand = Color(255, 204, 51);
			/*if (int(masColor[i][j].r) < 170) 
				masColor[i][j] = sea;
			else if (int(masColor[i][j].r) < 180)
				masColor[i][j] = sand;
			else if (int(masColor[i][j].r) < 240)
				masColor[i][j] = grass;
			else
				masColor[i][j] = snow;*/
			//masColor[i][j] = sand;

			/*if ((int(masColor[i][j].r) == 255 || int(masColor[i][j].r) == 68) && int(masColor2[i][j].r) > 120 && int(masColor2[i][j].r) < 130 )
				masColor[i][j] = sea;
			if ((int(masColor[i][j].r) == 255 || int(masColor[i][j].r) == 68) && ((int(masColor2[i][j].r) < 115 && int(masColor2[i][j].r) > 110)|| (int(masColor2[i][j].r) < 135 && int(masColor2[i][j].r) > 130)))
				masColor[i][j] = sand;*/
			if (masColor2[i][j].r > 250)
				masColor[i][j] = Color(139, 0, 255);
			/*for (int k = 0; k < count; k++) {
				if (dist(i, j, x[k], 14, 2)) {
					masColor[i][j] = sea;
				}
			}*/

			/*for (int k = 0; k < 10 - count; k++) {
				if (dist1(i, j, y[k], 14, 2)) {
					masColor[i][j] = sea;
				}
			}*/

		}
	return masColor;
}