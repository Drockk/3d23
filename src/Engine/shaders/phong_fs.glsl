#version 460

layout(location=0) out vec4 vFragColor;

#if __VERSION__ > 410
layout(std140, binding=0) uniform Modifiers {
#else
    layout(std140) uniform Color {
    #endif
    vec4  Kd;
    bool use_map_Kd;
};

const int MAX_POINT_LIGHTS=24;

struct PointLight {
    vec3 position_in_view_space;
    vec3 color;
    float intensity;
    float radius;
};

layout(std140, binding=2) uniform Lights {
    vec3 ambient;
    uint n_p_lights;
    PointLight p_light[MAX_POINT_LIGHTS];
};

#define INV_PI 1/3.14

in vec2 vertex_texcoords;
in vec3 vertex_normal_vs;
in vec3 vertex_position_vs;

uniform sampler2D map_Kd;

void main() {
    vec3 normal = normalize(vertex_normal_vs);
    if (!gl_FrontFacing) {
        normal = -normal;
    }

    vec4 color;
    if (!use_map_Kd) {
        color = Kd;
    }
    else {
        color = Kd*texture(map_Kd, vertex_texcoords);
    }

    vFragColor.a = 1.0;

    for (uint i = 0; i < n_p_lights; i++) {
        vec3 lightDir = normalize(p_light[i].position_in_view_space - vertex_position_vs);
        float diff = max(dot(normal, lightDir), 0.0);
        vec3 diffuse = diff * p_light[i].color;

        vFragColor.rgb += INV_PI * color.rgb * p_light[i].color * p_light[i].intensity * diffuse;
    }
}