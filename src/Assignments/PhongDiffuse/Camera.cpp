#include "Camera.h"
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/dual_quaternion.hpp"

namespace utils {
    glm::mat3 rotation(float angle, const glm::vec3& axis) {
        auto u = normalize(axis);
        auto s = std::sin(angle);
        auto c = std::cos(angle);

        return glm::mat3(
            c + u.x * u.x * (1.0f - c),
            u.y * u.x * (1.0f - c) + u.z * s,
            u.z * u.x * (1.0f - c) - u.y * s,

            u.x * u.y * (1.0f - c) - u.z * s,
            c + u.y * u.y * (1.0f - c),
            u.z * u.y * (1.0f - c) + u.x * s,

            u.x * u.z * (1.0f - c) + u.y * s,
            u.y * u.z * (1.0f - c) - u.x * s,
            c + u.z * u.z * (1.0f - c)
        );
    }
}

void Camera::look_at(const glm::vec3& eye, const glm::vec3& center, const glm::vec3& up) {
    m_z = normalize(eye - center);
    m_x = normalize(cross(up, m_z));
    m_y = normalize(cross(m_z, m_x));

    m_position = eye;
    m_center = center;
}

void Camera::zoom(float y_offset) {
    auto x = m_fov / glm::pi<float>();
    auto y = inverse_logistic(x);
    y += y_offset;
    x = logistic(y);
    m_fov = x * glm::pi<float>();
}

void Camera::rotate_around_center(const float angle, const glm::vec3& axis) {
    rotate_around_point(angle, axis, m_center);
}

void Camera::rotate_around_point(float angle, const glm::vec3& axis, const glm::vec3& c) {
    const auto R = utils::rotation(angle, axis);
    m_x = R * m_x;
    m_y = R * m_y;
    m_z = R * m_z;

    auto t = m_position - c;
    t = R * t;
    m_position = c + t;
}

float Camera::logistic(float y) {
    return 1.0f / (1.0f + std::exp(-y));
}

float Camera::inverse_logistic(float x) {
    return std::log(x / (1.0f - x));
}

glm::mat4 Camera::view() const {
    glm::mat4 view(1.0f);

    for (auto i = 0; i < 3; ++i) {
        view[i][0] = m_x[i];
        view[i][1] = m_y[i];
        view[i][2] = m_z[i];
    }

    const auto t = -glm::vec3{
        dot(m_x, m_position),
        dot(m_y, m_position),
        dot(m_z, m_position)
    };

    view[3] = glm::vec4(t, 1.0f);

    return view;
}

