#pragma once
#include "Box.h"

namespace GL
{
	struct VoxelizerData
	{
		VoxelizerData(const glm::vec3 center, f32 size)
			:
			voxelizationArea(center, glm::vec3(size))
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
			voxel_grid_size = u32(dimensions.x * dimensions.y * dimensions.z);
		}

		static constexpr u32 resolution = 128;
		Box voxelizationArea;

		glm::vec3 dimensions;
		f32 voxel_size;
		u32 voxel_grid_size;
	};
}