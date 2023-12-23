#include "app.h"

#include <array>
#include <iostream>
#include <vector>
#include <tuple>

#include "Application/utils.h"
#include <glm/gtc/type_ptr.hpp>

void SimpleShapeApplication::init() {
    // A utility function that reads the shader sources, compiles them and creates the program object
    // As everything in OpenGL we reference program by an integer "handle".
    const auto program = xe::utils::create_program(
            {{GL_VERTEX_SHADER, std::string(PROJECT_DIR) + "/shaders/base_vs.glsl"},
             {GL_FRAGMENT_SHADER, std::string(PROJECT_DIR) + "/shaders/base_fs.glsl"}});

    if (!program) {
        std::cerr << "Invalid program\n";
        std::terminate();
    }

    // A vector containing the x,y,z vertex coordinates for the triangle.
    const std::vector vertices = {
        -0.5f, 0.0f, -0.5f, // 0
         0.5f, 0.0f,  0.5f, // 1
        -0.5f, 0.0f,  0.5f, // 2
         0.5f, 0.0f, -0.5f, // 3
         0.0f, 1.0f,  0.0f  // 4
    };

    // Indices
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    m_indices = std::vector<GLushort> {
        1, 2, 0, // Floor 1
        3, 1, 0, // Floor 2
        0, 2, 4, // Wall 1
        1, 4, 2, // Wall 2
        3, 4, 1, // Wall 3
        0, 4, 3  // Wall 4
    };

    GLuint i_buffer_handle{};
    glGenBuffers(1, &i_buffer_handle);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, i_buffer_handle);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<GLsizeiptr>(m_indices.size()) * sizeof(GLushort), m_indices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    // Uniforms
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///VERTEX SHADER
    glGenBuffers(1, &m_uniform_vertex_buffer);
    glBindBuffer(GL_UNIFORM_BUFFER, m_uniform_vertex_buffer);
    glBufferData(GL_UNIFORM_BUFFER,  sizeof(glm::mat4), nullptr, GL_STATIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    glBindBufferBase(GL_UNIFORM_BUFFER, 1, m_uniform_vertex_buffer);
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    // Generating the buffer and loading the vertex data into it.
    GLuint v_buffer_handle{};
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

    //Calculate projection
    recalculate_projection();

    // Calculate view
    constexpr glm::vec3 camera_position = { 0.0f, 2.0f, 2.0f };
    constexpr glm::vec3 camera_target = { 0.0f, 0.0f, 0.0f };
    constexpr glm::vec3 camera_up = { 0.0f, 1.0f, 0.0f };

    m_view = lookAt(camera_position, camera_target, camera_up);

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
    //Calculate PVM matrix
    constexpr glm::mat4 model(1.0f);
    const auto pvm = m_projection * m_view * model;
    glBindBuffer(GL_UNIFORM_BUFFER, m_uniform_vertex_buffer);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), value_ptr(pvm));
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    // Binding the VAO will setup all the required vertex buffers.
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glBindVertexArray(vao_);
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(m_indices.size()), GL_UNSIGNED_SHORT, nullptr);
    glBindVertexArray(0);
}

void SimpleShapeApplication::framebuffer_resize_callback(int w, int h) {
    Application::framebuffer_resize_callback(w, h);

    glViewport(0, 0, w, h);
    recalculate_projection(static_cast<float>(w), static_cast<float>(h));
}

void SimpleShapeApplication::recalculate_projection() {
    const auto& [width, height] = frame_buffer_size();
    recalculate_projection(static_cast<float>(width), static_cast<float>(height));
}

void SimpleShapeApplication::recalculate_projection(float width, float height) {
    m_aspect = width / height;
    m_projection = glm::perspective(glm::radians(m_fov), m_aspect, m_near, m_far);
}
