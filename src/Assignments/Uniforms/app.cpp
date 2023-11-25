//
// Created by pbialas on 25.09.2020.
//

#include "app.h"

#include <array>
#include <iostream>
#include <vector>
#include <tuple>

#include "Application/utils.h"
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/type_ptr.hpp>

void SimpleShapeApplication::init() {
    // A utility function that reads the shader sources, compiles them and creates the program object
    // As everything in OpenGL we reference program by an integer "handle".
    const auto program = xe::utils::create_program(
            {{GL_VERTEX_SHADER,   std::string(PROJECT_DIR) + "/shaders/base_vs.glsl"},
             {GL_FRAGMENT_SHADER, std::string(PROJECT_DIR) + "/shaders/base_fs.glsl"}});

    if (!program) {
        std::cerr << "Invalid program" << std::endl;
        exit(-1);
    }

    // A vector containing the x,y,z vertex coordinates for the triangle.
    const std::vector vertices = {
        -0.5f,  0.0f, 0.0f, // 0
         0.5f,  0.0f, 0.0f, // 1
         0.0f,  0.5f, 0.0f, // 2
        -0.5f, -0.5f, 0.0f, // 3
         0.5f, -0.5f, 0.0f, // 4
    };

    // Colors
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    const std::vector colors = {
        1.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f
    };

    GLuint color_buffer_handle{};
    glGenBuffers(1, &color_buffer_handle);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, color_buffer_handle);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<GLsizeiptr>(colors.size()) * sizeof(GLfloat), colors.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    // Indices
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    const std::vector<GLushort> indices = {
        0, 1, 2, // Triangle 1
        0, 1, 3, // Triangle 2
        1, 3, 4  // Triangle 3
    };

    GLuint i_buffer_handle{};
    glGenBuffers(1, &i_buffer_handle);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, i_buffer_handle);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<GLsizeiptr>(indices.size()) * sizeof(GLushort), indices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    // Uniforms
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///FRAGMENT SHADER
    GLuint uniform_fragment_buffer_handle{};
    glGenBuffers(1, &uniform_fragment_buffer_handle);
    glBindBuffer(GL_UNIFORM_BUFFER, uniform_fragment_buffer_handle);
    glBufferData(GL_UNIFORM_BUFFER, 8 * sizeof(GLfloat), nullptr, GL_STATIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, uniform_fragment_buffer_handle);

    ///VERTEX SHADER
    GLuint uniform_vertex_buffer_handle{};
    glGenBuffers(1, &uniform_vertex_buffer_handle);
    glBindBuffer(GL_UNIFORM_BUFFER, uniform_vertex_buffer_handle);
    glBufferData(GL_UNIFORM_BUFFER, 12 * sizeof(GLfloat), nullptr, GL_STATIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    glBindBufferBase(GL_UNIFORM_BUFFER, 1, uniform_vertex_buffer_handle);
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    // Generating the buffer and loading the vertex data into it.
    GLuint v_buffer_handle;
    glGenBuffers(1, &v_buffer_handle);
    OGL_CALL(glBindBuffer(GL_ARRAY_BUFFER, v_buffer_handle));
    glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(vertices.size()) * sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // This setups a Vertex Array Object (VAO) that  encapsulates
    // the state of all vertex buffers needed for rendering
    glGenVertexArrays(1, &vao_);
    glBindVertexArray(vao_);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, i_buffer_handle);

    // This indicates that the data for attribute 0 should be read from a vertex buffer.
    // and this specifies how the data is layout in the buffer.
    glBindBuffer(GL_ARRAY_BUFFER, v_buffer_handle);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(0);

    // Colors
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    glBindBuffer(GL_ARRAY_BUFFER, color_buffer_handle);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(1);
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    // Uniforms
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///FRAGMENT SHADER
    {
        constexpr auto strength{ 0.5f };
        constexpr std::array<float, 3> color = { 1.0f, 0.5f, 1.0f };
        constexpr auto float_size = sizeof(float);

        glBindBuffer(GL_UNIFORM_BUFFER, uniform_fragment_buffer_handle);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, float_size, &strength);
        glBufferSubData(GL_UNIFORM_BUFFER, 4 * float_size, color.size() * float_size, color.data());
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
    }
    ///VERTEX SHADER
    {
        float theta = 1.0 * glm::pi<float>() / 6.0f;
        auto cs = std::cos(theta);
        auto ss = std::sin(theta);
        glm::mat2 rot{ cs,ss,-ss,cs };
        glm::vec2 trans{ 0.0,  -0.25 };
        glm::vec2 scale{ 0.5, 0.5 };

        glBindBuffer(GL_UNIFORM_BUFFER, uniform_vertex_buffer_handle);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::vec2), value_ptr(scale));
        glBufferSubData(GL_UNIFORM_BUFFER, 2 * sizeof(float), sizeof(glm::vec2), value_ptr(trans));
        glBufferSubData(GL_UNIFORM_BUFFER, 4 * sizeof(float), sizeof(glm::mat2), value_ptr(rot));
        glBufferSubData(GL_UNIFORM_BUFFER, 6 * sizeof(float), sizeof(glm::mat2), value_ptr(rot));
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
    }
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    //end of vao "recording"

    // Setting the background color of the rendering window,
    // I suggest not to use white or black for better debuging.
    glClearColor(0.81f, 0.81f, 0.8f, 1.0f);

    // This setups an OpenGL vieport of the size of the whole rendering window.
    auto[w, h] = frame_buffer_size();
    glViewport(0, 0, w, h);

    glUseProgram(program);
}

//This functions is called every frame and does the actual rendering.
void SimpleShapeApplication::frame() {
    // Binding the VAO will setup all the required vertex buffers.
    glBindVertexArray(vao_);
    glDrawElements(GL_TRIANGLES, 9, GL_UNSIGNED_SHORT, nullptr);
    glBindVertexArray(0);
}
