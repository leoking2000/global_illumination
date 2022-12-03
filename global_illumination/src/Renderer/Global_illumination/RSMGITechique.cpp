#include "RSMGITechique.h"
#include "AssetManagement/AssetManagement.h"

namespace GL
{
	RSMGLTechique::RSMGLTechique(u32 shadowMapSize, Scene& scene)
		:
		m_stratagy(FrameBuffer(shadowMapSize, shadowMapSize, 3), 
			AssetManagement::CreateShader("ShadowMaps/rsm_generation"), 
			[&](ShaderProgram& shader) {
				scene.light.SetUniforms(shader);
			})
	{
	}

	void GL::RSMGLTechique::Init()
	{
	}

	void GL::RSMGLTechique::Draw(Scene& scene, const glm::mat4& proj, const glm::mat4& view)
	{
		m_stratagy.ClearFrameBuffer();
	
		scene.Draw(m_stratagy, proj, view);
	}

	FrameBuffer& GL::RSMGLTechique::GetFrameBuffer()
	{
		return m_stratagy.GetFrameBuffer();
	}
}


