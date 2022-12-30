#pragma once
#include "Types.h"
#include <vector>

namespace GL
{
	class RandomNumbers
	{
	public:
		static void Genarate();

		static const std::vector<glm::vec3>& GetHaltonSequence3DSphere();
		static const std::vector<glm::vec2>& GetHaltonSequence2D();
	private:
		static std::vector<glm::vec2> m_halton_sequence_2d;
		static std::vector<glm::vec3> m_halton_sequence_3d_sphere;
	private:
		static bool m_genatated;
		static void GenarateHaltonSequence3DSphere();
		static void GenarateHaltonSequence2D();
	};
}
