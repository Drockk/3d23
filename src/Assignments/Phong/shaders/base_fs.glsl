#version 460

layout(location=0) out vec4 vFragColor;

layout(std140, binding=0) uniform Color {
    vec4  Kd;
    bool use_map_Kd;
};

flat in vec3 vertex_position;

void main() {
    vFragColor = vec4(vertex_position, 1.0);
}
