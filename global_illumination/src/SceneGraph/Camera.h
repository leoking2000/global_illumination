#pragma once
#include "Global/Types.h"

namespace GL
{
	class Camera
	{
	public:
		glm::vec3 pos = glm::vec3(0.0f, 0.0f, 0.0f);
		glm::vec3 dir = glm::vec3(-1.0f, 0.0f, 0.0f);

		glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
	public:
		Camera() {}
		Camera(const glm::vec3& in_pos, const glm::vec3& in_dir);
		glm::mat4 GetCameraView() const;
	public:
		float yaw = -PI / 2.0f;
		float pitch = 0.0f;

		bool mouseInput = false;
		bool x_press = false;
	};
}