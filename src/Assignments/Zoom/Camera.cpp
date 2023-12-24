#include "Camera.h"

void Camera::zoom(float y_offset) {
	auto x = m_fov / glm::pi<float>();
	auto y = inverse_logistic(x);
	y += y_offset;
	x = logistic(y);
	m_fov = x * glm::pi<float>();
}

float Camera::logistic(float y) {
	return 1.0f / (1.0f + std::exp(-y));
}

float Camera::inverse_logistic(float x) {
	return std::log(x / (1.0f - x));
}
