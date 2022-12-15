#include "Graphics/OpenGL.h"
#include "GlobalIllumination.h"
#include "AssetManagement/AssetManagement.h"
#include "Renderer/DrawStratagies/GeometryDrawStratagy.h"

namespace GL
{
	GlobalIllumination::GlobalIllumination(const GlobalIlluminationParameters& params)
		:
		m_params(params),
		m_voxelizer(m_params.voxelizer_params),
		m_caching_buffer(128, 128, 128, TextureFormat::RGBA32F, FrameBufferMode::Texture3D)
	{

	}

	void GlobalIllumination::Init(Scene& scene)
	{
		m_voxelizer.Init();

		m_rsm_stratagy = std::unique_ptr<DrawStrategy>(
			new GeometryDrawStratagy(
				FrameBuffer(m_params.rsm_resoulution, m_params.rsm_resoulution, 3),
				AssetManagement::CreateShader("GI/rsm_generation"),
				[&](ShaderProgram& shader) {
				scene.light.SetUniforms(shader);
				}
			)
		);
	}

	void GlobalIllumination::Draw(Scene& scene)
	{
		m_voxelizer.Voxelize(scene);
		m_rsm_stratagy->ClearFrameBuffer();
		scene.Draw(*m_rsm_stratagy, scene.light.LightProj(), scene.light.LightView());

		CachingStage(scene);
	}

	const Voxelizer& GlobalIllumination::GetVoxelizer() const
	{
		return m_voxelizer;
	}

	const FrameBuffer& GlobalIllumination::GetRSMBuffer() const
	{
		return m_rsm_stratagy->GetFrameBuffer();
	}

	void GlobalIllumination::CachingStage(Scene& scene)
	{
		m_caching_buffer.Bind();

		glCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));






		m_caching_buffer.UnBind();
	}

}


