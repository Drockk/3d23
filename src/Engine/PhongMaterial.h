//
// Created by Piotr Białas on 02/11/2021.
//

#pragma once

#include <glad/gl.h>
#include <glm/glm.hpp>

#include "Engine/Material.h"

struct PhongMaterialData
{
    glm::vec4 diffuse_color;
    uint32_t use_map_diffuse;
    alignas(16) glm::vec3 ambient_color;
    alignas(16)glm::vec3 specular_color;
    float specular_strength;
};

namespace xe {
    class PhongMaterial : public Material {
    public:
        PhongMaterial(const glm::vec4 color)
        {
            m_data.diffuse_color = color;
        }

        void bind() final;

        void set_texture(const GLuint p_texture) {
            m_texture = p_texture;
            m_texture_uint = p_texture;
        }

        void set_ambient(const glm::vec3& p_ambient_color)
        {
            m_data.ambient_color = p_ambient_color;
        }

        void set_specular(const glm::vec3& p_specular_color, const float p_specular_strength)
        {
            m_data.specular_color = p_specular_color;
            m_data.specular_strength = p_specular_strength;
        }

        static void init();

        static GLuint program() { return shader_; }

    private:
        static GLuint shader_;
        static GLuint color_uniform_buffer_;

        inline static GLint m_uniform_map_Kd_location{0};
        GLuint m_texture{};
        GLuint m_texture_uint{};

        PhongMaterialData m_data;
    };
}
