//
// Created by Piotr Białas on 02/11/2021.
//

#pragma once

#include <glad/gl.h>
#include <glm/glm.hpp>

#include "Application/utils.h"
#include "Engine/Material.h"

namespace xe {
    class ColorMaterial : public Material {
    public:
        ColorMaterial(const glm::vec4 color) : color_(color) {}

        void bind() final;

        void set_texture(const GLuint p_texture) {
            m_texture = p_texture;
            m_texture_uint = p_texture; }

        static void init();

        static GLuint program() { return shader_; }

    private:
        static GLuint shader_;
        static GLuint color_uniform_buffer_;

        glm::vec4 color_;


        inline static GLint m_uniform_map_Kd_location{0};
        GLuint m_texture{};
        GLuint m_texture_uint{};
    };
}
