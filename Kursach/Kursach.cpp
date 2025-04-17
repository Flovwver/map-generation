#define _USE_MATH_DEFINES 

#include "Loading.h"
#include <vector>
#include "Colorizing.h"
#include <ctime>
#include <cmath>
#include <algorithm>
#include <iostream>

#define SCALE 100   // Масштаб изображения

// Структура для хранения точки
struct Point {
    float x, y, z;
};

using namespace std;
using namespace sf;

// Функция для поворота точки вокруг осей X и Y
Point rotatePoint(Point p, float angleX, float angleY) {
    // Поворот вокруг оси X
    float cosX = cos(angleX), sinX = sin(angleX);
    float y1 = p.y * cosX - p.z * sinX;
    float z1 = p.y * sinX + p.z * cosX;
    // Поворот вокруг оси Y
    float cosY = cos(angleY), sinY = sin(angleY);
    float x1 = p.x * cosY + z1 * sinY;
    float z2 = -p.x * sinY + z1 * cosY;
    return { x1, y1, z2 };
}

// Функция для вычисления нормали к треугольнику
Point calculateNormal(const Point& p1, const Point& p2, const Point& p3) {
    Point u = { p2.x - p1.x, p2.y - p1.y, p2.z - p1.z };
    Point v = { p3.x - p1.x, p3.y - p1.y, p3.z - p1.z };
    Point normal = {
        u.y * v.z - u.z * v.y,
        u.z * v.x - u.x * v.z,
        u.x * v.y - u.y * v.x
    };
    float len = std::sqrt(normal.x * normal.x + normal.y * normal.y + normal.z * normal.z);
    if (len != 0.f) {
        normal.x /= len;
        normal.y /= len;
        normal.z /= len;
    }
    return normal;
}

// Функция для применения освещения к базовому цвету по нормали и направлению света
sf::Color applyLighting(const sf::Color& baseColor, const Point& normal, const Point& lightDir) {
    float dot = normal.x * lightDir.x + normal.y * lightDir.y + normal.z * lightDir.z;
    dot = std::max(0.f, dot);
    return sf::Color(
        static_cast<uint8_t>(baseColor.r * dot),
        static_cast<uint8_t>(baseColor.g * dot),
        static_cast<uint8_t>(baseColor.b * dot)
    );
}

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

    // Загрузка шейдеров: теперь загружаем и вершинный, и фрагментный шейдеры
    Shader shader;
    if (!shader.loadFromFile("vertex_shader.glsl", "fragment_shader.glsl"))
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
    float angleX = 0.4f;
    float angleY = 0.3f;

    // Направление света (нормализованное)
    Vector3f lightDir(0.5f, 1.0f, 1.0f);
    float len = sqrt(lightDir.x * lightDir.x + lightDir.y * lightDir.y + lightDir.z * lightDir.z);
    lightDir /= len;

    // Переменные для обработки мыши
    bool dragging = false;
    Vector2i lastMousePos = Mouse::getPosition(window);
    float sensitivity = 0.005f;  // Регулировка чувствительности поворота

    // --- Изменение геометрии --- 
    // Вместо вычисления множества треугольников на CPU создаем простую плоскость (quad).
    // Вершины задаются в нормальном диапазоне [0,1] (будут использоваться как UV)
    VertexArray terrain(sf::PrimitiveType::TriangleFan, 4);
    terrain[0].position = Vector2f(0.f, 0.f);  // левый верхний угол (UV = (0,0))
    terrain[1].position = Vector2f(1.f, 0.f);  // правый верхний (UV = (1,0))
    terrain[2].position = Vector2f(1.f, 1.f);  // правый нижний  (UV = (1,1))
    terrain[3].position = Vector2f(0.f, 1.f);  // левый нижний   (UV = (0,1))
    // Необязательно: задаём texCoords так же, чтобы в шейдере использовать их как координаты.
    terrain[0].texCoords = Vector2f(0.f, 0.f);
    terrain[1].texCoords = Vector2f(1.f, 0.f);
    terrain[2].texCoords = Vector2f(1.f, 1.f);
    terrain[3].texCoords = Vector2f(0.f, 1.f);
    // --- Конец изменений по геометрии ---

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
                    playercoord.x += speed / WIDTH;
                    if (playercoord.x > 2.f) {
                        playercoord.x = 0.f;
                    }
                }
                if (keyPressed->scancode == sf::Keyboard::Scancode::Left) {
                    playercoord.x -= speed / WIDTH;
                    if (playercoord.x < 0.f) {
                        playercoord.x = 2.f;
                    }
                }
                if (keyPressed->scancode == sf::Keyboard::Scancode::Up) {
                    playercoord.y += speed / WIDTH;
                    if (playercoord.y > 1.f)
                        playercoord.y = 0.f;
                }
                if (keyPressed->scancode == sf::Keyboard::Scancode::Down) {
                    playercoord.y -= speed / WIDTH;
                    if (playercoord.y < 0.f)
                        playercoord.y = 1.f;
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

        // Обновляем uniform-переменные шейдера
        shader.setUniform("resolution", Vector2f(WIDTH, HEIGHT));
        shader.setUniform("zoom", float(zoom));
        shader.setUniform("playercoord", playercoord);
        shader.setUniform("angleX", angleX);
        shader.setUniform("angleY", angleY);
        shader.setUniform("lightDir", Glsl::Vec3(lightDir.x, lightDir.y, lightDir.z));

        // Передаем текстуры в шейдер
        shader.setUniform("colormap", texMap);
        shader.setUniform("colormap2", texHeights);

        window.clear(Color::White);
        // Отрисовка пляски – теперь отрисовываем наш quad с шейдером, который «поднимает» поверхность через колоримэп2
        window.draw(terrain, &shader);

        window.draw(rectangleNewHeights);
        window.draw(newHeightsText);
        window.draw(rectangleInputSeed);
        window.draw(inputSeedText);

        window.display();
    }

    return 0;
}
