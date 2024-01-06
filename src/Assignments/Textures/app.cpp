#include "app.h"

#include <array>
#include <iostream>
#include <vector>

#include "Application/utils.h"
#include <glm/gtc/type_ptr.hpp>

#define STB_IMAGE_IMPLEMENTATION  1
#include "3rdParty/stb/stb_image.h"

#include <spdlog/spdlog.h>

void SimpleShapeApplication::init() {
    //Load texture
    load_image(std::string(ROOT_DIR) + "/Models/multicolor.png");
    //Material
    xe::ColorMaterial::init();

    // Vertices
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    const std::vector vertices = {
         // Positions       // Texture Coordinates
        -0.5f, 0.0f, -0.5f, 0.1910f, 0.5000f, // 0
         0.5f, 0.0f,  0.5f, 0.8090f, 0.5000f, // 1
        -0.5f, 0.0f,  0.5f, 0.5000f, 0.8090f, // 2
         0.5f, 0.0f, -0.5f, 0.5000f, 0.1910f, // 3
         0.0f, 1.0f,  0.0f, 0.0000f, 1.0000f, // 4
         0.0f, 1.0f,  0.0f, 1.0000f, 1.0000f, // 5
         0.0f, 1.0f,  0.0f, 1.0000f, 0.0000f, // 6
         0.0f, 1.0f,  0.0f, 0.0000f, 0.0000f, // 7
    };

    GLuint v_buffer_handle{};
    glGenBuffers(1, &v_buffer_handle);
    OGL_CALL(glBindBuffer(GL_ARRAY_BUFFER, v_buffer_handle));
    glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(vertices.size()) * sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Indices
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    m_indices = std::vector<GLushort> {
        1, 2, 0, // Floor 1
        3, 1, 0, // Floor 2
        0, 2, 4, // Wall 1
        2, 1, 5, // Wall 2
        1, 3, 6, // Wall 3
        3, 0, 7  // Wall 4
    };

    GLuint i_buffer_handle{};
    glGenBuffers(1, &i_buffer_handle);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, i_buffer_handle);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<GLsizeiptr>(m_indices.size()) * sizeof(GLushort), m_indices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // Uniforms
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///VERTEX SHADER
    glGenBuffers(1, &m_uniform_vertex_buffer);
    glBindBuffer(GL_UNIFORM_BUFFER, m_uniform_vertex_buffer);
    glBufferData(GL_UNIFORM_BUFFER,  sizeof(glm::mat4), nullptr, GL_STATIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    glBindBufferBase(GL_UNIFORM_BUFFER, 1, m_uniform_vertex_buffer);
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    // This setups a Vertex Array Object (VAO) that  encapsulates
    // the state of all vertex buffers needed for rendering
    glGenVertexArrays(1, &vao_);
    glBindVertexArray(vao_);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, i_buffer_handle);

    // This indicates that the data for attribute 0 should be read from a vertex buffer.
    // and this specifies how the data is layout in the buffer.
    glBindBuffer(GL_ARRAY_BUFFER, v_buffer_handle);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), nullptr);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), reinterpret_cast<GLvoid*>(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    //Calculate projection
    const auto& [width, height] = frame_buffer_size();
    m_camera.perspective(glm::pi<float>() / 4.0f, width / height, 0.1f, 100.0f);

    // Calculate view
    constexpr glm::vec3 camera_position = { 0.0f, -2.0f, 1.0f };
    constexpr glm::vec3 camera_target = { 0.0f, 0.0f, 0.0f };
    constexpr glm::vec3 camera_up = { 0.0f, 1.0f, 0.0f };

    m_camera.look_at(camera_position, camera_target, camera_up);
    m_camera_controler.set_camera(&m_camera);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    //end of vao "recording"

    // Setting the background color of the rendering window,
    // I suggest not to use white or black for better debuging.
    glClearColor(0.81f, 0.81f, 0.8f, 1.0f);

    // This setups an OpenGL viewport of the size of the whole rendering window.
    auto[w, h] = frame_buffer_size();
    glViewport(0, 0, w, h);

    glUseProgram(xe::ColorMaterial::program());
}

//This functions is called every frame and does the actual rendering.
void SimpleShapeApplication::frame() {
    //Materials
    m_color_material.bind();

    //Calculate PVM matrix
    constexpr glm::mat4 model(1.0f);
    const auto& view = m_camera.view();
    const auto& projection = m_camera.projection();
    const auto pvm = projection * view * model;
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

void SimpleShapeApplication::framebuffer_resize_callback(const int w, const int h) {
    Application::framebuffer_resize_callback(w, h);

    glViewport(0, 0, w, h);
    m_camera.set_aspect(w / h);
}

void SimpleShapeApplication::scroll_callback(double xoffset, double yoffset) {
    Application::scroll_callback(xoffset, yoffset);
    m_camera.zoom(yoffset / 30.0f);
}

void SimpleShapeApplication::mouse_button_callback(int button, int action, int mods) {
    Application::mouse_button_callback(button, action, mods);

    double x, y;
    glfwGetCursorPos(window_, &x, &y);

    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
        m_camera_controler.lmb_pressed(x, y);

    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
        m_camera_controler.lmb_released(x, y);
}

void SimpleShapeApplication::cursor_position_callback(double x, double y) {
    Application::cursor_position_callback(x, y);

    m_camera_controler.mouse_moved(x, y);
}

void SimpleShapeApplication::load_image(const std::string& p_path) {
    stbi_set_flip_vertically_on_load(true);
    GLint width{}, height{}, channels{};
    const auto img = stbi_load(p_path.c_str(), &width, &height, &channels, 0);
    if (!img) {
        spdlog::warn("Could not read image from file `{}'", p_path);
    }

    GLuint texture{};
    glGenTextures(1, &texture);

    glBindTexture(GL_TEXTURE_2D, texture);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, img);
    glGenerateMipmap(GL_TEXTURE_2D);

    m_color_material.set_texture(texture);
}
