#pragma once
#include "Global/Types.h"
#include "Global/Material.h"
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

	public:
		std::vector<u32> m_meshs; // stores ids the asset manager gives.
		std::vector<Material> m_materials;
	};


}
