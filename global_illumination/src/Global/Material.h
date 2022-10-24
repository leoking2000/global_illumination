#pragma once
#include "Types.h"
#include <string>

namespace GL
{
	class Material
	{
	public:
		glm::vec3 Albedo = glm::vec3(1.0f, 0.0f, 0.0f);
		std::string AlbedoMap;

		std::string NormalMap;

		f32 Roughness = 0.5f;
		std::string RoughnessMap;

		f32 Metallic = 0.0f;
		std::string MetallicMap;
	};

}