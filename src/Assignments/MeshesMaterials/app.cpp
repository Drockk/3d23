#include "app.h"

#include <Engine/mesh_loader.h>

#include <array>
#include <vector>

#include <glm/gtc/type_ptr.hpp>

#define STB_IMAGE_IMPLEMENTATION 1
#include "3rdParty/stb/stb_image.h"

#include <spdlog/spdlog.h>

void SimpleShapeApplication::init() {
    xe::ColorMaterial::init();

    auto mesh_path = std::string(ROOT_DIR) + "/Models";
    auto mesh = xe::load_mesh_from_obj(mesh_path + "/blue_marble.obj", mesh_path);
    m_meshes.emplace_back(mesh);

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
    constexpr glm::vec3 camera_position = { 5.0f, 5.0f, 5.0f };
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

    for (const auto mesh: m_meshes)
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
