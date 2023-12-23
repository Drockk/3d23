#pragma once

#include <vector>

#include "Application/application.h"
#include "Application/utils.h"

#include "glad/gl.h"

#include <glm/glm.hpp>

class SimpleShapeApplication : public xe::Application
{
public:
    SimpleShapeApplication(const int width, const int height, const std::string& title, const bool debug):
    Application(width, height, title, debug),
    m_fov{45.0f}, m_near{0.1f}, m_far{100.0f} {
        
    }

    void init() override;

    void frame() override;

    void framebuffer_resize_callback(int w, int h) override;

private:
    void recalculate_projection();
    void recalculate_projection(float width, float height);

    GLuint vao_{};
    GLuint m_uniform_vertex_buffer{};

    std::vector<GLushort> m_indices;

    float m_fov{};
    float m_aspect{};
    float m_near{};
    float m_far{};

    glm::mat4 m_projection{};
    glm::mat4 m_view{};
};