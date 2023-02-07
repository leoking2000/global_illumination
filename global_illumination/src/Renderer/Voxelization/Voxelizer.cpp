#include "Graphics/OpenGL.h"
#include "Voxelizer.h"
#include "VoxelizerDrawStratagy.h"
#include "AssetManagement/AssetManagement.h"

#include "Global/Logger.h"

namespace GL
{
	Voxelizer::Voxelizer(const VoxelizerParameters& params)
		:
		m_data(params.center, params.size),
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

	const FrameBuffer& Voxelizer::GetVoxels() const
	{
		return m_voxels_dilated;
		//return m_merge_voxels;
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
		AssetManagement::GetMesh(m_screen_filled_quad)->Draw();
		shader.UnBind();

		m_voxels_dilated.UnBind();
	}
}


