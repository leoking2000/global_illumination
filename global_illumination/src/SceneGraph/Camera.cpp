#include "Camera.h"

namespace GL
{
	Camera::Camera(const glm::vec3& in_pos, const glm::vec3& in_dir)
		:
		pos(in_pos),
		dir(glm::normalize(in_dir))
	{

	}

	glm::mat4 Camera::GetCameraView() const
	{
		return glm::lookAt(pos, pos + dir, up);
	}

}


