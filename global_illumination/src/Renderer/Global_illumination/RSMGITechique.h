#pragma once
#include "GITechnique.h"
#include "Renderer/GeometryDrawStratagy.h"

namespace GL
{
	class RSMGLTechique : public GITechnique
	{
	public:
		RSMGLTechique(u32 shadowMapSize, Scene& scene);
	public:
		void Init() override;
		void Draw(Scene& scene, const glm::mat4& proj, const glm::mat4& view) override;
		FrameBuffer& GetFrameBuffer() override;
	private:
		GeometryDrawStratagy m_stratagy;
	};
}