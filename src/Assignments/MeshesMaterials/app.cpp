#include "app.h"

#include <array>
#include <vector>

#include "Application/utils.h"
#include <glm/gtc/type_ptr.hpp>

#define STB_IMAGE_IMPLEMENTATION  1
#include "3rdParty/stb/stb_image.h"

#include <spdlog/spdlog.h>

void SimpleShapeApplication::init() {
    xe::ColorMaterial::init();

    std::vector vertices = {
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

    std::vector<GLushort> indices = {
    1, 2, 0, // Floor 1
    3, 1, 0, // Floor 2
    0, 2, 4, // Wall 1
    2, 1, 5, // Wall 2
    1, 3, 6, // Wall 3
    3, 0, 7  // Wall 4
    };

    auto* pyramid = new xe::Mesh;
    pyramid->allocate_vertex_buffer(vertices.size() * sizeof(GLfloat), GL_STATIC_DRAW);
    pyramid->load_vertices(0, vertices.size() * sizeof(GLfloat), vertices.data());

    pyramid->vertex_attrib_pointer(0, 3, GL_FLOAT, 5 * sizeof(GLfloat), 0);
    pyramid->vertex_attrib_pointer(1, 2, GL_FLOAT, 5 * sizeof(GLfloat), 3 * sizeof(GLfloat));

    pyramid->allocate_index_buffer(indices.size() * sizeof(GLushort), GL_STATIC_DRAW);
    pyramid->load_indices(0, indices.size() * sizeof(GLushort), indices.data());

    pyramid->add_submesh(0, indices.size(), &m_color_material);

    add_submesh(pyramid);

    //Load texture
    load_image(std::string(ROOT_DIR) + "/Models/multicolor.png");

    // Uniforms
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///VERTEX SHADER
    glGenBuffers(1, &m_uniform_vertex_buffer);
    glBindBuffer(GL_UNIFORM_BUFFER, m_uniform_vertex_buffer);
    glBufferData(GL_UNIFORM_BUFFER,  sizeof(glm::mat4), nullptr, GL_STATIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    glBindBufferBase(GL_UNIFORM_BUFFER, 1, m_uniform_vertex_buffer);
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    //Calculate projection
    const auto& [width, height] = frame_buffer_size();
    m_camera.perspective(glm::pi<float>() / 4.0f, width / height, 0.1f, 100.0f);

    // Calculate view
    constexpr glm::vec3 camera_position = { 0.0f, 2.0f, 1.0f };
    constexpr glm::vec3 camera_target = { 0.0f, 0.0f, 0.0f };
    constexpr glm::vec3 camera_up = { 0.0f, 1.0f, 0.0f };

    m_camera.look_at(camera_position, camera_target, camera_up);
    m_camera_controler.set_camera(&m_camera);

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

    for (const auto* mesh: m_meshes)
    {
        mesh->draw();
    }
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

void SimpleShapeApplication::add_submesh(xe::Mesh* p_mesh)
{
    m_meshes.emplace_back(p_mesh);
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
