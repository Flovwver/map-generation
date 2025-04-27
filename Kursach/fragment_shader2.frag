#version 330 core

out vec4 fragColor;

void main() {
    // Переместим треугольник в область видимости
    vec3 v0 = vec3(100, 100, 0);
    vec3 v1 = vec3(300, 100, 0);
    vec3 v2 = vec3(200, 400, 0);

    // Генерация луча на основе экранных координат
    vec3 rayOrigin = vec3(gl_FragCoord.xy, 500.0); // Начало луча
    vec3 rayTarget = vec3(gl_FragCoord.xy, 0.0);   // Цель луча — плоскость треугольника
    vec3 rayDirection = normalize(rayTarget - rayOrigin); // Нормализованный вектор направления

    // Вектор нормали плоскости треугольника
    vec3 edge1 = v1 - v0;
    vec3 edge2 = v2 - v0;
    vec3 normal = cross(edge1, edge2);

    // Проверка на параллельность
    float nDotD = dot(normal, rayDirection);
    if (abs(nDotD) < 1e-6) {
        fragColor = vec4(1.0, 0.0, 0.0, 1.0); // Нет пересечения — красный цвет
        return;
    }

    // Вычисление t для пересечения
    float t = dot(normal, v0 - rayOrigin) / nDotD;
    if (t < 0.0) {
        fragColor = vec4(0.0, 1.0, 0.0, 1.0); // Луч не пересекает треугольник — зеленый цвет
        return;
    }

    // Точка пересечения
    vec3 P = rayOrigin + t * rayDirection;

    // Проверка, находится ли точка P внутри треугольника
    vec3 C;

    // Вектор AB
    vec3 edge0 = v1 - v0;
    vec3 vp0 = P - v0;
    C = cross(edge0, vp0);
    if (dot(normal, C) < 0.0) {
        fragColor = vec4(0.0, 0.0, 1.0, 1.0); // За пределами треугольника — синий цвет
        return;
    }

    // Вектор BC
    edge1 = v2 - v1;
    vec3 vp1 = P - v1;
    C = cross(edge1, vp1);
    if (dot(normal, C) < 0.0) {
        fragColor = vec4(0.0, 0.0, 1.0, 1.0); // За пределами треугольника — синий цвет
        return;
    }

    // Вектор CA
    edge2 = v0 - v2;
    vec3 vp2 = P - v2;
    C = cross(edge2, vp2);
    if (dot(normal, C) < 0.0) {
        fragColor = vec4(0.0, 0.0, 1.0, 1.0); // За пределами треугольника — синий цвет
        return;
    }

    // Точка P лежит внутри треугольника
    fragColor = vec4(1.0, 1.0, 0.0, 1.0); // Пересечение найдено — желтый цвет
}
