#pragma once
#include "Graphics/Texture.h"
#include <string>

namespace GL
{
	class TextureFactory
	{
	public:
		static Texture Create(const std::string& file_path);
		static Texture CreateRandom1D(u32 size);
	private:
		static Texture ErrorTexture();
	};


}