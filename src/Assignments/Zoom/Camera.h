#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera {
public:
    void look_at(const glm::vec3& eye, const glm::vec3& center, const glm::vec3& up) {
        m_view = glm::lookAt(eye, center, up);
    }

    void perspective(const float fov, const float aspect, const float near, const float far) {
        m_fov = fov;
        m_aspect = aspect;
        m_near = near;
        m_far = far;
    }

    void set_aspect(const float aspect) {
        m_aspect = aspect;
    }

    [[nodiscard]] glm::mat4 view() const {
        return m_view;
    }

    [[nodiscard]] glm::mat4 projection() const {
        return glm::perspective(m_fov, m_aspect, m_near, m_far);
    }

    void zoom(float y_offset);

private:
    static float logistic(float y);
    static float inverse_logistic(float x);

    float m_aspect{};
    float m_far{};
    float m_fov{};
    float m_near{};

    glm::mat4 m_view{};
};