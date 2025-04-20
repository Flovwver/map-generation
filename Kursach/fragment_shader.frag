#version 120

// --- Uniform-переменные ---
// Текстура с высотами: высота закодирована в компоненте R.
uniform sampler2D colormap2;

// Коэффициент масштабирования (зум).
uniform float zoom;

// Углы поворота вокруг осей X и Y (передаются из приложения).
uniform float angleX;
uniform float angleY;

// Размер области (или экрана), используется для преобразования [0,1]>[0, resolution].
uniform vec2 resolution;


// --- Переменные, передаваемые во фрагментный шейдер ---
varying vec2 vTexCoords;   // будем передавать UV
varying vec3 vNormal;      // вычисленная нормаль для освещения

// --- Uniform-переменные ---
// Текстура базового цвета: может использоваться для наложения цветового слоя.
uniform sampler2D colormap;

// Направление источника света (должно быть нормализовано).
uniform vec3 lightDir;


void main() {
    // Считываем базовый цвет из текстуры colormap.
    vec4 baseColor = texture2D(colormap, vTexCoords);

    // Нормализуем нормаль и направление света.
    vec3 norm = normalize(vNormal);
    vec3 light = normalize(lightDir);

    // Вычисляем диффузное освещение через скалярное произведение.
    // Функция max гарантирует, что значение не будет отрицательным.
    float diffuse = (dot(norm, light) + 1.f) / 2.f;

    // Итоговый цвет получается путём умножения базового цвета на коэффициент освещённости.
    vec3 finalColor = baseColor.rgb * diffuse;

    gl_FragColor = vec4(finalColor, 1.0);
}
