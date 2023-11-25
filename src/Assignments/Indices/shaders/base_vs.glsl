#version 410

layout(location=0) in  vec3 a_vertex_position;
layout(location=1) in  vec3 a_vertex_color;

out vec3 in_color;

void main() {
    in_color = a_vertex_color;
    gl_Position = vec4(a_vertex_position, 1.0);
}
