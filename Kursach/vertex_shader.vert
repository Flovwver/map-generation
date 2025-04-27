#version 120

// --- Uniform-переменные ---
// Текстура с высотами: высота закодирована в компоненте R.
uniform sampler2D colormap2;

// Коэффициент масштабирования (зум).
uniform float zoom;

// Углы поворота вокруг осей X и Y (передаются из приложения).
uniform float angleX;
uniform float angleY;

// Размер области (или экрана)
uniform vec2 resolution;

// --- Атрибуты вершины ---
// Стандартное имя для позиции в SFML: задается как "position" (диапазон [0,1]).
attribute vec3 position;

// Атрибут с UV-координатами текстуры:
attribute vec2 texCoords;

// --- Переменные, передаваемые во фрагментный шейдер ---
varying vec2 vTexCoords;   // будем передавать UV
varying vec3 vNormal;      // вычисленная нормаль для освещения

vec4 rotate(vec4 vector, float ax, float ay) {
    mat4 translation0505;
    translation0505[0] = vec4(1.f, 0.f, 0.f, -0.5f);
    translation0505[1] = vec4(0.f, 1.f, 0.f, -0.5f);
    translation0505[2] = vec4(0.f, 0.f, 1.f, 0.f);
    translation0505[3] = vec4(0.f, 0.f, 0.f, 1.f);

    mat4 translation_0505;
    translation_0505[0] = vec4(1.f, 0.f, 0.f, 0.5f);
    translation_0505[1] = vec4(0.f, 1.f, 0.f, 0.5f);
    translation_0505[2] = vec4(0.f, 0.f, 1.f, 0.f);
    translation_0505[3] = vec4(0.f, 0.f, 0.f, 1.f);

    mat4 translationx;
    translationx[0] = vec4(1.f, 0.f,        0.f,        0.f);
    translationx[1] = vec4(0.f, cos(ax),    -sin(ax),   0.f);
    translationx[2] = vec4(0.f, sin(ax),    cos(ax),    0.f);
    translationx[3] = vec4(0.f, 0.f,        0.f,        1.f);
    
    mat4 translationy;
    translationy[0] = vec4(cos(ay), 0.f,    -sin(ay),   0.f);
    translationy[1] = vec4(0.f,     1.f,    0.f,        0.f);
    translationy[2] = vec4(sin(ay), 0.f,    cos(ay),    0.f);
    translationy[3] = vec4(0.f,     0.f,    0.f,        1.f);

    vec4 newVector = translation_0505 * translationy * translationx * translation0505 * vector;
    return newVector;
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
    vec3 normal = vec3(hL - hR, hD - hU, 0.01f);
    return normalize(normal);
}

void main() {
    // Передаём UV-координаты во фрагментный шейдер.
    gl_TexCoord[0] = gl_TextureMatrix[0] * gl_MultiTexCoord0;

    // Вычисляем нормаль методом конечных разностей по текстурным координатам.
    vNormal = computeNormal(texCoords);

    // Нормализуем координаты вершины.
    vec4 pos = gl_Vertex;

    // Применяем поворот на основе uniform-значений.
    pos = rotate(pos, angleX, angleY);

    // Масштабируем координаты после вращения.
    pos.xy *= zoom;

    // Преобразуем в окончательное положение.
    gl_Position = gl_ModelViewProjectionMatrix * pos;
}
