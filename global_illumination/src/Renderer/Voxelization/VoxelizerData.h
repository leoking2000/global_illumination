#pragma once
#include "AABB.h"
#include <assert.h>

namespace GL
{
	struct VoxelizerData
	{
		VoxelizerData(AABB new_box, u32 max_dimension)
		{
			assert(max_dimension <= 128);

			voxelizationArea = new_box;
			this->max_dimension = max_dimension;

			glm::vec3 boxSize = voxelizationArea.GetSize();
			// the max_dimension is set to be the dimension of the box's max side
			glm::vec3 ratio = boxSize / voxelizationArea.GetMaxSide();

			glm::ivec3 res(0);

			// given that, calculate a uniform sized voxel grid
			// the size of the voxel is the ratio between the size and the max side (1.0 for the max side)
			res = (f32)max_dimension * ratio + 0.5f;
			dimensions = glm::max(res, glm::ivec3(1));

			glm::vec3 voxel_side = boxSize / glm::vec3(dimensions);
			voxel_size = voxel_side.x;
			voxel_grid_size = u32(dimensions.x * dimensions.y * dimensions.z);
		}

		AABB voxelizationArea;
		u32 max_dimension;
		glm::vec3 dimensions;
		f32 voxel_size;
		u32 voxel_grid_size;
	};
}