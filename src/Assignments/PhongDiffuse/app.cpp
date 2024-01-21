#include "app.h"

#include <Engine/ColorMaterial.h>
#include <Engine/PhongMaterial.h>
#include <Engine/mesh_loader.h>

#include <array>
#include <vector>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/type_ptr.hpp>

#define STB_IMAGE_IMPLEMENTATION 1
#include "3rdParty/stb/stb_image.h"

#include <spdlog/spdlog.h>

constexpr auto MAX_POINT_LIGHTS = 24;

struct Lights
{
    glm::vec3 ambient;
    uint32_t n_p_lights;
    xe::PointLight p_lights[MAX_POINT_LIGHTS];
};

struct Transformations
{
    glm::mat4 PVM;
    alignas(16) glm::mat4 VM;
    alignas(16) glm::mat3 N;
};

void SimpleShapeApplication::init() {
    xe::ColorMaterial::init();
    xe::PhongMaterial::init();

    auto mesh_path = std::string(ROOT_DIR) + "/Models";
    auto mesh = xe::load_mesh_from_obj(mesh_path + "/cube.obj", mesh_path);
    m_meshes.emplace_back(mesh);

    // Uniforms
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///VERTEX SHADER
    glGenBuffers(1, &m_uniform_vertex_buffer);
    glBindBuffer(GL_UNIFORM_BUFFER, m_uniform_vertex_buffer);
    glBufferData(GL_UNIFORM_BUFFER,  sizeof(Transformations), nullptr, GL_STATIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    glBindBufferBase(GL_UNIFORM_BUFFER, 1, m_uniform_vertex_buffer);

    ///Lights uniform
    glGenBuffers(1, &m_uniform_lights_buffer);
    glBindBuffer(GL_UNIFORM_BUFFER, m_uniform_lights_buffer);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(Lights), nullptr, GL_STATIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    glBindBufferBase(GL_UNIFORM_BUFFER, 2, m_uniform_lights_buffer);
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    // Add ambient color
    add_ambient({ 0.25f, 0.25f, 0.25f });
    // Add lights
    const auto light = xe::PointLight({ 0.0, 4.0, 0.0 }, { 0.0, 1.0, 0.0 }, 5.0f, 0.1f);
    add_light(light);

    //Calculate projection
    const auto& [width, height] = frame_buffer_size();
    m_camera.perspective(glm::pi<float>() / 4.0f, width / height, 0.1f, 100.0f);

    // Calculate view
    constexpr glm::vec3 camera_position = { 5.0f, 10.0f, 5.0f };
    constexpr glm::vec3 camera_target = { 0.0f, 0.5f, 0.0f };
    constexpr glm::vec3 camera_up = { 1.0f, 0.0f, 0.0f };

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
    const auto vm = view * model;
    const auto r = glm::mat3(vm);
    const auto n = glm::mat3(cross(r[1], r[2]), cross(r[2], r[0]), cross(r[0], r[1]));

    Transformations transformations;
    transformations.PVM = pvm;
    transformations.VM = vm;
    transformations.N = n;

    glBindBuffer(GL_UNIFORM_BUFFER, m_uniform_vertex_buffer);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(Transformations), &transformations);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    //Light uniform
    Lights lights;
    lights.ambient = ambient_;
    lights.n_p_lights = p_lights_.size();
    memcpy_s(lights.p_lights, MAX_POINT_LIGHTS * sizeof(xe::PointLight), p_lights_.data(), p_lights_.size() * sizeof(xe::PointLight));

    glBindBuffer(GL_UNIFORM_BUFFER, m_uniform_lights_buffer);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(Lights), &lights);
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
