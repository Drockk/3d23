#version 460

layout(location=0) out vec4 vFragColor;

#if __VERSION__ > 410
layout(std140, binding=0) uniform Material {
#else
    layout(std140) uniform Color {
    #endif
    vec4  Kd;
    bool use_map_Kd;
    vec3 m_ambient_color;
    vec3 m_specular_color;
    float m_specular_strength;
};

const int MAX_POINT_LIGHTS=24;

struct PointLight {
    vec3 position;
    vec3 color;
    float intensity;
    float radius;
};

layout(std140, binding=2) uniform Lights {
    vec3 ambient_color;
    uint n_p_lights;
    PointLight p_light[MAX_POINT_LIGHTS];
};

layout(std140, binding=3) uniform Camera {
    vec3 camera_position;
};

in vec2 vertex_texcoords;
in vec3 vetex_position;
in vec3 vertex_normal;

uniform sampler2D map_Kd;

void main() {
    // Ambient
    vec3 ambient = m_ambient_color;

    vec4 color;
    if (!use_map_Kd) {
        color = Kd;
    }
    else {
        color = Kd*texture(map_Kd, vertex_texcoords);
    }

    vec3 result = vec3(0.0);
    for (uint i = 0; i < n_p_lights; i++) {
        // Diffuse
        vec3 normal = normalize(vertex_normal);
        vec3 light_direction = normalize(p_light[i].position - vetex_position);
        float diff = max(dot(normal, light_direction), 0.0);
        vec3 diffuse = diff * p_light[i].color * p_light[i].intensity;

        // Specular
        vec3 view_direction = normalize(-vetex_position);
        vec3 reflect_direction = reflect(-light_direction, normal);
        float spec = pow(max(dot(view_direction, reflect_direction), 0.0), 32);
        vec3 specular = m_specular_strength * spec * m_specular_color;

        result += (ambient + diffuse + specular) * color.rgb;
    }

    vFragColor = vec4(result, 1.0);
}