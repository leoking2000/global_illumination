#include "TextureFactory.h"
#include "Global/Logger.h"
#include <vector>
#include <random>

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

namespace GL
{
	Texture TextureFactory::Create(const std::string& file_path)
	{
		stbi_set_flip_vertically_on_load(1);

		int width;
		int height;
		int bpp;

		stbi_uc* data = stbi_load(file_path.c_str(), &width, &height, &bpp, 4);
		
		if (data == nullptr)
		{
			LOGERROR("Failed to Created Texture From: " + file_path);
			return ErrorTexture();
		}

		LOGINFO("Created Texture From: " + file_path);

		Texture tex(DIM_2D, { width, height, 0 }, TextureFormat::RGBA8UB,
			TextureMinFiltering::MIN_NEAREST_MIPMAP_NEAREST, TextureMagFiltering::MAG_LINEAR, 
			TextureWrapping::REPEAT, TextureWrapping::REPEAT, 
			data
		);

		stbi_image_free(data);

		return std::move(tex);
	}

	Texture TextureFactory::CreateRandom1D(u32 size)
	{
		std::vector<glm::vec4> image_data;
		image_data.resize(size);

		std::mt19937 gen(3190090);
		std::uniform_real_distribution<float> dis(-1.0, 1.0);

		for (u32 i = 0; i < image_data.size(); i++)
		{
			float x = dis(gen);
			float y = dis(gen);
			float z = dis(gen);
			float a = dis(gen);

			image_data[i] = glm::vec4(x, y, z, a);
		}

		return Texture(DIM_1D, { size, 0, 0 }, TextureFormat::RGBA32F,
			TextureMinFiltering::MIN_NEAREST, TextureMagFiltering::MAG_NEAREST,
			TextureWrapping::REPEAT, TextureWrapping::REPEAT, (u8*)image_data.data());
	}

	Texture TextureFactory::ErrorTexture()
	{
		u8 data[] = {
			255, 255, 255, 255, /**/  0,   0,   0, 255,
			  0,   0,   0, 255,     255, 255, 255, 255
		};

		return Texture(DIM_2D, { 2, 2, 0 }, TextureFormat::RGBA8UB,
			TextureMinFiltering::MIN_NEAREST, TextureMagFiltering::MAG_NEAREST, 
			TextureWrapping::REPEAT, TextureWrapping::REPEAT,
			data
		);
	}
}


