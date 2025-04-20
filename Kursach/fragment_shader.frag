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


// --- ����������, ������������ �� ����������� ������ ---
varying vec2 vTexCoords;   // ����� ���������� UV
varying vec3 vNormal;      // ����������� ������� ��� ���������

// --- Uniform-���������� ---
// �������� �������� �����: ����� �������������� ��� ��������� ��������� ����.
uniform sampler2D colormap;

// ����������� ��������� ����� (������ ���� �������������).
uniform vec3 lightDir;


void main() {
    // ��������� ������� ���� �� �������� colormap.
    vec4 baseColor = texture2D(colormap, vTexCoords);

    // ����������� ������� � ����������� �����.
    vec3 norm = normalize(vNormal);
    vec3 light = normalize(lightDir);

    // ��������� ��������� ��������� ����� ��������� ������������.
    // ������� max �����������, ��� �������� �� ����� �������������.
    float diffuse = (dot(norm, light) + 1.f) / 2.f;

    // �������� ���� ���������� ���� ��������� �������� ����� �� ����������� ������������.
    vec3 finalColor = baseColor.rgb * diffuse;

    gl_FragColor = vec4(finalColor, 1.0);
}
