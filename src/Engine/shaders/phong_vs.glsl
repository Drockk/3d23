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
out vec3 vertex_normal_vs;
out vec3 vertex_position_vs;

void main() {
    vertex_texcoords = a_vertex_texcoords;

    vec4 a_vertex_position_vs = VM * a_vertex_position;
    vertex_position_vs = a_vertex_position_vs.xyz/a_vertex_position_vs.w;

    vertex_normal_vs = normalize(N * a_vertex_normal);

    gl_Position =  PVM*a_vertex_position;
}
