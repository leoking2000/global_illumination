#pragma once
#include "VoxelizerData.h"
#include "SceneGraph/Scene.h"
#include "Graphics/AtomicCounter.h"

namespace GL
{
	struct VoxelizerParameters
	{
		glm::vec3 center = glm::vec3(0.0f, 5.0f, 0.0f);
		glm::vec3 size = glm::vec3(30.0f);
		u32 resolution = 128;
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

		void ImGui();

		const FrameBuffer& GetVoxels(bool dilated = true) const;
		const VoxelizerData& GetData() const;

		u32 m_screen_filled_quad;
	private:
		void ThreeWayStep(Scene& scene);
		void MergeStep();
		void DilationStep();
	private:
		VoxelizerData m_data;

		std::unique_ptr<DrawStrategy> m_strategie;

		FrameBuffer m_merge_voxels;
		u32 m_mergeShader;

		FrameBuffer m_voxels_dilated;
		u32 m_dilationShader;

		AtomicCounter m_atomic_counter;
		u32 m_num_active_voxels;
	};
}