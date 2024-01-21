#version 460

layout(location=0) in  vec3 a_vertex_position;

layout(std140, binding=1) uniform Transformations {
 mat4 PVM;
 };

flat out vec3 vertex_position;

void main() {
    vertex_position = a_vertex_position;
    gl_Position =  PVM * vec4(a_vertex_position, 1.0);
}
