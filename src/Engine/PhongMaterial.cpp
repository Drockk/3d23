//
// Created by Piotr Białas on 02/11/2021.
//
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/string_cast.hpp"

#include "Engine/PhongMaterial.h"

#include "Application/utils.h"
#include "spdlog/spdlog.h"
#include "stb/stb_image.h"

namespace xe {

    GLuint PhongMaterial::color_uniform_buffer_ = 0u;
    GLuint PhongMaterial::shader_ = 0u;

    void PhongMaterial::bind() {
        m_data.use_map_diffuse = 0;
        if (m_texture > 0) {
            glUniform1i(m_uniform_map_Kd_location, m_texture_uint);
            glActiveTexture(GL_TEXTURE0 + m_texture_uint);
            glBindTexture(GL_TEXTURE_2D, m_texture);
            m_data.use_map_diffuse = 1;
        }

        glBindBufferBase(GL_UNIFORM_BUFFER, 0, color_uniform_buffer_);
        glUseProgram(program());
        glBindBuffer(GL_UNIFORM_BUFFER, color_uniform_buffer_);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(PhongMaterialData), &m_data);
        glBindBuffer(GL_UNIFORM_BUFFER, 0u);
    }


    void PhongMaterial::init() {
        auto program = xe::utils::create_program(
                {{GL_VERTEX_SHADER,   std::string(PROJECT_DIR) + "/shaders/phong_vs.glsl"},
                 {GL_FRAGMENT_SHADER, std::string(PROJECT_DIR) + "/shaders/phong_fs.glsl"}});
        if (!program) {
            std::cerr << "Invalid program\n";
            exit(-1);
        }

        shader_ = program;

        glGenBuffers(1, &color_uniform_buffer_);

        glBindBuffer(GL_UNIFORM_BUFFER, color_uniform_buffer_);
        glBufferData(GL_UNIFORM_BUFFER, sizeof(PhongMaterialData), nullptr, GL_STATIC_DRAW);
        glBindBuffer(GL_UNIFORM_BUFFER, 0u);
#ifdef __APPLE__
        auto u_modifiers_index = glGetUniformBlockIndex(program, "Color");
        if (u_modifiers_index == -1) {
            std::cerr << "Cannot find Color uniform block in program" << std::endl;
        } else {
            glUniformBlockBinding(program, u_modifiers_index, 0);
        }
#endif

#ifdef __APPLE__
        auto u_transformations_index = glGetUniformBlockIndex(program, "Transformations");
        if (u_transformations_index == -1) {
            std::cerr << "Cannot find Transformations uniform block in program" << std::endl;
        } else {
            glUniformBlockBinding(program, u_transformations_index, 1);
        }
#endif

        m_uniform_map_Kd_location = glGetUniformLocation(program, "map_Kd");
        if (m_uniform_map_Kd_location == -1) {
            std::cerr << "Cannot get uniform map_Kd location\n";
        }
    }
}
