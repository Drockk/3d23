#version 460

layout(location=0) in vec4 a_vertex_position;
layout(location=1) in vec2 a_vertex_texcoords;
layout(location=5) in vec3 a_vertex_normal;

#if __VERSION__ > 410
layout(std140, binding=1) uniform Transformations {
#else
    layout(std140) uniform Transformations {
    #endif
    mat4 PVM;
    mat4 VM;
    mat3 N;
};

out vec2 vertex_texcoords;
out vec3 vetex_position;
out vec3 vertex_normal;

void main() {
    vertex_texcoords = a_vertex_texcoords;

    vetex_position = vec3(VM * a_vertex_position);
    vertex_normal = N * a_vertex_normal;

    gl_Position =  PVM * a_vertex_position;
}
