#pragma once
#include "Global/Types.h"

namespace GL
{
	class AtomicCounter
	{
	public:
		AtomicCounter();

		AtomicCounter(const AtomicCounter& other) = delete;
		AtomicCounter& operator=(const AtomicCounter&) = delete;

		//AtomicCounter(AtomicCounter&& other);
		//AtomicCounter& operator=(AtomicCounter&& other);

		~AtomicCounter();
	public:
		void Bind() const;
		void UnBind() const;

		u32 Get();
		void Reset();
	private:
		u32 m_id;
	};
}