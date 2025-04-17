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

// Функция, осуществляющая поворот точки в 3D вокруг осей X и Y.
vec3 rotatePoint(vec3 pos, float ax, float ay) {
    // Поворот вокруг оси X:
    float cosX = cos(ax);
    float sinX = sin(ax);
    float y = pos.y * cosX - pos.z * sinX;
    float z = pos.y * sinX + pos.z * cosX;
    pos.y = y;
    pos.z = z;
    // Поворот вокруг оси Y:
    float cosY = cos(ay);
    float sinY = sin(ay);
    float x = pos.x * cosY + pos.z * sinY;
    z = -pos.x * sinY + pos.z * cosY;
    pos.x = x;
    pos.z = z;
    return pos;
}

// Функция вычисляет нормаль, используя центральные разности по текстурным координатам.
// Здесь мы предполагаем, что разрешение текстуры (или области) совпадает с uniform `resolution`.
vec3 computeNormal(vec2 uv) {
    // Мелкий сдвиг для выборки соседних значений.
    float offset = 1.0 / resolution.x;
    // Считываем высоты соседних пикселей.
    float hL = texture2D(colormap2, uv - vec2(offset, 0.0)).r;
    float hR = texture2D(colormap2, uv + vec2(offset, 0.0)).r;
    float hD = texture2D(colormap2, uv - vec2(0.0, offset)).r;
    float hU = texture2D(colormap2, uv + vec2(0.0, offset)).r;
    // Здесь вектор нормали образуется из разностей (hL - hR) и (hD - hU).
    // Коэффициент по оси Z (здесь 2.0) можно корректировать для усиления/ослабления влияния высоты.
    vec3 normal = vec3(hL - hR, hD - hU, 2.0);
    return normalize(normal);
}

void main() {
    // Передаём UV-координаты во фрагментный шейдер.
    vTexCoords = texCoords;

    // Считываем значение высоты из текстуры высот colormap2 по UV.
    float height = texture2D(colormap2, texCoords).r;

    // Формируем позицию вершины в мировых координатах.
    // "position" в диапазоне [0,1] масштабируется в [0,resolution].
    // Высота умножается на resolution.x (можно выбрать другой коэффициент для регулировки «подъёма»).
    vec3 pos = vec3(position * resolution, height * resolution.x);

    // Применяем поворот на основе uniform-значений.
    pos = rotatePoint(pos, angleX, angleY);

    // Применяем зум: масштабируем только горизонтальные компоненты.
    pos.xy *= zoom;

    // Вычисляем нормаль методом конечных разностей по текстурным координатам.
    vNormal = computeNormal(texCoords);

    // Финальное преобразование вершины: стандартная проекция с использованием встроенной матрицы.
    gl_Position = gl_ModelViewProjectionMatrix * vec4(pos, 1.0);
}
