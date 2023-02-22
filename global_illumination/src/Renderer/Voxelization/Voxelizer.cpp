#include "Graphics/OpenGL.h"
#include "Voxelizer.h"
#include "VoxelizerDrawStratagy.h"
#include "AssetManagement/AssetManagement.h"
#include "imgui/imgui.h"

#include "Global/Logger.h"

namespace GL
{
	Voxelizer::Voxelizer(const VoxelizerParameters& params)
		:
		m_data(AABB(params.center, params.size), params.resolution),
		m_merge_voxels((u32)m_data.dimensions.x, (u32)m_data.dimensions.y, 1,
			TextureMinFiltering::MIN_NEAREST, TextureMagFiltering::MAG_NEAREST, TextureFormat::RGBA32UI),
		m_voxels_dilated((u32)m_data.dimensions.x, (u32)m_data.dimensions.y, 1,
			TextureMinFiltering::MIN_NEAREST, TextureMagFiltering::MAG_NEAREST, TextureFormat::RGBA32UI)
	{

	}

	void Voxelizer::Init()
	{
		// Create the VoxelizerDrawStratagy //
		m_strategie = std::make_unique<VoxelizerDrawStratagy>(m_data);

		// Create merge shader //
		m_mergeShader = AssetManagement::CreateShader("Voxelization\\ThreeWayBinaryMerge");

		// Create dilation shader //
		m_dilationShader = AssetManagement::CreateShader("Voxelization\\DilateVoxelSpace");

		// make usefull meshs
		m_screen_filled_quad = AssetManagement::CreateMesh(DefaultShape::SCRERN_FILLED_QUARD);
	}

	void Voxelizer::Voxelize(Scene& scene)
	{
		ThreeWayStep(scene);
		MergeStep();
		DilationStep();
	}

	void Voxelizer::ImGui()
	{
		glm::vec3 center = m_data.voxelizationArea.GetCenter();
		glm::vec3 size   = m_data.voxelizationArea.GetSize();
		glm::ivec3 dimensions = (glm::ivec3)m_data.dimensions;

		if (ImGui::CollapsingHeader("Voxelizer"))
		{
			ImGui::Text("AABB Center (%f, %f, %f)", center.x, center.y, center.z);
			ImGui::Text("AABB Size   (%f, %f, %f)", size.x, size.y, size.z);
			ImGui::Text("------------------------");
			ImGui::Text("Voxel Grid size (%i, %i, %i)", dimensions.x, dimensions.y, dimensions.z);
			ImGui::Text("Active Voxels: %i/%i", m_num_active_voxels, m_data.voxel_grid_size);
		}
	}

	const FrameBuffer& Voxelizer::GetVoxels(bool dilated) const
	{
		if (dilated) {
			return m_voxels_dilated;
		}
		return m_merge_voxels;
	}

	const VoxelizerData& Voxelizer::GetData() const
	{
		return m_data;
	}

	void Voxelizer::ThreeWayStep(Scene& scene)
	{
		m_strategie->ClearFrameBuffer();
		scene.Draw(*m_strategie, glm::identity<glm::mat4>(), glm::identity<glm::mat4>());
	}

	void Voxelizer::MergeStep()
	{
		m_merge_voxels.Bind();

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

		m_merge_voxels.UnBind();
	}

	void Voxelizer::DilationStep()
	{
		m_atomic_counter.Reset();

		m_voxels_dilated.Bind();

		glCall(glViewport(0, 0, (u32)m_data.dimensions.x, (u32)m_data.dimensions.y));
		glCall(glClearColor(0.0f, 0.0f, 0.0f, 0.0f));
		glCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
		glCall(glDisable(GL_DEPTH_TEST));

		m_merge_voxels.BindColorTexture(0, 1);

		ShaderProgram& shader = *AssetManagement::GetShader(m_dilationShader);

		// set uniforms
		shader.SetUniform("u_voxels", 1);

		shader.Bind();
		m_atomic_counter.Bind();

		AssetManagement::GetMesh(m_screen_filled_quad)->Draw();

		m_atomic_counter.UnBind();
		shader.UnBind();

		m_voxels_dilated.UnBind();

		m_num_active_voxels = m_atomic_counter.Get();
	}
}


