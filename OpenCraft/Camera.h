#pragma once

#include <vector>

#include "ChunkManager.h"

#include "glad.h"
#include "ext/glm/glm.hpp"
#include "ext/glm/gtc/matrix_transform.hpp"

enum class CameraMovement
{
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT,
	UP,
	DOWN
};

// Default camera values
constexpr float YAW = 0.0f;
constexpr float PITCH = 0.0f;
constexpr float SPEED = 5.0f;
constexpr float SENSITIVTY = 0.1f;
constexpr float ZOOM = 45.0f;
constexpr float ACCERATION = 0.1f;
constexpr float JUMP_INIT_SPEED = -0.03125f;

class Camera
{
public:
	Camera(const glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f),
		const glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
		const float yaw = YAW, const float pitch = PITCH);
	Camera(const float posX, const float posY, const float posZ,
		const float upX, const float upY, const float upZ,
		const float yaw, const float pitch);

	glm::mat4 getViewMatrix()const;

	void jump();
	void step(const float deltaTime, ChunkManager& chunkManager);
	void processKeyboard(const CameraMovement direction, const float deltaTime,ChunkManager& chunkManager);
	void processMouseMovement(const float xOffset, const float yOffset,
		const GLboolean constrainPitch = true);
	void processMouseScroll(float yoffset);

	glm::vec3 getPosition(void)const;
	glm::vec3 getFront(void)const;

	float getZoom(void)const;

	~Camera() = default;
private:
	// Camera Attributes
	float m_yaw;
	float m_pitch;
	float m_movementSpeed;
	float m_mouseSensitivity;
	float m_zoom;

	glm::vec3 m_position;
	glm::vec3 m_front;
	glm::vec3 m_up;
	glm::vec3 m_right;
	glm::vec3 m_worldUp;

	bool m_roamMode;
	float m_ySpeed;


	void updateCameraVectors();
	void processKeyBoardRoam(const CameraMovement direction, const float deltaTime, ChunkManager& chunkManager);
	void processKeyBoardNative(const CameraMovement direction, const float deltaTime, ChunkManager& chunkManager);
};

