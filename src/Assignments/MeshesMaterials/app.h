#pragma once

#include <vector>

#include "Application/application.h"
#include "Application/utils.h"
#include <Engine/Material.h>
#include <Engine/Mesh.h>

#include "glad/gl.h"

#include "Camera.h"
#include "CameraControler.h"

class SimpleShapeApplication : public xe::Application
{
public:
    SimpleShapeApplication(const int width, const int height, const std::string& title, const bool debug):
    Application(width, height, title, debug), m_color_material({1.0f, 1.0f, 1.0f, 1.0f}) {
    }

    void init() override;

    void frame() override;

    void framebuffer_resize_callback(int w, int h) override;
    void scroll_callback(double xoffset, double yoffset) override;

    void mouse_button_callback(int button, int action, int mods) override;
    void cursor_position_callback(double x, double y) override;

private:
    void add_submesh(xe::Mesh* p_mesh);
    void load_image(const std::string& p_path);

    GLuint m_uniform_vertex_buffer{};

    Camera m_camera;
    CameraControler m_camera_controler;

    xe::ColorMaterial m_color_material;

    std::vector<xe::Mesh*> m_meshes;
};