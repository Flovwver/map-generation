#version 120

// --- Uniform-���������� ---
// �������� � ��������: ������ ������������ � ���������� R.
uniform sampler2D colormap2;

// ����������� ��������������� (���).
uniform float zoom;

// ���� �������� ������ ���� X � Y (���������� �� ����������).
uniform float angleX;
uniform float angleY;

// ������ ������� (��� ������), ������������ ��� �������������� [0,1]>[0, resolution].
uniform vec2 resolution;

uniform float time;

// --- Uniform-���������� ---
// �������� �������� �����: ����� �������������� ��� ��������� ��������� ����.
uniform sampler2D colormap;

// ����������� ��������� ����� (������ ���� �������������).
uniform vec3 lightDir;

// --- ����������, ���������� �� ���������� ������� ---
//varying vec2 vTexCoords;   // UV-���������� ��� ������� �������� �����
//varying vec3 vNormal;      // �������, ����������� � ��������� �������

vec3 computeNormal(vec2 uv) {
    // ������ ����� ��� ������� �������� ��������.
    float offset = 1.0 / resolution.x;
    // ��������� ������ �������� ��������.
    float hL = texture2D(colormap2, uv - vec2(offset, 0.0)).r;
    float hR = texture2D(colormap2, uv + vec2(offset, 0.0)).r;
    float hD = texture2D(colormap2, uv - vec2(0.0, offset)).r;
    float hU = texture2D(colormap2, uv + vec2(0.0, offset)).r;
    // ����� ������ ������� ���������� �� ��������� (hL - hR) � (hD - hU).
    // ����������� �� ��� Z (����� 2.0) ����� �������������� ��� ��������/���������� ������� ������.
    vec3 normal = vec3(hL - hR, hD - hU, 0.01f);//100.f);//
    return normalize(normal);
}

vec2 rotate(vec2 point, float phi, vec2 center) {
    vec2 newPoint;
    newPoint.x = (point.x - center.x) * cos(phi) - (point.y - center.y) * sin(phi) + center.x;
    newPoint.y = (point.x - center.x) * sin(phi) + (point.y - center.y) * cos(phi) + center.y;
    return newPoint;
}

vec3 rotate(vec3 vector, float phi) {
    vec3 newVector;
    mat3 translation;
    translation[0] = vec3(cos(phi),-sin(phi),0.f);
    translation[1] = vec3(sin(phi),cos(phi),0.f);
    translation[2] = vec3(0.f,0.f,1.f);
    newVector = vector * translation;
    return newVector;
}

void main() {
    // ��������� ������� ���� �� �������� colormap.
	vec2 xy = (gl_FragCoord.xy / resolution / zoom);
    float fragTime = time / 10000000.f;
    fragTime = 0;
    vec2 center = vec2(0.5f, 0.5f);
    xy = rotate(xy, fragTime, center);
    vec3 norm = computeNormal(xy);
    vec3 lightDir = normalize(vec3(-0.5f, 0.5f, 0.f));
    norm = rotate(norm, -fragTime);
    float diffuse = sqrt((dot(norm, lightDir) + 1.f) / 2.f);
    vec4 baseColor = texture2D(colormap, xy) * diffuse;
    baseColor.a = 1.f;


    gl_FragColor = baseColor;
}
