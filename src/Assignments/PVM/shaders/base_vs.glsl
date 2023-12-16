#version 460

layout(location=0) in  vec4 a_vertex_position;

layout(std140, binding=1) uniform Transformations {
 mat4 PVM;
 };

void main() {
    gl_Position =  PVM * a_vertex_position;
}
