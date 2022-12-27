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
		m_grid_buffer(128, 128, 128, TextureFormat::RGBA32F, FrameBufferMode::Texture3D)
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

		m_grid_shader = AssetManagement::CreateShader("GI/grid_generation");
	}

	void GlobalIllumination::Draw(Scene& scene)
	{
		m_voxelizer.Voxelize(scene);
		GridCreationStage(scene);

		m_rsm_stratagy->ClearFrameBuffer();
		scene.Draw(*m_rsm_stratagy, scene.light.LightProj(), scene.light.LightView());
	}

	const Voxelizer& GlobalIllumination::GetVoxelizer() const
	{
		return m_voxelizer;
	}

	const FrameBuffer& GlobalIllumination::GetRSMBuffer() const
	{
		return m_rsm_stratagy->GetFrameBuffer();
	}

	void GlobalIllumination::GridCreationStage(Scene& scene)
	{
		m_grid_buffer.Bind();

		glCall(glViewport(0, 0, m_grid_buffer.Width(), m_grid_buffer.Height()));
		glCall(glClearColor(0.0f, 0.0f, 0.0f, 0.0f));
		glCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
		glCall(glDisable(GL_DEPTH_TEST));

		ShaderProgram& shader = *AssetManagement::GetShader(m_grid_shader);
		Mesh& mesh = *AssetManagement::GetMesh(m_voxelizer.m_screen_filled_quad);

		m_voxelizer.GetVoxels().BindColorTexture(0, 0);
		shader.SetUniform("u_voxels", 0);

		shader.SetUniform("u_size", glm::ivec3(m_voxelizer.m_data.dimensions));
		shader.SetUniform("uniform_bbox_min", m_voxelizer.m_data.voxelizationArea.GetMin());
		shader.SetUniform("uniform_bbox_max", m_voxelizer.m_data.voxelizationArea.GetMax());

		shader.Bind();
			
		mesh.m_vertexArray.Bind();
		mesh.m_indexBuffer.Bind();

		glDrawElementsInstanced(GL_TRIANGLES, mesh.m_indexBuffer.GetCount(), GL_UNSIGNED_INT, nullptr, 128);

		mesh.m_vertexArray.UnBind();
		mesh.m_indexBuffer.UnBind();

		shader.UnBind();

		m_grid_buffer.UnBind();
	}

	void GlobalIllumination::DrawPreview(const FrameBuffer& framebuffer, const glm::mat4& proj_view)
	{
		framebuffer.Bind();

		glCall(glDisable(GL_BLEND));
		glCall(glDisable(GL_CULL_FACE));
		glCall(glEnable(GL_DEPTH_TEST));

		ShaderProgram& shader = *AssetManagement::GetShader(m_voxelizer.m_previewSpheresShader);
		Mesh& mesh = *AssetManagement::GetMesh(m_voxelizer.m_cube);

		m_grid_buffer.BindColorTexture(0, 6);

		shader.SetUniform("u_use_voxels", 0);
		shader.SetUniform("u_grid", 6);

		shader.SetUniform("u_proj_view_matrix", proj_view);
		glm::mat4 scale_mat = glm::scale(glm::mat4(1.0f), glm::vec3(0.05f));
		shader.SetUniform("u_scale", scale_mat);

		shader.SetUniform("u_size", glm::ivec3(m_voxelizer.m_data.dimensions));
		shader.SetUniform("uniform_bbox_min", m_voxelizer.m_data.voxelizationArea.GetMin());
		shader.SetUniform("uniform_bbox_max", m_voxelizer.m_data.voxelizationArea.GetMax());

		shader.Bind();

		mesh.m_vertexArray.Bind();
		mesh.m_indexBuffer.Bind();

		int number_of_spheres = m_voxelizer.m_data.voxel_grid_size;
		glDrawElementsInstanced(GL_TRIANGLES, mesh.m_indexBuffer.GetCount(), GL_UNSIGNED_INT, nullptr, number_of_spheres);

		mesh.m_vertexArray.UnBind();
		mesh.m_indexBuffer.UnBind();

		shader.UnBind();

		framebuffer.UnBind();
	}

}


