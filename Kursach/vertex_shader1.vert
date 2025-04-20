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

// --- �������� ������� ---
// ����������� ��� ��� ������� � SFML: �������� ��� "position" (�������� [0,1]).
attribute vec2 position;
// ������� � UV-������������ ��������:
attribute vec2 texCoords;

// --- ����������, ������������ �� ����������� ������ ---
varying vec2 vTexCoords;   // ����� ���������� UV
varying vec3 vNormal;      // ����������� ������� ��� ���������

// �������, �������������� ������� ����� � 3D ������ ���� X � Y.
vec3 rotatePoint(vec3 pos, float ax, float ay) {
    // ������� ������ ��� X:
    float cosX = cos(ax);
    float sinX = sin(ax);
    float y = pos.y * cosX - pos.z * sinX;
    float z = pos.y * sinX + pos.z * cosX;
    pos.y = y;
    pos.z = z;
    // ������� ������ ��� Y:
    float cosY = cos(ay);
    float sinY = sin(ay);
    float x = pos.x * cosY + pos.z * sinY;
    z = -pos.x * sinY + pos.z * cosY;
    pos.x = x;
    pos.z = z;
    return pos;
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
    // ����������� �� ��� Z (����� 2.0) ����� �������������� ��� ��������/���������� ������� ������.
    vec3 normal = vec3(hL - hR, hD - hU, 2.0);
    return normalize(normal);
}

void main() {
    // ������� UV-���������� �� ����������� ������.
    vTexCoords = vec2(0,0);

    // ��������� �������� ������ �� �������� ����� colormap2 �� UV.
    float height = texture2D(colormap2, texCoords).r;

    // ��������� ������� ������� � ������� �����������.
    // "position" � ��������� [0,1] �������������� � [0,resolution].
    // ������ ���������� �� resolution.x (����� ������� ������ ����������� ��� ����������� ��������).
    vec3 pos = vec3(position * resolution, height * resolution.x);

    // ��������� ������� �� ������ uniform-��������.
    //pos = rotatePoint(pos, angleX, angleY);

    // ��������� ���: ������������ ������ �������������� ����������.
    pos.xy *= zoom;

    // ��������� ������� ������� �������� ��������� �� ���������� �����������.
    vNormal = computeNormal(texCoords);

    // ��������� �������������� �������: ����������� �������� � �������������� ���������� �������.
    gl_Position = vec4(position,1.0,1.0);
}
