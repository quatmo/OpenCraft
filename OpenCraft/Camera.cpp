#include "Camera.h"

Camera::Camera(const glm::vec3 position, const glm::vec3 up,
	const float yaw, const float pitch) :
	m_front(glm::vec3(0.0f, 0.0f, -1.0f)),
	m_movementSpeed(SPEED),
	m_mouseSensitivity(SENSITIVTY),
	m_zoom(ZOOM),
	m_position(position),
	m_worldUp(up),
	m_yaw(yaw),
	m_pitch(pitch)
{
	updateCameraVectors();
}

Camera::Camera(const float posX, const float posY, const float posZ,
	const float upX, const float upY, const float upZ,
	const float yaw, const float pitch) :
	m_front(glm::vec3(0.0f, 0.0f, -1.0f)),
	m_movementSpeed(SPEED),
	m_mouseSensitivity(SENSITIVTY),
	m_zoom(ZOOM)
{
	m_position = glm::vec3(posX, posY, posZ);
	m_worldUp = glm::vec3(upX, upY, upZ);
	m_yaw = yaw;
	m_pitch = pitch;
	updateCameraVectors();
}

glm::mat4 Camera::getViewMatrix() const
{
	return glm::lookAt(m_position, m_position + m_front, m_up);
}

void Camera::processKeyboard(const CameraMovement direction, const float deltaTime)
{
	float velocity = m_movementSpeed * deltaTime;
	switch (direction)
	{
	case CameraMovement::FORWARD:
		m_position += m_front * velocity; break;
	case CameraMovement::BACKWARD:
		m_position -= m_front * velocity; break;
	case CameraMovement::LEFT:
		m_position -= m_right * velocity; break;
	case CameraMovement::RIGHT:
		m_position += m_right * velocity; break;
	case CameraMovement::UP:
		m_position += m_up * velocity; break;
	case CameraMovement::DOWN:
		m_position -= m_up * velocity; break;
	default:
		break;
	}
}

void Camera::processMouseMovement(const float xOffset, const float yOffset, const GLboolean constrainPitch)
{
	auto newXOffset = xOffset * m_mouseSensitivity;
	auto newYOffset = yOffset * m_mouseSensitivity;

	m_yaw += newXOffset;
	m_pitch += newYOffset;

	// Make sure that when pitch is out of bounds, screen doesn't get flipped
	if (constrainPitch)
	{
		if (m_pitch > 89.0f)
		{
			m_pitch = 89.0f;
		}
		else if (m_pitch < -89.0f)
		{
			m_pitch = -89.0f;
		}
	}
	updateCameraVectors();
}

void Camera::processMouseScroll(float yoffset)
{
	if (m_zoom >= 1.0f && m_zoom <= 45.0f)
	{
		m_zoom -= yoffset;
	}
	if (m_zoom <= 1.0f)
	{
		m_zoom = 1.0f;
	}
	else if (m_zoom >= 45.0f)
	{
		m_zoom = 45.0f;
	}
}

glm::vec3 Camera::getPosition(void) const
{
	return m_position;
}

float Camera::getZoom(void) const
{
	return m_zoom;
}

void Camera::updateCameraVectors()
{
	// Calculate the new Front vector
	glm::vec3 front;
	front.x = cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
	front.y = sin(glm::radians(m_pitch));
	front.z = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
	m_front = glm::normalize(front);
	// Also re-calculate the Right and Up vector
	m_right = glm::normalize(glm::cross(m_front, m_worldUp));
	m_up = glm::normalize(glm::cross(m_right, m_front));
}
