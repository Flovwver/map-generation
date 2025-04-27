#define _USE_MATH_DEFINES 

#include "Loading.h"
#include <vector>
#include "Colorizing.h"
#include <ctime>
#include <cmath>
#include <algorithm>
#include <iostream>
#include <chrono>

#define SCALE 100   // Масштаб изображения


using namespace std;
using namespace sf;

int main()
{
    MapDisplayMode displayMode = TopographicContinent;
    double zoom = 1.f;
    RenderWindow window(VideoMode({ WIDTH_WINDOW, HEIGHT_WINDOW }), "Random generation");

    float speed = 0.4f;
    Vector2f playercoord(0.f, 0.f);

    setlocale(LC_ALL, "RUS");
    srand(time(0));

    float* perlineNoise = new float[HEIGHT * WIDTH];
    Color** map = new Color * [HEIGHT];

    window.clear(Color(255, 255, 255));

    Texture texMap(sf::Vector2u(WIDTH, HEIGHT));
    Sprite spr(texMap);
    Texture texHeights(sf::Vector2u(WIDTH, HEIGHT));
    //Sprite spr(texHeights);

    GenerateHeightsMap(perlineNoise, map, texMap, texHeights, 7/*rand() % 5000*/, displayMode);

    bool mapIsChange = false;
    auto start = std::chrono::system_clock::now();
    // Загрузка шейдеров: теперь загружаем и вершинный, и фрагментный шейдеры
    Shader shader;
    //if (!shader.loadFromFile("vertex_shader.vert", "fragment_shader.frag"))
    if (!shader.loadFromFile("fragment_shader.frag", sf::Shader::Type::Fragment))
    {
        cout << "Ошибка загрузки шейдеров" << endl;
        return -1;
    }

    // Передаем текстуры в шейдер
    shader.setUniform("colormap", texMap);
    shader.setUniform("colormap2", texHeights);

    Font font("arial.ttf");

    Text newHeightsText(font);
    newHeightsText.setStyle(Text::Bold);
    newHeightsText.setString("Create new height's map\nwith random seed");
    newHeightsText.setFillColor(Color::Black);
    newHeightsText.setCharacterSize(20);
    newHeightsText.setPosition(Vector2f(WIDTH_WINDOW - 280, HEIGHT_WINDOW / 4));

    RectangleShape rectangleNewHeights(Vector2f(250, 50));
    rectangleNewHeights.setPosition(Vector2f(WIDTH_WINDOW - 280, HEIGHT_WINDOW / 4));
    rectangleNewHeights.setFillColor(Color(100, 250, 50));

    Text inputSeedText(font);
    inputSeedText.setStyle(Text::Bold);
    inputSeedText.setString("Create new height's map\nwith input seed");
    inputSeedText.setFillColor(Color::Black);
    inputSeedText.setCharacterSize(20);
    inputSeedText.setPosition(Vector2f(WIDTH_WINDOW - 280, 3 * HEIGHT_WINDOW / 4));

    RectangleShape rectangleInputSeed(Vector2f(250, 50));
    rectangleInputSeed.setPosition(Vector2f(WIDTH_WINDOW - 280, 3 * HEIGHT_WINDOW / 4));
    rectangleInputSeed.setFillColor(Color(100, 250, 50));

    // Изначальные углы поворота камеры (эти значения будут изменяться мышью)
    float angleX = 0.0f;
    float angleY = 0.0f;

    // Направление света (нормализованное)
    Vector3f lightDir(-0.5f, 0.5f, 0.0f);
    float len = sqrt(lightDir.x * lightDir.x + lightDir.y * lightDir.y + lightDir.z * lightDir.z);
    lightDir /= len;

    // Переменные для обработки мыши
    bool dragging = false;
    Vector2i lastMousePos = Mouse::getPosition(window);
    float sensitivity = 0.005f;  // Регулировка чувствительности поворота

    RectangleShape MapPlace(Vector2f(WIDTH, HEIGHT));
    MapPlace.setPosition(Vector2f(0, 0));
    MapPlace.setFillColor(Color(255, 0, 255));

    // --- Изменение геометрии --- 
    // Вершины задаются в нормальном диапазоне [0,1] (будут использоваться как UV)
    VertexArray terrain(sf::PrimitiveType::TriangleFan, HEIGHT * WIDTH);
    terrain[0].position = Vector2f(0.f, 0.f);  // левый верхний угол (UV = (0,0))
    terrain[1].position = Vector2f(WIDTH, 0.f);  // правый верхний (UV = (1,0))
    terrain[2].position = Vector2f(WIDTH, HEIGHT);  // правый нижний  (UV = (1,1))
    terrain[3].position = Vector2f(0.f, HEIGHT);  // левый нижний   (UV = (0,1))
    // Необязательно: задаём texCoords так же, чтобы в шейдере использовать их как координаты.
    terrain[0].texCoords = Vector2f(0.f, 0.f);
    terrain[1].texCoords = Vector2f(1.f, 0.f);
    terrain[2].texCoords = Vector2f(1.f, 1.f);
    terrain[3].texCoords = Vector2f(0.f, 1.f);

    terrain[0].color = sf::Color::Red;
    terrain[1].color = sf::Color::Blue;
    terrain[2].color = sf::Color::Red;
    terrain[3].color = sf::Color::Green;

    // --- Конец изменений по геометрии ---

    sf::Vector3f rayOrigin(0.0f, 0.0f, 10.0f);
    sf::Vector3f rayDirection = sf::Vector3f(0.5f, 0.5f, -1.0f);
    rayDirection = rayDirection / std::sqrt(rayDirection.x * rayDirection.x +
        rayDirection.y * rayDirection.y +
        rayDirection.z * rayDirection.z); // Нормализация

    while (window.isOpen())
    {
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window.close();
            else if (const auto* mouseMoved = event->getIf<sf::Event::MouseMoved>())
            {
                sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                sf::Vector2f mousePosF(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));

                if (dragging) {
                    // вычисляем разницу в положении мыши
                    int dx = mousePos.x - lastMousePos.x;
                    int dy = mousePos.y - lastMousePos.y;
                    angleY += dx * sensitivity;
                    angleX += dy * sensitivity;
                    lastMousePos = mousePos;
                }
                else {
                    // Обработка подсветки UI-элементов
                    if (rectangleNewHeights.getGlobalBounds().contains(mousePosF))
                        rectangleNewHeights.setFillColor(Color(250, 20, 20));
                    else
                        rectangleNewHeights.setFillColor(Color(100, 250, 50));

                    if (rectangleInputSeed.getGlobalBounds().contains(mousePosF))
                        rectangleInputSeed.setFillColor(Color(250, 20, 20));
                    else
                        rectangleInputSeed.setFillColor(Color(100, 250, 50));
                }
            }
            else if (const auto* mousePressed = event->getIf<sf::Event::MouseButtonPressed>())
            {
                if (mousePressed->button == sf::Mouse::Button::Right) {
                    dragging = true;
                    lastMousePos = sf::Mouse::getPosition(window);
                }
                sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                sf::Vector2f mousePosF(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));
                if (rectangleNewHeights.getGlobalBounds().contains(mousePosF))
                {
                    GenerateHeightsMap(perlineNoise, map, texMap, texHeights, rand() % 5000, displayMode);
                }
                if (rectangleInputSeed.getGlobalBounds().contains(mousePosF))
                {
                    cout << "Input seed: ";
                    int seed = 5;
                    cin >> seed;
                    GenerateHeightsMap(perlineNoise, map, texMap, texHeights, seed, displayMode);
                }
            }
            else if (const auto* MouseReleased = event->getIf<sf::Event::MouseButtonReleased>())
            {
                if (MouseReleased->button == sf::Mouse::Button::Right) {
                    dragging = false;
                }
            }
            else if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>())
            {
                if (keyPressed->scancode == sf::Keyboard::Scancode::Escape)
                    window.close();
                if (keyPressed->scancode == sf::Keyboard::Scancode::Right) {
                    angleX += speed / WIDTH;
                }
                if (keyPressed->scancode == sf::Keyboard::Scancode::Left) {
                    angleX -= speed / WIDTH;
                }
                if (keyPressed->scancode == sf::Keyboard::Scancode::Up) {
                    angleY += speed / WIDTH;
                }
                if (keyPressed->scancode == sf::Keyboard::Scancode::Down) {
                    angleY -= speed / WIDTH;
                }
            }
            else if (const auto* keyReleased = event->getIf<sf::Event::KeyReleased>())
            {
                if (keyReleased->scancode == sf::Keyboard::Scancode::Equal) {
                    zoom *= 2.f;
                }
                if (keyReleased->scancode == sf::Keyboard::Scancode::Hyphen) {
                    zoom /= 2.f;
                }
            }
        }


        auto current = std::chrono::system_clock::now();
        float different = std::chrono::system_clock::duration(current - start).count();
        
        shader.setUniform("time", different);

        // Обновляем uniform-переменные шейдера
        shader.setUniform("gridSize", Vector2f(WIDTH, HEIGHT));
        shader.setUniform("zoom", float(zoom));
        shader.setUniform("playercoord", playercoord);
        shader.setUniform("angleX", angleX);
        shader.setUniform("angleY", angleY);
        shader.setUniform("lightDirection", Glsl::Vec3(lightDir.x, lightDir.y, lightDir.z));

        // Передача uniform-параметров в шейдер
        shader.setUniform("rayOrigin", Glsl::Vec3(rayOrigin.x, rayOrigin.y, rayOrigin.z));
        shader.setUniform("rayDirection", Glsl::Vec3(rayDirection.x, rayDirection.y, rayDirection.z));

        // Передаем текстуры в шейдер
        shader.setUniform("colorMap", texMap);
        shader.setUniform("heightMap", texHeights);
        window.clear(Color::White);
        
        window.draw(terrain, &shader);
        //window.draw(terrain);

        window.draw(rectangleNewHeights);
        window.draw(newHeightsText);
        window.draw(rectangleInputSeed);
        window.draw(inputSeedText);

        window.display();
    }

    return 0;
}
