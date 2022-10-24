#pragma once
#include "Graphics/Texture.h"
#include <string>

namespace GL
{
	class TextureFactory
	{
	public:
		static Texture Create(const std::string& file_path);
	private:
		static Texture ErrorTexture();
	};


}