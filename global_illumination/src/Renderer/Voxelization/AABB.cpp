#include "AABB.h"

namespace GL
{
	AABB::AABB()
	{
		Set(glm::vec3(0.0f), glm::vec3(0.0f));
	}

	AABB::AABB(const glm::vec3& center_point, const glm::vec3& box_size)
	{
		Set(center_point, box_size);
	}

	f32 AABB::GetMaxSide() const
	{
		f32 maxside;

		if (m_size.x > m_size.y)
		{
			if (m_size.x > m_size.z)
				maxside = m_size.x;
			else
				maxside = m_size.z;
		}
		else
		{
			if (m_size.y > m_size.z)
				maxside = m_size.y;
			else
				maxside = m_size.z;
		}

		return maxside;
	}

	f32 AABB::GetMinSide() const
	{
		f32 minside;

		if (m_size.x < m_size.y)
		{
			if (m_size.x < m_size.z)
				minside = m_size.x;
			else
				minside = m_size.z;
		}
		else
		{
			if (m_size.y < m_size.z)
				minside = m_size.y;
			else
				minside = m_size.z;
		}

		return minside;
	}

	void AABB::Set(const glm::vec3& center_point, const glm::vec3& box_size)
	{
		m_size = box_size;
		m_center = center_point;
		m_min = m_center - m_size * 0.5f;
		m_max = m_center + m_size * 0.5f;
	}
}


