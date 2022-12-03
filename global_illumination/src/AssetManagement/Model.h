#pragma once
#include "Global/Types.h"
#include "Factories/Material.h"
#include <vector>

namespace GL
{
	class Model
	{
	public:
		Model(const std::vector<u32>& meshs, const std::vector<Material>& materials)
			:
			m_meshs(meshs),
			m_materials(materials)
		{

		}
		Model(u32 mesh, Material mat)
		{
			m_meshs.push_back(mesh);
			m_materials.push_back(mat);
		}

	public:
		std::vector<u32> m_meshs; // stores ids the asset manager gives.
		std::vector<Material> m_materials;
	};


}
