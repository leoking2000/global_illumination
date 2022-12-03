#pragma once
#include "Global/Types.h"

namespace GL
{
	class Box
	{
	public:
		Box(const glm::vec3& center_point, const glm::vec3& box_size);
	public:
		inline glm::vec3 GetSize(void) const { return m_size; }
		inline glm::vec3 GetCenter(void) const { return m_center; }
		inline glm::vec3 GetMin(void) const { return m_min; }
		inline glm::vec3 GetMax(void) const { return m_max; }

		f32 GetMaxSide() const;
		f32 GetMinSide() const;

		void Set(const glm::vec3& center_point, const glm::vec3& box_size);
	private:
		glm::vec3 m_min;
		glm::vec3 m_max;
		glm::vec3 m_size;
		glm::vec3 m_center;
	};
}