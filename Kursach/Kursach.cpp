#define _USE_MATH_DEFINES 

#include "Loading.h"
#include "Colorizing.h"
#include "ctime"

using namespace std;
using namespace sf;

int main()
{
	MapDisplayMode displayMode = Topographic;
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

	GenerateHeightsMap(perlineNoise, map, tex, rand() % 5000, displayMode);

	bool mapIsChange = false;

	Font font;
	if (!font.loadFromFile("arial.ttf"))
		return EXIT_FAILURE;

	Text newHeightsText;
	newHeightsText.setFont(font);
	newHeightsText.setStyle(Text::Bold);
	newHeightsText.setString("Create new height's map\nwith random seed");
	newHeightsText.setFillColor(Color::Black);
	newHeightsText.setCharacterSize(20);
	newHeightsText.setPosition(WIDTH_WINDOW - 280, HEIGHT_WINDOW / 4);

	RectangleShape rectangleNewHeights(Vector2f(250, 50));
	rectangleNewHeights.setPosition(WIDTH_WINDOW - 280, HEIGHT_WINDOW / 4);
	rectangleNewHeights.setFillColor(Color(100, 250, 50));

	Text inputSeedText;
	inputSeedText.setFont(font);
	inputSeedText.setStyle(Text::Bold);
	inputSeedText.setString("Create new height's map\nwith input seed");
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
					GenerateHeightsMap(perlineNoise, map, tex, rand() % 5000, displayMode);
				}
				if (rectangleInputSeed.getGlobalBounds().contains(mousePosF))
				{
					cout << "Input seed: ";
					int seed = 5;
					cin >> seed;
					GenerateHeightsMap(perlineNoise, map, tex, seed, displayMode);
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

