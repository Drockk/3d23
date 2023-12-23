#version 460

layout(location=0) out vec4 vFragColor;

flat in vec3 vertex_position;

void main() {
    vFragColor = vec4(vertex_position, 1.0);
}
