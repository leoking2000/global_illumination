#pragma once
#include "Global/Types.h"

namespace GL
{
	enum Texturedimensions
	{
		DIM_1D = 0,
		DIM_2D = 1,
		DIM_3D = 2,
		DIM_2D_ARRAY = 3
	};

	enum class TextureFormat
	{
		RGBA8UB,
		RGBA16F,
		RGBA32F,
		RGBA32UI,
		R32UI,
		R32F,
		DEPTH_COMPONENT32F
	};

	enum class TextureWrapping
	{
		REPEAT,
		MIRRORED_REPEAT,
		CLAMP_TO_EDGE,
		CLAMP_TO_BORDER
	};

	enum class TextureMinFiltering
	{
		MIN_NEAREST,
		MIN_LINEAR,
		MIN_NEAREST_MIPMAP_NEAREST,
		MIN_LINEAR_MIPMAP_NEAREST,
		MIN_NEAREST_MIPMAP_LINEAR,
		MIN_LINEAR_MIPMAP_LINEAR
	};

	enum class TextureMagFiltering
	{
		MAG_NEAREST,
		MAG_LINEAR
	};

	class Texture
	{
		using TexSize = glm::vec<3, u32>;
	public:
		Texture(Texturedimensions dimensions, TexSize size, TextureFormat format,
			TextureMinFiltering min_filter, TextureMagFiltering mag_filter,
			TextureWrapping S, TextureWrapping T, u8* data
		);

		Texture(const Texture& other) = delete;
		Texture& operator=(const Texture& other) = delete;

		Texture(Texture&& other);
		Texture& operator=(Texture&& other);

		~Texture();
	public:
		void Bind(u32 slot = 0) const;
		void UnBind() const;

		void SetFiltering(TextureMinFiltering min_filter, TextureMagFiltering mag_filter);
		void SetWrapping(TextureWrapping S, TextureWrapping T);
		void SetImageData(u8* data, TextureFormat format);

		void Resize(const TexSize& new_size);
	public:
		inline Texturedimensions Dimensions() const { return m_params.dimensions; }
		inline TexSize Size() const { return m_params.size; }
	private:
		bool IsTexSizeValid(const TexSize& new_size);
	private:
		struct TextureParameters
		{
			TextureParameters(Texturedimensions dimensions, TexSize size,
				TextureFormat format,
				TextureMinFiltering min_filter, TextureMagFiltering mag_filter,
				TextureWrapping S, TextureWrapping T
			)
				:
				dimensions(dimensions),
				size(size),
				format(format),
				min_filter(min_filter),
				mag_filter(mag_filter),
				wrapping_s(S),
				wrapping_t(T)
			{}

			Texturedimensions dimensions;
			TexSize size;
			TextureFormat format;
			TextureMinFiltering min_filter;
			TextureMagFiltering mag_filter;
			TextureWrapping wrapping_s;
			TextureWrapping wrapping_t;
		};
	private:
		u32 m_id = 0;
		bool m_minimap = false;
		TextureParameters m_params;
	private:
		friend class FrameBuffer;
		static u32 TYPE[4];
	};
}