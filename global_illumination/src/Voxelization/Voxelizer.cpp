#include "Graphics/OpenGL.h"
#include "Voxelizer.h"
#include "VoxelizerDrawStratagy.h"
#include "AssetManagement/AssetManagement.h"

#include "Global/Logger.h"

namespace GL
{
	Voxelizer::Voxelizer(const VoxelizerParameters& params)
		:
		m_data(params.voxelizationArea, params.resolution)
	{

	}

	void Voxelizer::Init()
	{
		// Init Voxelizer Data //
		glm::vec3 boxSize = m_data.voxelizationArea.GetSize();
		// the voxel resolution is set to be the resolution of the box's max side
		glm::vec3 ratio = boxSize / m_data.voxelizationArea.GetMaxSide();

		glm::ivec3 res(0);

		// given that, calculate a uniform sized voxel grid
		// the size of the voxel is the ratio between the size and the max side (1.0 for the max side)
		res = (f32)m_data.resolution * ratio + 0.5f;
		m_data.dimensions = glm::max(res, glm::ivec3(1));

		glm::vec3 voxel_side = boxSize / glm::vec3(m_data.dimensions);
		m_data.voxel_size = voxel_side.x;
		m_data.voxel_grid_size = m_data.dimensions.x * m_data.dimensions.y * m_data.dimensions.z;

		// Create the VoxelizerDrawStratagy //
		m_strategie = std::make_unique<VoxelizerDrawStratagy>(m_data);

		// Create merge Framebuffer and shader //
		m_voxels = std::make_unique<FrameBuffer>(
			m_data.dimensions.x, m_data.dimensions.y, 1,
			TextureFormat::RGBA32F);

		m_mergeShader = AssetManagement::CreateShader("Voxelization\\ThreeWayBinaryMerge");

		// make preview shader
		m_previewSpheresShader = AssetManagement::CreateShader("Voxelization\\BinaryPreviewSpheres");

		// make usefull meshs
		m_screen_filled_quad = AssetManagement::CreateMesh(DefaultShape::SCRERN_FILLED_QUARD);
		m_cube = AssetManagement::CreateMesh(DefaultShape::CUBE);
	}

	void Voxelizer::Voxelize(Scene& scene)
	{
		VoxelizerDrawStratagy& vds = (VoxelizerDrawStratagy&)(*m_strategie);

		vds.ClearFrameBuffer();
		scene.Draw(vds, glm::identity<glm::mat4>(), glm::identity<glm::mat4>());

		m_voxels->Bind();

		glCall(glViewport(0, 0, (u32)m_data.dimensions.x, (u32)m_data.dimensions.y));
		glCall(glClearColor(0.0f, 0.0f, 0.0f, 0.0f));
		glCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
		glCall(glDisable(GL_DEPTH_TEST));

		vds.GetFrameBuffer().BindColorTexture(0, 1);

		ShaderProgram& shader = *AssetManagement::GetShader(m_mergeShader);
		glm::mat4x4 mat = glm::ortho<f32>(-1, 1, -1, 1, 1, -1);
		glm::ivec3 u_dimensions(m_data.dimensions);

		shader.SetUniform("u_mvp", mat);
		shader.SetUniform("u_sampler_three_way", 0);
		shader.SetUniform("u_dimensions", u_dimensions);


		shader.Bind();
		AssetManagement::GetMesh(m_screen_filled_quad)->Draw();
		shader.UnBind();

		m_voxels->UnBind();
	}

	void Voxelizer::DrawPreviewSpheres(const FrameBuffer& framebuffer, const glm::mat4& proj_view)
	{
		framebuffer.Bind();

		glCall(glDisable(GL_BLEND));
		glCall(glDisable(GL_CULL_FACE));
		glCall(glEnable(GL_DEPTH_TEST));

		ShaderProgram& shader = *AssetManagement::GetShader(m_previewSpheresShader);
		Mesh& mesh = *AssetManagement::GetMesh(m_cube);

		m_voxels->BindColorTexture(0, 6);

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

	FrameBuffer& Voxelizer::GetVoxels()
	{
		return *m_voxels;
	}


}


