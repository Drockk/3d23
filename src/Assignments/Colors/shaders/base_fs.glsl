#version 410

layout(location=0) out vec4 vFragColor;

in vec3 in_color;

void main() {
    vFragColor = vec4(in_color, 1.0f);
    // vFragColor = vec4(0.5, 0.5, 0.5, 1.0);
}
