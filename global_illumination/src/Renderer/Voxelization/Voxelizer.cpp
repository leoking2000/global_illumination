#include "Graphics/OpenGL.h"
#include "Voxelizer.h"
#include "VoxelizerDrawStratagy.h"
#include "AssetManagement/AssetManagement.h"

#include "Global/Logger.h"

namespace GL
{
	Voxelizer::Voxelizer(const VoxelizerParameters& params)
		:
		m_data(params.center, params.size)
	{

	}

	void Voxelizer::Init()
	{
		// Create the VoxelizerDrawStratagy //
		m_strategie = std::make_unique<VoxelizerDrawStratagy>(m_data);

		// Create merge Framebuffer and shader //
		m_merge_voxels = std::make_unique<FrameBuffer>(
			(u32)m_data.dimensions.x, (u32)m_data.dimensions.y, 1,
			TextureFormat::RGBA32UI);
		m_mergeShader = AssetManagement::CreateShader("Voxelization\\ThreeWayBinaryMerge");

		// Create dilation Framebuffer and shader //
		m_voxels_dilated = std::make_unique<FrameBuffer>(
			(u32)m_data.dimensions.x, (u32)m_data.dimensions.y, 1,
			TextureFormat::RGBA32UI);
		m_dilationShader = AssetManagement::CreateShader("Voxelization\\DilateVoxelSpace");

		// make preview shader
		m_previewSpheresShader = AssetManagement::CreateShader("Voxelization\\BinaryPreviewSpheres");

		// make usefull meshs
		m_screen_filled_quad = AssetManagement::CreateMesh(DefaultShape::SCRERN_FILLED_QUARD);
		m_cube = AssetManagement::CreateMesh(DefaultShape::CUBE);

	}

	void Voxelizer::Voxelize(Scene& scene)
	{
		ThreeWayStep(scene);
		MergeStep();
		DilationStep();
	}

	void Voxelizer::DrawPreviewSpheres(const FrameBuffer& framebuffer, const glm::mat4& proj_view) const
	{
		VoxelizerDrawStratagy& vds = (VoxelizerDrawStratagy&)(*m_strategie);

		framebuffer.Bind();

		glCall(glDisable(GL_BLEND));
		glCall(glDisable(GL_CULL_FACE));
		glCall(glEnable(GL_DEPTH_TEST));

		ShaderProgram& shader = *AssetManagement::GetShader(m_previewSpheresShader);
		Mesh& mesh = *AssetManagement::GetMesh(m_cube);

		m_voxels_dilated->BindColorTexture(0, 6);

		shader.SetUniform("u_voxels", 6);

		shader.SetUniform("u_proj_view_matrix", proj_view);
		glm::mat4 scale_mat = glm::scale(glm::mat4(1.0f), glm::vec3(0.05f));
		shader.SetUniform("u_scale", scale_mat);

		shader.SetUniform("u_size", glm::ivec3(m_data.dimensions));
		shader.SetUniform("uniform_bbox_min", m_data.voxelizationArea.GetMin());
		shader.SetUniform("uniform_bbox_max", m_data.voxelizationArea.GetMax());
		
		shader.Bind();

		mesh.m_vertexArray.Bind();
		mesh.m_indexBuffer.Bind();

		int number_of_spheres = m_data.voxel_grid_size;
		glDrawElementsInstanced(GL_TRIANGLES, mesh.m_indexBuffer.GetCount(), GL_UNSIGNED_INT, nullptr, number_of_spheres);

		mesh.m_vertexArray.UnBind();
		mesh.m_indexBuffer.UnBind();

		framebuffer.UnBind();
	}

	const FrameBuffer& Voxelizer::GetVoxels() const
	{
		return *m_voxels_dilated;
	}

	void Voxelizer::ThreeWayStep(Scene& scene)
	{
		m_strategie->ClearFrameBuffer();
		scene.Draw(*m_strategie, glm::identity<glm::mat4>(), glm::identity<glm::mat4>());
	}

	void Voxelizer::MergeStep()
	{
		m_merge_voxels->Bind();

		glCall(glViewport(0, 0, (u32)m_data.dimensions.x, (u32)m_data.dimensions.y));
		glCall(glClearColor(0.0f, 0.0f, 0.0f, 0.0f));
		glCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
		glCall(glDisable(GL_DEPTH_TEST));

		m_strategie->GetFrameBuffer().BindColorTexture(0, 1);

		ShaderProgram& shader = *AssetManagement::GetShader(m_mergeShader);
		glm::ivec3 u_dimensions(m_data.dimensions);

		shader.SetUniform("u_sampler_three_way", 1);
		shader.SetUniform("u_dimensions", u_dimensions);


		shader.Bind();
		AssetManagement::GetMesh(m_screen_filled_quad)->Draw();
		shader.UnBind();

		m_merge_voxels->UnBind();
	}

	void Voxelizer::DilationStep()
	{
		m_voxels_dilated->Bind();

		glCall(glViewport(0, 0, (u32)m_data.dimensions.x, (u32)m_data.dimensions.y));
		glCall(glClearColor(0.0f, 0.0f, 0.0f, 0.0f));
		glCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
		glCall(glDisable(GL_DEPTH_TEST));

		m_merge_voxels->BindColorTexture(0, 1);

		ShaderProgram& shader = *AssetManagement::GetShader(m_dilationShader);

		// set uniforms
		shader.SetUniform("u_voxels", 1);

		shader.Bind();
		AssetManagement::GetMesh(m_screen_filled_quad)->Draw();
		shader.UnBind();

		m_voxels_dilated->UnBind();
	}


}


