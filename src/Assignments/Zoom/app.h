#pragma once

#include <vector>

#include "Application/application.h"
#include "Application/utils.h"

#include "glad/gl.h"

#include <glm/glm.hpp>

#include "Camera.h"

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

private:

    GLuint vao_{};
    GLuint m_uniform_vertex_buffer{};

    std::vector<GLushort> m_indices;

    Camera m_camera;
};