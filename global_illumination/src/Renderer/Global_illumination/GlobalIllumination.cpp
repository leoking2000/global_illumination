#include "Graphics/OpenGL.h"
#include "GlobalIllumination.h"
#include "AssetManagement/AssetManagement.h"
#include "Renderer/DrawStratagies/GeometryDrawStratagy.h"
#include "Global/RandomNumbers.h"
#include "imgui/imgui.h"
#include "Global/Logger.h"
#include <algorithm>

namespace GL
{
	GlobalIllumination::GlobalIllumination(const GlobalIlluminationParameters& params, u32 window_width, u32 window_height)
		:
		m_params(params),
		m_voxelizer(m_params.voxelizer_params),
		m_cachingBuffer(
			(u32)m_voxelizer.GetData().dimensions.x, (u32)m_voxelizer.GetData().dimensions.y, (u32)m_voxelizer.GetData().dimensions.z, 7,
			TextureMinFiltering::MIN_LINEAR, TextureMagFiltering::MAG_LINEAR, TextureFormat::RGBA32F),
		m_cachingBuffer_copy(
			(u32)m_voxelizer.GetData().dimensions.x, (u32)m_voxelizer.GetData().dimensions.y, (u32)m_voxelizer.GetData().dimensions.z, 7,
			TextureMinFiltering::MIN_LINEAR, TextureMagFiltering::MAG_LINEAR, TextureFormat::RGBA32F),
		m_bounces(params.bounces)
	{

	}

	void GlobalIllumination::Init(Scene& scene)
	{
		m_voxelizer.Init();

		m_rsm_stratagy = std::unique_ptr<DrawStrategy>(
			new GeometryDrawStratagy(
				FrameBuffer(m_params.rsm_resoulution, m_params.rsm_resoulution, 3, 
					TextureMinFiltering::MIN_LINEAR, TextureMagFiltering::MAG_LINEAR),
				AssetManagement::CreateShader("GI/rsm_generation"),
				[&](ShaderProgram& shader) {
				scene.light.SetUniforms(shader);
				}
			)
		);

		m_caching_shader = AssetManagement::CreateShader("GI/caching");
		m_bounces_shader = AssetManagement::CreateShader("GI/bounce");
		m_reconstruction_shader = AssetManagement::CreateShader("GI/reconstruction");

		RandomNumbers::Genarate();
	}

	void GlobalIllumination::Draw(Scene& scene,const FrameBuffer& shading_buffer, const FrameBuffer& geometryBuffer,
		const glm::mat4& proj, const glm::mat4& view)
	{
		// voxelize
		m_voxelizer.Voxelize(scene);

		// caching
		CachingStep(scene);

		// bounce
		BounceStep();

		// blend

		// Reconstruction
		ReconstructionStep(scene, shading_buffer, geometryBuffer, proj, view);
	}

	const Voxelizer& GlobalIllumination::GetVoxelizer() const
	{
		return m_voxelizer;
	}

	void GlobalIllumination::DrawRSM(Scene& scene)
	{
		// genarate RSM
		m_rsm_stratagy->ClearFrameBuffer();
		scene.Draw(*m_rsm_stratagy, scene.light.LightProj(), scene.light.LightView());
	}

	const FrameBuffer& GlobalIllumination::GetRSMBuffer() const
	{
		return m_rsm_stratagy->GetFrameBuffer();
	}

	void GlobalIllumination::ImGui()
	{
		if (ImGui::CollapsingHeader("Global Illumination"))
		{
			ImGui::Text("Caching Step");
			ImGui::SliderInt("RSM Samples", &m_num_RSM_samples, 0, 200);
			ImGui::DragFloat("Spread", &m_spread, 0.001f, 0, 2);
			ImGui::Checkbox("Occlusion Enable", &m_occlusion_enable);
			ImGui::SliderInt("Occlusion Samples", &m_num_occlusion_sample, 1, 20);

			ImGui::Text("Bounces Step");
			ImGui::SliderInt("Bounces", &m_bounces, 1, 5);

			ImGui::Text("Reconstruction");
			ImGui::DragFloat("factor", &m_factor, 0.001f, 0, 2);
		}
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

		// Voxels
		m_voxelizer.GetVoxels().BindColorTexture(0, 0);

		const glm::ivec3 size = glm::ivec3(m_voxelizer.GetData().dimensions);
		const glm::vec3 bbox_max = m_voxelizer.GetData().voxelizationArea.GetMax();
		const glm::vec3 bbox_min = m_voxelizer.GetData().voxelizationArea.GetMin();
		const glm::vec3 bextents = bbox_max - bbox_min;

		glm::vec3 stratum = bextents;
		stratum /= size;

		shader.SetUniform("u_voxels_musked", 0);
		shader.SetUniform("u_size", size);
		shader.SetUniform("u_bbox_max", bbox_max);
		shader.SetUniform("u_bbox_min", bbox_min);
		shader.SetUniform("u_stratum", stratum);
		shader.SetUniform("u_occlusion_bextents", bextents);

		// RSM
		GetRSMBuffer().BindDepthTexture(1);
		GetRSMBuffer().BindColorTexture(0, 2);
		GetRSMBuffer().BindColorTexture(2, 3);

		shader.SetUniform("u_RSM_depth", 1);
		shader.SetUniform("u_RSM_flux", 2);
		shader.SetUniform("u_RSM_normal", 3);
		
		// Light
		glm::mat4 light_mvp = scene.light.LightProj() * scene.light.LightView();
		glm::mat4 light_mvp_inv = glm::inverse(light_mvp);

		shader.SetUniform("u_light_pos", scene.light.m_pos);
		shader.SetUniform("u_light_dir", scene.light.m_dir);
		shader.SetUniform("u_light_projection_view", light_mvp);
		shader.SetUniform("u_light_projection_view_inv", light_mvp_inv);

		// settings
		shader.SetUniform("u_spread", m_spread);
		shader.SetUniform("u_num_RSM_samples", m_num_RSM_samples);
		shader.SetUniform("u_occlusion_enable", (u32)m_occlusion_enable);
		shader.SetUniform("u_num_occlusion_samples", m_num_occlusion_sample);

		// random
		shader.SetUniform("u_samples_2d", RandomNumbers::GetHaltonSequence2D(), m_num_RSM_samples);
		shader.SetUniform("u_samples_3d", RandomNumbers::GetHaltonSequence3DSphere(), m_num_RSM_samples);

		shader.Bind();

		mesh.m_vertexArray.Bind();
		mesh.m_indexBuffer.Bind();

		glDrawElementsInstanced(GL_TRIANGLES, mesh.m_indexBuffer.GetCount(), GL_UNSIGNED_INT, nullptr, m_cachingBuffer.Depth());

		mesh.m_vertexArray.UnBind();
		mesh.m_indexBuffer.UnBind();

		shader.UnBind();

		m_cachingBuffer.UnBind();

		m_active_cachingBuffer = &m_cachingBuffer;
	}

	void GlobalIllumination::BounceStep()
	{
		if (m_bounces <= 1) return;

		m_active_cachingBuffer = &m_cachingBuffer;
		FrameBuffer* write_buffer = &m_cachingBuffer_copy;

		ShaderProgram& shader = *AssetManagement::GetShader(m_bounces_shader);
		Mesh& mesh = *AssetManagement::GetMesh(m_voxelizer.m_screen_filled_quad);

		glm::vec3 bsize = m_voxelizer.GetData().voxelizationArea.GetSize();
		glm::vec3 stratum = bsize;
		stratum /= m_voxelizer.GetData().dimensions;

		for (i32 i = 1; i < m_bounces; i++)
		{
			write_buffer->Bind();

			glCall(glViewport(0, 0, write_buffer->Width(), write_buffer->Height()));
			glCall(glClearColor(0.0f, 0.0f, 0.0f, 0.0f));
			glCall(glClear(GL_COLOR_BUFFER_BIT));
			glCall(glDisable(GL_DEPTH_TEST));
			glCall(glDisable(GL_BLEND));

			m_active_cachingBuffer->BindColorTexture(0, 7);
			m_active_cachingBuffer->BindColorTexture(1, 8);
			m_active_cachingBuffer->BindColorTexture(2, 9);
			m_active_cachingBuffer->BindColorTexture(3, 10);
			m_active_cachingBuffer->BindColorTexture(4, 11);
			m_active_cachingBuffer->BindColorTexture(5, 12);
			m_active_cachingBuffer->BindColorTexture(6, 13);

			shader.SetUniform("caching_data[0]", 7);
			shader.SetUniform("caching_data[1]", 8);
			shader.SetUniform("caching_data[2]", 9);
			shader.SetUniform("caching_data[3]", 10);
			shader.SetUniform("caching_data[4]", 11);
			shader.SetUniform("caching_data[5]", 12);
			shader.SetUniform("caching_data[6]", 13);

			m_voxelizer.GetVoxels().BindColorTexture(0, 0);
			shader.SetUniform("u_voxels_musked", 0);

			shader.SetUniform("u_size", glm::ivec3(m_voxelizer.GetData().dimensions));
			shader.SetUniform("u_bbox_max", m_voxelizer.GetData().voxelizationArea.GetMax());
			shader.SetUniform("u_bbox_min", m_voxelizer.GetData().voxelizationArea.GetMin());
			shader.SetUniform("u_stratum", stratum);

			shader.SetUniform("u_num_samples", 100);
			shader.SetUniform("u_samples_3d", RandomNumbers::GetHaltonSequence3DSphereOnSurface(), 100);
			//shader.SetUniform("u_average_albedo", 0.5f);

			shader.Bind();
			mesh.m_vertexArray.Bind();
			mesh.m_indexBuffer.Bind();

			glDrawElementsInstanced(GL_TRIANGLES, mesh.m_indexBuffer.GetCount(), GL_UNSIGNED_INT, nullptr, write_buffer->Depth());

			write_buffer->UnBind();

			std::swap(write_buffer, m_active_cachingBuffer);
		}

		mesh.m_vertexArray.UnBind();
		mesh.m_indexBuffer.UnBind();

		shader.UnBind();

		write_buffer->UnBind();
		m_active_cachingBuffer->UnBind();

	}

	void GlobalIllumination::ReconstructionStep(Scene& scene, const FrameBuffer& shading_buffer, const FrameBuffer& geometryBuffer,
		const glm::mat4& proj, const glm::mat4& view)
	{
		glCall(glEnable(GL_BLEND));
		glBlendFunc(GL_ONE, GL_ONE);
		glCall(glBlendEquation(GL_FUNC_ADD));

		shading_buffer.Bind();

		glCall(glViewport(0, 0, shading_buffer.Width(), shading_buffer.Height()));
		glCall(glDisable(GL_DEPTH_TEST));

		ShaderProgram& shader = *AssetManagement::GetShader(m_reconstruction_shader);

		geometryBuffer.BindColorTexture(0, 0);
		geometryBuffer.BindColorTexture(1, 1);
		geometryBuffer.BindColorTexture(2, 2);
		geometryBuffer.BindColorTexture(3, 3);
		geometryBuffer.BindDepthTexture(5);

		shader.SetUniform("u_tex_albedo", 0);
		shader.SetUniform("u_tex_pos", 1);
		shader.SetUniform("u_tex_normal", 2);
		shader.SetUniform("u_tex_mask", 3);
		shader.SetUniform("u_tex_depth", 5);

		shader.SetUniform("u_proj_view_inv", glm::inverse(proj * view));

		shader.SetUniform("u_bbox_min", m_voxelizer.GetData().voxelizationArea.GetMin());
		shader.SetUniform("u_bbox_max", m_voxelizer.GetData().voxelizationArea.GetMax());

		shader.SetUniform("u_factor", m_factor);

		assert(m_active_cachingBuffer != nullptr);
		m_active_cachingBuffer->BindColorTexture(0, 6);
		m_active_cachingBuffer->BindColorTexture(1, 7);
		m_active_cachingBuffer->BindColorTexture(2, 8);
		m_active_cachingBuffer->BindColorTexture(3, 9);
		m_active_cachingBuffer->BindColorTexture(4, 10);
		m_active_cachingBuffer->BindColorTexture(5, 11);
		m_active_cachingBuffer->BindColorTexture(6, 12);

		shader.SetUniform("u_caching_data[0]", 6);
		shader.SetUniform("u_caching_data[1]", 7);
		shader.SetUniform("u_caching_data[2]", 8);
		shader.SetUniform("u_caching_data[3]", 9);
		shader.SetUniform("u_caching_data[4]", 10);
		shader.SetUniform("u_caching_data[5]", 11);
		shader.SetUniform("u_caching_data[6]", 12);

		shader.Bind();
		AssetManagement::GetMesh(m_voxelizer.m_screen_filled_quad)->Draw();
		shader.UnBind();

		shading_buffer.UnBind();

		glCall(glDisable(GL_BLEND));
	}

}


