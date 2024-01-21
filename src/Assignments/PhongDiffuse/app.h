#pragma once

#include <vector>

#include "Application/application.h"
#include "Application/utils.h"
#include <Engine/Mesh.h>
#include <Engine/Light.h>

#include "glad/gl.h"

#include "Camera.h"
#include "CameraControler.h"

class SimpleShapeApplication : public xe::Application
{
public:
    SimpleShapeApplication(const int width, const int height, const std::string& title, const bool debug):
    Application(width, height, title, debug) {
    }

    void init() override;

    void frame() override;

    void framebuffer_resize_callback(int w, int h) override;
    void scroll_callback(double xoffset, double yoffset) override;

    void mouse_button_callback(int button, int action, int mods) override;
    void cursor_position_callback(double x, double y) override;

private:
    void add_light(const xe::PointLight& p_light)
    {
        p_lights_.emplace_back(p_light);
    }

    void add_ambient(glm::vec3 ambient)
    {
        ambient_ = ambient;
    }

    GLuint m_uniform_vertex_buffer{};
    GLuint m_uniform_lights_buffer{};

    Camera m_camera;
    CameraControler m_camera_controler;

    std::vector<std::shared_ptr<xe::Mesh>> m_meshes;

    glm::vec3 ambient_;
    std::vector<xe::PointLight> p_lights_;
};