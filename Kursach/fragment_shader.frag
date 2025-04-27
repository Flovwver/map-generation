#version 330 core

uniform sampler2D heightMap;    // Текстура высот (TIN)
uniform sampler2D colorMap;
uniform vec2 gridSize;          // Размер сетки (например, vec2(WIDTH, HEIGHT))
uniform vec3 lightDirection;    // Направление света (нормализованный вектор)
vec3 offset = vec3(0.f, 0.f, 0.f);
vec3 center = vec3(gridSize.x / 2.f, gridSize.y / 2.f, 0.f);
float angle = radians(60.0); // Угол поворота
mat3 rotationMatrix_X = mat3(
        1.0, 0.0, 0.0,
        0.0, cos(angle), -sin(angle),
        0.0, sin(angle), cos(angle)
    );
uniform float time;
float fragTime = time / 30000000.f;
mat3 rotationMatrix_Z = mat3(
        cos(fragTime), -sin(fragTime), 0.0f,
        sin(fragTime),  cos(fragTime), 0.0f,
        0.0f,         0.0f,      1.0f
    );

out vec4 fragColor;

bool intersectTriangle(vec3 v0, vec3 v1, vec3 v2, vec3 ro, vec3 rd, out float t, out vec3 intersectionPoint) {
    vec3 edge1 = v1 - v0;
    vec3 edge2 = v2 - v0;
    vec3 normal = cross(edge1, edge2);

    float nDotD = dot(normal, rd);
    if (abs(nDotD) < 1e-6) return false; // Параллельный луч

    t = dot(normal, v0 - ro) / nDotD;
    if (t < 0.0) return false; // Пересечения нет

    vec3 P = ro + t * rd;

    // Проверка внутри треугольника
    vec3 C;
    vec3 edge;
    vec3 vp;

    edge = v1 - v0;
    vp = P - v0;
    C = cross(edge, vp);
    if (dot(normal, C) < 0.0) return false;

    edge = v2 - v1;
    vp = P - v1;
    C = cross(edge, vp);
    if (dot(normal, C) < 0.0) return false;

    edge = v0 - v2;
    vp = P - v2;
    C = cross(edge, vp);
    if (dot(normal, C) < 0.0) return false;

    intersectionPoint = P; // Сохраняем точку пересечения
    return true;
}


vec3 calculateVertexNormal(ivec2 coords) {
    // Извлечение соседних высот для расчёта градиентов
    float hCenter = texelFetch(heightMap, coords, 0).r;
    float hRight = texelFetch(heightMap, coords + ivec2(1, 0), 0).r;
    float hUp = texelFetch(heightMap, coords + ivec2(0, 1), 0).r;

    // Векторные градиенты
    vec3 gradientRight = vec3(1.0, 0.0, hRight - hCenter);
    vec3 gradientUp = vec3(0.0, 1.0, hUp - hCenter);

    // Нормаль через векторное произведение
    vec3 normal = normalize(cross(gradientRight, gradientUp));
    return normal;
}

vec3 getVertexFromHeightMap(ivec2 coords) {
    // Извлечение вершины из текстуры высот
    float x = float(coords.x);
    float y = float(coords.y);
    float z = texelFetch(heightMap, coords, 0).r * gridSize.x * 0.1f; // Значение высоты (z-координата)
    vec3 vertex = vec3(x, y, z);
    return rotationMatrix_X * rotationMatrix_Z * (vertex - center) + offset + center;
}

// Функция для кубической интерполяции по Catmull–Rom spline
vec3 cubicInterpolate(vec3 p0, vec3 p1, vec3 p2, vec3 p3, float t) {
    return 0.5 * (
         2.0 * p1 +
         (-p0 + p2) * t +
         (2.0 * p0 - 5.0 * p1 + 4.0 * p2 - p3) * t * t +
         (-p0 + 3.0 * p1 - 3.0 * p2 + p3) * t * t * t
    );
}

// Производная кубической интерполяции по Catmull–Rom spline
vec3 cubicDerivative(vec3 p0, vec3 p1, vec3 p2, vec3 p3, float t) {
    return 0.5 * (
         (-p0 + p2) +
         2.0 * (2.0 * p0 - 5.0 * p1 + 4.0 * p2 - p3) * t +
         3.0 * (-p0 + 3.0 * p1 - 3.0 * p2 + p3) * t * t
    );
}

// Функция вычисления нормали методом бикубической интерполяции.
// u и v – фракционные координаты внутри центральной ячейки [0, 1].
// Передаются все 16 вершин 4x4-пэча.
vec3 calculateBicubicNormal(
    float u, float v,
    vec3 v00, vec3 v01, vec3 v02, vec3 v03,
    vec3 v10, vec3 v11, vec3 v12, vec3 v13,
    vec3 v20, vec3 v21, vec3 v22, vec3 v23,
    vec3 v30, vec3 v31, vec3 v32, vec3 v33
) {
    // Интерполируем по x для каждого ряда (положение и производная по x)
    vec3 rowPos0 = cubicInterpolate(v00, v01, v02, v03, u);
    vec3 rowPos1 = cubicInterpolate(v10, v11, v12, v13, u);
    vec3 rowPos2 = cubicInterpolate(v20, v21, v22, v23, u);
    vec3 rowPos3 = cubicInterpolate(v30, v31, v32, v33, u);
    
    vec3 rowDer0 = cubicDerivative(v00, v01, v02, v03, u);
    vec3 rowDer1 = cubicDerivative(v10, v11, v12, v13, u);
    vec3 rowDer2 = cubicDerivative(v20, v21, v22, v23, u);
    vec3 rowDer3 = cubicDerivative(v30, v31, v32, v33, u);
    
    // Интерполируем по y для получения итоговой позиции
    vec3 pos = cubicInterpolate(rowPos0, rowPos1, rowPos2, rowPos3, v);
    
    // Вычисляем производную по y из интерполированных позиций
    vec3 dPos_dy = cubicDerivative(rowPos0, rowPos1, rowPos2, rowPos3, v);
    
    // Интерполируем ранее вычисленные производные по x вдоль y
    vec3 dPos_dx = cubicInterpolate(rowDer0, rowDer1, rowDer2, rowDer3, v);
    
    // Вычисляем нормаль как векторное произведение касательных
    vec3 normal = normalize(cross(dPos_dx, dPos_dy));
    return normal;
}

vec2 computeLocalCoords(vec3 globalPoint, vec3 v0, vec3 v1, vec3 v2, vec3 v3) {

    // 1. Расчёт нормали плоскости четырёхугольника
    vec3 edge1 = v1 - v0;
    vec3 edge2 = v3 - v0;
    vec3 normal = normalize(cross(edge1, edge2));

    // 2. Проекция точки на плоскость
    float D = -dot(normal, v0);
    float distance = dot(normal, globalPoint) + D;
    vec3 projectedPoint = globalPoint - distance * normal;

    // 3. Билинейная интерполяция для локальных координат (u, v)
    vec3 edgeU = v1 - v0; // Ось U
    vec3 edgeV = v3 - v0; // Ось V
    vec3 offset = projectedPoint - v0;

    float u = dot(offset, edgeU) / dot(edgeU, edgeU); // Нормализация U
    float v = dot(offset, edgeV) / dot(edgeV, edgeV); // Нормализация V

    // Локальные координаты (u, v)
    return vec2(u, v);
}


void main() {

    vec3 rayOrigin = vec3(gl_FragCoord.xy, 500.0); // Начало луча
    vec3 rayTarget = vec3(gl_FragCoord.xy, 0.0);   // Цель луча — плоскость треугольника
    vec3 rayDirection = normalize(rayTarget - rayOrigin); // Нормализованный вектор направления
    int detalizationLevel = 8;
    float closestT = 1e6;
    bool hit = false;
    vec3 hitNormal;
    vec3 intersectionPoint;
    vec2 intersectionPointLocal;
    vec3 a11 = getVertexFromHeightMap(ivec2(0,                  0));
    vec3 a12 = getVertexFromHeightMap(ivec2(0,                  int(gridSize.y)));
    vec3 a21 = getVertexFromHeightMap(ivec2(int(gridSize.x),    0));
    vec3 a22 = getVertexFromHeightMap(ivec2(int(gridSize.x),    int(gridSize.y)));
    vec2 intersectionPointLocalAllQuad;
    for (int y = 0; y < (int(gridSize.y) - detalizationLevel); y += detalizationLevel) {
        for (int x = 0; x < (int(gridSize.x) - detalizationLevel); x += detalizationLevel) {
            // Получение координат точек для двух треугольников в текущей ячейке

            // Собираем 16 вершин 4×4-пэча вокруг текущей ячейки
            vec3 v00 = getVertexFromHeightMap(ivec2(x - detalizationLevel,        y - detalizationLevel));
            vec3 v01 = getVertexFromHeightMap(ivec2(x,                            y - detalizationLevel));
            vec3 v02 = getVertexFromHeightMap(ivec2(x + detalizationLevel,        y - detalizationLevel));
            vec3 v03 = getVertexFromHeightMap(ivec2(x + 2 * detalizationLevel,    y - detalizationLevel));

            vec3 v10 = getVertexFromHeightMap(ivec2(x - detalizationLevel,        y));
            vec3 v11 = getVertexFromHeightMap(ivec2(x,                            y)); // Центральная точка (v0)
            vec3 v12 = getVertexFromHeightMap(ivec2(x + detalizationLevel,        y)); // (v1)
            vec3 v13 = getVertexFromHeightMap(ivec2(x + 2 * detalizationLevel,    y));

            vec3 v20 = getVertexFromHeightMap(ivec2(x - detalizationLevel,        y + detalizationLevel));
            vec3 v21 = getVertexFromHeightMap(ivec2(x,                            y + detalizationLevel)); // (v2)
            vec3 v22 = getVertexFromHeightMap(ivec2(x + detalizationLevel,        y + detalizationLevel)); // (v3)
            vec3 v23 = getVertexFromHeightMap(ivec2(x + 2 * detalizationLevel,    y + detalizationLevel));

            vec3 v30 = getVertexFromHeightMap(ivec2(x - detalizationLevel,        y + 2 * detalizationLevel));
            vec3 v31 = getVertexFromHeightMap(ivec2(x,                            y + 2 * detalizationLevel));
            vec3 v32 = getVertexFromHeightMap(ivec2(x + detalizationLevel,        y + 2 * detalizationLevel));
            vec3 v33 = getVertexFromHeightMap(ivec2(x + 2 * detalizationLevel,    y + 2 * detalizationLevel));

            float t;

            // Первый треугольник, соответствующий вершинам (v11, v12, v21)
            if (intersectTriangle(v11, v12, v21, rayOrigin, rayDirection, t, intersectionPoint)) {
                if (t < closestT) {
                    closestT = t;
                    hit = true;
                    intersectionPointLocal = computeLocalCoords(intersectionPoint, v11, v12, v22, v21);
                    intersectionPointLocalAllQuad = computeLocalCoords(intersectionPoint, a11, a12, a22, a21);
                    
                    hitNormal = calculateBicubicNormal(intersectionPointLocal.x, intersectionPointLocal.y,
                                                       v00, v01, v02, v03,
                                                       v10, v11, v12, v13,
                                                       v20, v21, v22, v23,
                                                       v30, v31, v32, v33);
                }
            }

            // Второй треугольник, соответствующий вершинам (v12, v22, v21)
            if (intersectTriangle(v12, v22, v21, rayOrigin, rayDirection, t, intersectionPoint)) {
                if (t < closestT) {
                    closestT = t;
                    hit = true;
                    intersectionPointLocal = computeLocalCoords(intersectionPoint, v11, v12, v22, v21);
                    intersectionPointLocalAllQuad = computeLocalCoords(intersectionPoint, a11, a12, a22, a21);
                    
                    hitNormal = calculateBicubicNormal(intersectionPointLocal.x, intersectionPointLocal.y,
                                                       v00, v01, v02, v03,
                                                       v10, v11, v12, v13,
                                                       v20, v21, v22, v23,
                                                       v30, v31, v32, v33);
                }
            }
        }
    }

    if (hit) {
        float brightness = min(max(dot(hitNormal, normalize(lightDirection)), 0.0), 1.0);
        vec4 baseColor = texture2D(colorMap, vec2(intersectionPointLocalAllQuad.y, intersectionPointLocalAllQuad.x));
        fragColor = vec4(baseColor.rgb * brightness, 1.0); // * brightness
    } else {
        fragColor = vec4(0.0, 0.0, 0.0, 1.0); // Нет пересечения — черный цвет
    }
}
