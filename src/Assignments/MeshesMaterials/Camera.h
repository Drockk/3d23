#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera {
public:
    void look_at(const glm::vec3& eye, const glm::vec3& center, const glm::vec3& up);

    void perspective(const float fov, const float aspect, const float near, const float far) {
        m_fov = fov;
        m_aspect = aspect;
        m_near = near;
        m_far = far;
    }

    void set_aspect(const float aspect) {
        m_aspect = aspect;
    }

    [[nodiscard]] glm::mat4 view() const;

    [[nodiscard]] glm::mat4 projection() const {
        return glm::perspective(m_fov, m_aspect, m_near, m_far);
    }

    void zoom(float y_offset);

    [[nodiscard]] glm::vec3 x() const {
        return m_x;
    }

    [[nodiscard]] glm::vec3 y() const {
        return m_y;
    }

    [[nodiscard]] glm::vec3 z() const {
        return m_z;
    }

    [[nodiscard]] glm::vec3 position() const {
        return m_position;
    }

    [[nodiscard]] glm::vec3 center() const {
        return m_center;
    }

    void rotate_around_center(float angle, const glm::vec3& axis);

private:
    void rotate_around_point(float angle, const glm::vec3& axis, const glm::vec3& c);

    static float logistic(float y);
    static float inverse_logistic(float x);

    float m_aspect{};
    float m_far{};
    float m_fov{};
    float m_near{};

    glm::vec3 m_position{};
    glm::vec3 m_center{};
    glm::vec3 m_x{};
    glm::vec3 m_y{};
    glm::vec3 m_z{};
};