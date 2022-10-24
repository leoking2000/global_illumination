#pragma once
#include "Graphics/Shader.h"
#include <string>

namespace GL
{
	class ShaderFactory
	{
	public:
		static ShaderProgram Create(const std::string& name);
		static void Reload(ShaderProgram& shader, const std::string& name);
	};


}