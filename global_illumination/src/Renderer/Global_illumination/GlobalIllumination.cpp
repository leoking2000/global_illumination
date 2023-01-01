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
			7, TextureFormat::RGBA32F)
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
		m_reconstruction_shader = AssetManagement::CreateShader("GI/reconstruction");

		RandomNumbers::Genarate();
	}

	void GlobalIllumination::PreDraw(Scene& scene)
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

	void GlobalIllumination::Draw(Scene& scene, const FrameBuffer& shading_buffer, const FrameBuffer& geometryBuffer, 
		const glm::mat4& proj, const glm::mat4& view, const glm::vec3& background_color)
	{
		shading_buffer.Bind();

		glCall(glViewport(0, 0, shading_buffer.Width(), shading_buffer.Height()));
		glCall(glClearColor(background_color.r, background_color.g, background_color.b, 1));
		glCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

		glCall(glDisable(GL_DEPTH_TEST));

		geometryBuffer.BindColorTexture(0, 0);
		geometryBuffer.BindColorTexture(1, 1);
		geometryBuffer.BindColorTexture(2, 2);
		geometryBuffer.BindColorTexture(3, 3);
		geometryBuffer.BindDepthTexture(5);

		ShaderProgram& shader = *AssetManagement::GetShader(m_reconstruction_shader);

		shader.SetUniform("u_tex_albedo", 0);
		shader.SetUniform("u_tex_pos", 1);
		shader.SetUniform("u_tex_normal", 2);
		shader.SetUniform("u_tex_mask", 3);
		shader.SetUniform("u_tex_depth", 5);

		shader.SetUniform("u_camera_pos", scene.camera.pos);
		shader.SetUniform("u_camera_dir", scene.camera.dir);

		scene.light.SetUniforms(shader);

		shader.SetUniform("u_light_projection_view", scene.light.LightProj() * scene.light.LightView());

		GetRSMBuffer().BindDepthTexture(6);
		shader.SetUniform("u_shadowMap", 6);

		shader.SetUniform("u_projection_view_inv", glm::inverse(proj * view));
		shader.SetUniform("u_view_inv", glm::inverse(view));

		m_cachingBuffer.BindColorTexture(0, 7);
		m_cachingBuffer.BindColorTexture(1, 8);
		m_cachingBuffer.BindColorTexture(2, 9);
		m_cachingBuffer.BindColorTexture(3, 10);
		m_cachingBuffer.BindColorTexture(4, 11);
		m_cachingBuffer.BindColorTexture(5, 12);
		m_cachingBuffer.BindColorTexture(6, 13);

		shader.SetUniform("caching_data[0]", 7);
		shader.SetUniform("caching_data[1]", 8);
		shader.SetUniform("caching_data[2]", 9);
		shader.SetUniform("caching_data[3]", 10);
		shader.SetUniform("caching_data[4]", 11);
		shader.SetUniform("caching_data[5]", 12);
		shader.SetUniform("caching_data[6]", 13);

		shader.SetUniform("u_size", glm::ivec3(m_voxelizer.GetData().dimensions));
		shader.SetUniform("u_bbox_min", m_voxelizer.GetData().voxelizationArea.GetMin());
		shader.SetUniform("u_bbox_max", m_voxelizer.GetData().voxelizationArea.GetMax());

		shader.Bind();

		AssetManagement::GetMesh(m_voxelizer.m_screen_filled_quad)->Draw();

		shader.UnBind();
		shading_buffer.UnBind();
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

		shader.SetUniform("u_size", glm::ivec3(m_voxelizer.GetData().dimensions));
		shader.SetUniform("u_bbox_min", m_voxelizer.GetData().voxelizationArea.GetMin());

		const glm::vec3& bsize = m_voxelizer.GetData().voxelizationArea.GetSize();
		glm::vec3 stratum = bsize;
		stratum /= glm::ivec3(m_voxelizer.GetData().dimensions);
		shader.SetUniform("u_stratum", stratum);

		glm::vec3 voxelizer_bmin = m_voxelizer.GetData().voxelizationArea.GetMin();
		glm::vec3 voxelizer_bmax = m_voxelizer.GetData().voxelizationArea.GetMax();
		glm::vec3 voxelizer_bextents = voxelizer_bmax - voxelizer_bmin;
		shader.SetUniform("u_occlusion_bextents", voxelizer_bextents);


		GetRSMBuffer().BindColorTexture(0, 1);
		shader.SetUniform("u_RSM_flux", 1);

		GetRSMBuffer().BindColorTexture(1, 2);
		shader.SetUniform("u_RSM_pos", 2);

		GetRSMBuffer().BindColorTexture(2, 3);
		shader.SetUniform("u_RSM_normal", 3);

		shader.SetUniform("u_light_pos", scene.light.m_pos);
		shader.SetUniform("u_light_dir", scene.light.m_dir);
		shader.SetUniform("u_light_projection_view", scene.light.LightProj() * scene.light.LightView());

		shader.SetUniform("u_spread", 1.0f);
		shader.SetUniform("u_num_samples", 200);

		assert(shader.SetUniform("u_samples_2d", RandomNumbers::GetHaltonSequence2D(), 200));
		assert(shader.SetUniform("u_samples_3d", RandomNumbers::GetHaltonSequence3DSphere(), 200));

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


