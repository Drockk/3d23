#pragma once
#include "Camera.h"

class CameraControler {
public:
    CameraControler() = default;
    CameraControler(xe::Camera* p_camera) : m_camera{ p_camera } {}

    void set_camera(xe::Camera* p_camera) {
        m_camera = p_camera;
    }

    void rotate_camera(float dx, float dy) {
        if (m_camera == nullptr)
            return;

        m_camera->rotate_around_center(-m_scale * dy, m_camera->x());
        m_camera->rotate_around_center(-m_scale * dx, m_camera->y());
    }

    void mouse_moved(const float x, const float y) {
        if (m_lmb_pressed) {
            auto dx = x - m_x;
            auto dy = y - m_y;
            m_x = x;
            m_y = y;

            rotate_camera(dx, dy);
        }
    }

    void lmb_pressed(const float x, const float y) {
        m_lmb_pressed = true;
        m_x = x;
        m_y = y;
    }

    void lmb_released(const float x, const float y) {
        m_lmb_pressed = false;
        auto dx = x - m_x;
        auto dy = y - m_y;

        rotate_camera(dx, dy);
    }

private:
    xe::Camera* m_camera{ nullptr };

    float m_scale{ 1.0f };
    bool m_lmb_pressed{ false };

    float m_x{};
    float m_y{};
};