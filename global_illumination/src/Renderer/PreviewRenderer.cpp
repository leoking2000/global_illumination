#include "Graphics/OpenGL.h"
#include "AssetManagement/AssetManagement.h"
#include "Voxelization/Voxelizer.h"
#include "PreviewRenderer.h"

namespace GL
{
	PreviewRenderer::PreviewRenderer()
	{
		m_PreviewCubesShader = AssetManagement::CreateShader("Preview\\BinaryPreviewCubes");
		m_cube = AssetManagement::CreateMesh(DefaultShape::CUBE);
	}

	void PreviewRenderer::DrawVoxels(const FrameBuffer& geometryBuffer, const glm::mat4& proj_view, const VoxelizerData& data,
		const FrameBuffer& voxelBuffer, bool is_musked, bool show_all)
	{
		geometryBuffer.Bind();

		glCall(glDisable(GL_BLEND));
		glCall(glDisable(GL_CULL_FACE));
		glCall(glEnable(GL_DEPTH_TEST));

		ShaderProgram& shader = *AssetManagement::GetShader(m_PreviewCubesShader);
		Mesh& mesh = *AssetManagement::GetMesh(m_cube);

		shader.SetUniform("u_show_all", show_all ? 1 : 0);

		if (is_musked)
		{
			shader.SetUniform("u_use_musked", 1);
			shader.SetUniform("u_voxels_musked", 9);
			voxelBuffer.BindColorTexture(0, 9);
		}
		else
		{
			shader.SetUniform("u_use_musked", 0);
			shader.SetUniform("u_voxels_3D", 8);
			voxelBuffer.BindColorTexture(0, 8);
		}

		shader.SetUniform("u_proj_view_matrix", proj_view);
		glm::mat4 scale_mat = glm::scale(glm::mat4(1.0f), glm::vec3(0.05f));
		shader.SetUniform("u_scale", scale_mat);

		shader.SetUniform("u_size", glm::ivec3(data.dimensions));
		shader.SetUniform("uniform_bbox_min", data.voxelizationArea.GetMin());
		shader.SetUniform("uniform_bbox_max", data.voxelizationArea.GetMax());

		shader.Bind();

		mesh.m_vertexArray.Bind();
		mesh.m_indexBuffer.Bind();

		int number_of_spheres = data.voxel_grid_size;
		glDrawElementsInstanced(GL_TRIANGLES, mesh.m_indexBuffer.GetCount(), GL_UNSIGNED_INT, nullptr, number_of_spheres);

		mesh.m_vertexArray.UnBind();
		mesh.m_indexBuffer.UnBind();

		shader.UnBind();

		geometryBuffer.UnBind();
	}
}


