#version 330 core

out vec4 fragColor;

void main() {
    // ���������� ����������� � ������� ���������
    vec3 v0 = vec3(100, 100, 0);
    vec3 v1 = vec3(300, 100, 0);
    vec3 v2 = vec3(200, 400, 0);

    // ��������� ���� �� ������ �������� ���������
    vec3 rayOrigin = vec3(gl_FragCoord.xy, 500.0); // ������ ����
    vec3 rayTarget = vec3(gl_FragCoord.xy, 0.0);   // ���� ���� � ��������� ������������
    vec3 rayDirection = normalize(rayTarget - rayOrigin); // ��������������� ������ �����������

    // ������ ������� ��������� ������������
    vec3 edge1 = v1 - v0;
    vec3 edge2 = v2 - v0;
    vec3 normal = cross(edge1, edge2);

    // �������� �� ��������������
    float nDotD = dot(normal, rayDirection);
    if (abs(nDotD) < 1e-6) {
        fragColor = vec4(1.0, 0.0, 0.0, 1.0); // ��� ����������� � ������� ����
        return;
    }

    // ���������� t ��� �����������
    float t = dot(normal, v0 - rayOrigin) / nDotD;
    if (t < 0.0) {
        fragColor = vec4(0.0, 1.0, 0.0, 1.0); // ��� �� ���������� ����������� � ������� ����
        return;
    }

    // ����� �����������
    vec3 P = rayOrigin + t * rayDirection;

    // ��������, ��������� �� ����� P ������ ������������
    vec3 C;

    // ������ AB
    vec3 edge0 = v1 - v0;
    vec3 vp0 = P - v0;
    C = cross(edge0, vp0);
    if (dot(normal, C) < 0.0) {
        fragColor = vec4(0.0, 0.0, 1.0, 1.0); // �� ��������� ������������ � ����� ����
        return;
    }

    // ������ BC
    edge1 = v2 - v1;
    vec3 vp1 = P - v1;
    C = cross(edge1, vp1);
    if (dot(normal, C) < 0.0) {
        fragColor = vec4(0.0, 0.0, 1.0, 1.0); // �� ��������� ������������ � ����� ����
        return;
    }

    // ������ CA
    edge2 = v0 - v2;
    vec3 vp2 = P - v2;
    C = cross(edge2, vp2);
    if (dot(normal, C) < 0.0) {
        fragColor = vec4(0.0, 0.0, 1.0, 1.0); // �� ��������� ������������ � ����� ����
        return;
    }

    // ����� P ����� ������ ������������
    fragColor = vec4(1.0, 1.0, 0.0, 1.0); // ����������� ������� � ������ ����
}
