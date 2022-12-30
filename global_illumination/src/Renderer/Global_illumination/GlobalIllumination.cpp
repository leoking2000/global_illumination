#include "Graphics/OpenGL.h"
#include "GlobalIllumination.h"
#include "AssetManagement/AssetManagement.h"
#include "Renderer/DrawStratagies/GeometryDrawStratagy.h"
#include "Global/RandomNumbers.h"

namespace GL
{
	GlobalIllumination::GlobalIllumination(const GlobalIlluminationParameters& params)
		:
		m_params(params),
		m_voxelizer(m_params.voxelizer_params),
		m_cachingBuffer(
			(u32)m_voxelizer.GetData().dimensions.x, (u32)m_voxelizer.GetData().dimensions.y, (u32)m_voxelizer.GetData().dimensions.z, 
			6, TextureFormat::RGBA32F)
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

		m_caching_shader = AssetManagement::CreateShader("GI/caching");

		RandomNumbers::Genarate();
	}

	void GlobalIllumination::Draw(Scene& scene)
	{
		// voxelize
		m_voxelizer.Voxelize(scene);

		// genarate RSM
		m_rsm_stratagy->ClearFrameBuffer();
		scene.Draw(*m_rsm_stratagy, scene.light.LightProj(), scene.light.LightView());

		// caching
		CachingStep(scene);

		// bounce

		// blend
	}

	const Voxelizer& GlobalIllumination::GetVoxelizer() const
	{
		return m_voxelizer;
	}

	const FrameBuffer& GlobalIllumination::GetRSMBuffer() const
	{
		return m_rsm_stratagy->GetFrameBuffer();
	}

	void GlobalIllumination::CachingStep(Scene& scene)
	{
		m_cachingBuffer.Bind();

		glCall(glViewport(0, 0, m_cachingBuffer.Width(), m_cachingBuffer.Height()));
		glCall(glClearColor(0.0f, 0.0f, 0.0f, 0.0f));
		glCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
		glCall(glDisable(GL_DEPTH_TEST));

		ShaderProgram& shader = *AssetManagement::GetShader(m_caching_shader);
		Mesh& mesh = *AssetManagement::GetMesh(m_voxelizer.m_screen_filled_quad);

		m_voxelizer.GetVoxels(true).BindColorTexture(0, 0);
		shader.SetUniform("u_voxels_musked", 0);

		m_voxelizer.GetVoxels(false).BindColorTexture(0, 1);
		shader.SetUniform("u_voxels_3D", 1);

		shader.SetUniform("u_light_pos", scene.light.m_pos);
		shader.SetUniform("u_light_dir", scene.light.m_dir);
		shader.SetUniform("u_light_projection_view", scene.light.LightProj() * scene.light.LightView());


		GetRSMBuffer().BindColorTexture(0, 4);
		shader.SetUniform("u_RSM_flux", 4);

		GetRSMBuffer().BindColorTexture(1, 3);
		shader.SetUniform("u_RSM_pos", 3);

		GetRSMBuffer().BindColorTexture(2, 5);
		shader.SetUniform("u_RSM_normal", 5);

		shader.SetUniform("u_spread", 1.0f);
		shader.SetUniform("u_num_samples", 50);

		assert(shader.SetUniform("u_samples_2d", RandomNumbers::GetHaltonSequence2D(), 50));
		assert(shader.SetUniform("u_samples_3d", RandomNumbers::GetHaltonSequence3DSphere(), 50));

		shader.Bind();

		mesh.m_vertexArray.Bind();
		mesh.m_indexBuffer.Bind();

		glDrawElementsInstanced(GL_TRIANGLES, mesh.m_indexBuffer.GetCount(), GL_UNSIGNED_INT, nullptr, m_cachingBuffer.Depth());

		mesh.m_vertexArray.UnBind();
		mesh.m_indexBuffer.UnBind();

		shader.UnBind();

		m_cachingBuffer.UnBind();
	}

}


