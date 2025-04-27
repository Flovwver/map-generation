#version 120

// --- Uniform-���������� ---
// �������� � ��������: ������ ������������ � ���������� R.
uniform sampler2D colormap2;

// ����������� ��������������� (���).
uniform float zoom;

// ���� �������� ������ ���� X � Y (���������� �� ����������).
uniform float angleX;
uniform float angleY;

// ������ ������� (��� ������)
uniform vec2 resolution;

// --- �������� ������� ---
// ����������� ��� ��� ������� � SFML: �������� ��� "position" (�������� [0,1]).
attribute vec3 position;

// ������� � UV-������������ ��������:
attribute vec2 texCoords;

// --- ����������, ������������ �� ����������� ������ ---
varying vec2 vTexCoords;   // ����� ���������� UV
varying vec3 vNormal;      // ����������� ������� ��� ���������

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


// ������� ��������� �������, ��������� ����������� �������� �� ���������� �����������.
// ����� �� ������������, ��� ���������� �������� (��� �������) ��������� � uniform `resolution`.
vec3 computeNormal(vec2 uv) {
    // ������ ����� ��� ������� �������� ��������.
    float offset = 1.0 / resolution.x;
    // ��������� ������ �������� ��������.
    float hL = texture2D(colormap2, uv - vec2(offset, 0.0)).r;
    float hR = texture2D(colormap2, uv + vec2(offset, 0.0)).r;
    float hD = texture2D(colormap2, uv - vec2(0.0, offset)).r;
    float hU = texture2D(colormap2, uv + vec2(0.0, offset)).r;
    // ����� ������ ������� ���������� �� ��������� (hL - hR) � (hD - hU).
    vec3 normal = vec3(hL - hR, hD - hU, 0.01f);
    return normalize(normal);
}

void main() {
    // ������� UV-���������� �� ����������� ������.
    gl_TexCoord[0] = gl_TextureMatrix[0] * gl_MultiTexCoord0;

    // ��������� ������� ������� �������� ��������� �� ���������� �����������.
    vNormal = computeNormal(texCoords);

    // ����������� ���������� �������.
    vec4 pos = gl_Vertex;

    // ��������� ������� �� ������ uniform-��������.
    pos = rotate(pos, angleX, angleY);

    // ������������ ���������� ����� ��������.
    pos.xy *= zoom;

    // ����������� � ������������� ���������.
    gl_Position = gl_ModelViewProjectionMatrix * pos;
}
