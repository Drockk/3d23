#pragma once

#include <vector>

#include "Application/application.h"
#include "Application/utils.h"

#include "glad/gl.h"

class SimpleShapeApplication : public xe::Application
{
public:
    SimpleShapeApplication(int width, int height, std::string title, bool debug) : Application(width, height, title, debug), m_width(width), m_height(height) {}

    void init() override;

    void frame() override;

private:
    GLuint vao_{};
    int m_width{};
    int m_height{};

    std::vector<GLushort> m_indices;
};