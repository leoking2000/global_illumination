#pragma once
#include "Graphics/Shader.h"
#include "Graphics/FrameBuffer.h"
#include "AssetManagement/Model.h"

namespace GL
{
	class DrawStrategy
	{
	public:
		virtual void ClearFrameBuffer() = 0;
		virtual void Draw(const Model& model, const glm::mat4& proj_matrix, const glm::mat4& view_matrix, const glm::mat4& model_matrix) const = 0;
		virtual FrameBuffer& GetFrameBuffer()  = 0;
	};
}