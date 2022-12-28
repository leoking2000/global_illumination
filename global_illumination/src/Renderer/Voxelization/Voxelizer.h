#pragma once
#include "VoxelizerData.h"
#include "SceneGraph/Scene.h"

namespace GL
{
	struct VoxelizerParameters
	{
		glm::vec3 center = glm::vec3(0.0f, 5.0f, 0.0f);
		f32 size = 22.0f;
	};

	class Voxelizer
	{
	public:
		Voxelizer(const VoxelizerParameters& params);

		Voxelizer(const Voxelizer&) = delete;
		Voxelizer& operator=(const Voxelizer&) = delete;
	public:
		void Init();
		void Voxelize(Scene& scene);

		const FrameBuffer& GetVoxels(bool musked) const;
		const VoxelizerData& GetData() const;
	private:
		void ThreeWayStep(Scene& scene);
		void MergeStep();
		void DilationStep();
		void GridCreationStage(Scene& scene);
	private:
		VoxelizerData m_data;

		std::unique_ptr<DrawStrategy> m_strategie;

		FrameBuffer m_merge_voxels;
		u32 m_mergeShader;

		FrameBuffer m_voxels_dilated;
		u32 m_dilationShader;

		FrameBuffer m_grid_buffer;
		u32 m_grid_shader;

		u32 m_previewSpheresShader;

		u32 m_cube;
		u32 m_screen_filled_quad;
		
	};
}