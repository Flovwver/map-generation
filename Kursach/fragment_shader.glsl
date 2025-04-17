#version 120

// --- Uniform-переменные ---
// Текстура с высотами: высота закодирована в компоненте R.
uniform sampler2D colormap2;

// Коэффициент масштабирования (зум).
uniform float zoom;

// Углы поворота вокруг осей X и Y (передаются из приложения).
uniform float angleX;
uniform float angleY;

// Размер области (или экрана), используется для преобразования [0,1]→[0, resolution].
uniform vec2 resolution;

// --- Атрибуты вершины ---
// Стандартное имя для позиции в SFML: задается как "position" (диапазон [0,1]).
attribute vec2 position;
// Атрибут с UV-координатами текстуры:
attribute vec2 texCoords;

// --- Переменные, передаваемые во фрагментный шейдер ---
varying vec2 vTexCoords;   // будем передавать UV
varying vec3 vNormal;      // вычисленная нормаль для освещения

// --- Uniform-переменные ---
// Текстура базового цвета: может использоваться для наложения цветового слоя.
uniform sampler2D colormap;

// Направление источника света (должно быть нормализовано).
uniform vec3 lightDir;

// --- Переменные, полученные из вершинного шейдера ---
varying vec2 vTexCoords;   // UV-координаты для выборки базового цвета
varying vec3 vNormal;      // Нормаль, вычисленная в вершинном шейдере

void main() {
    // Считываем базовый цвет из текстуры colormap.
    vec4 baseColor = texture2D(colormap, vTexCoords);

    // Нормализуем нормаль и направление света.
    vec3 norm = normalize(vNormal);
    vec3 light = normalize(lightDir);

    // Вычисляем диффузное освещение через скалярное произведение.
    // Функция max гарантирует, что значение не будет отрицательным.
    float diffuse = max(dot(norm, light), 0.0);

    // Итоговый цвет получается путём умножения базового цвета на коэффициент освещённости.
    vec3 finalColor = baseColor.rgb * diffuse;

    gl_FragColor = vec4(finalColor, baseColor.a);
}
