#pragma once

#include <vector>

#include "Application/application.h"
#include "Application/utils.h"

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

    GLuint vao_{};
    GLuint m_uniform_vertex_buffer{};

    std::vector<GLushort> m_indices;

    Camera m_camera;
    CameraControler m_camera_controler;
};