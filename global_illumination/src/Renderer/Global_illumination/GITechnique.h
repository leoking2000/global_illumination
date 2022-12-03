#pragma once
#include "SceneGraph/Scene.h"
#include "Graphics/FrameBuffer.h"

namespace GL
{
	class GITechnique
	{
	public:
		virtual void Init() = 0;
		virtual void Draw(Scene& scene, const glm::mat4& proj, const glm::mat4& view) = 0;
		virtual FrameBuffer& GetFrameBuffer() = 0;
	};
}


