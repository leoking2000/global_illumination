#pragma once
#include "Box.h"
#include "SceneGraph/Scene.h"

namespace GL
{
	struct VoxelizerParameters
	{
		//Box voxelizationArea = Box(glm::vec3(0.0f, 5.0f, 0.0f), glm::vec3(22.0f, 12.0f, 15.0f));
		Box voxelizationArea = Box(glm::vec3(0.0f, 5.0f, 0.0f), glm::vec3(22.0f));
		const u32 resolution = 128;
	};

	struct VoxelizerData
	{
		VoxelizerData(const Box& box, u32 r) 
			: 
			voxelizationArea(box), 
			resolution(r) 
		{
			glm::vec3 boxSize = voxelizationArea.GetSize();
			// the voxel resolution is set to be the resolution of the box's max side
			glm::vec3 ratio = boxSize / voxelizationArea.GetMaxSide();

			glm::ivec3 res(0);

			// given that, calculate a uniform sized voxel grid
			// the size of the voxel is the ratio between the size and the max side (1.0 for the max side)
			res = (f32)resolution * ratio + 0.5f;
			dimensions = glm::max(res, glm::ivec3(1));

			glm::vec3 voxel_side = boxSize / glm::vec3(dimensions);
			voxel_size = voxel_side.x;
			voxel_grid_size = dimensions.x * dimensions.y * dimensions.z;
		}

		Box voxelizationArea;
		u32 resolution;

		glm::vec3 dimensions;
		f32 voxel_size;
		u32 voxel_grid_size;
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
		void DrawPreviewSpheres(const FrameBuffer& framebuffer,const glm::mat4& proj_view);
		FrameBuffer& GetVoxels();
	private:
		VoxelizerData m_data;

		std::unique_ptr<DrawStrategy> m_strategie;

		std::unique_ptr<FrameBuffer> m_voxels;
		u32 m_mergeShader;

		u32 m_previewSpheresShader;

		u32 m_cube;
		u32 m_screen_filled_quad;
	private:
		//std::unique_ptr<Texture> _voxels;
	};
}