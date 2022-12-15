#include "OpenGL.h"
#include "Texture.h"
#include "Global/Logger.h"
#include <stdexcept>

namespace GL
{
	inline u32 Texture::TYPE[4] = {
		GL_TEXTURE_1D,
		GL_TEXTURE_2D,
		GL_TEXTURE_3D,
		GL_TEXTURE_2D_ARRAY
	};

	Texture::Texture(
		Texturedimensions dimensions, TexSize size,
		TextureFormat format,
		TextureMinFiltering min_filter, TextureMagFiltering mag_filter,
		TextureWrapping S, TextureWrapping T, u8* data
	)
		:
		m_params(dimensions, size, format, min_filter, mag_filter, S, T)
	{
		//if (IsTexSizeValid(m_params.size) == false)
		//{
			//LOGERROR("Can't Create Texture with one of the dimensions be 0");
			//throw std::runtime_error("Can't Create Texture");
		//}

		glCall(glGenTextures(1, &m_id));

		SetFiltering(m_params.min_filter, m_params.mag_filter);
		SetWrapping(m_params.wrapping_s, m_params.wrapping_t);
		SetImageData(data, m_params.format);
	}

	Texture::Texture(Texture&& other)
		:
		m_id(other.m_id),
		m_params(other.m_params)
	{
		other.m_id = 0;
	}

	Texture& Texture::operator=(Texture&& other)
	{
		glCall(glDeleteTextures(1, &m_id));
		m_id = other.m_id;
		m_params = other.m_params;
		other.m_id = 0;
		return *this;
	}

	Texture::~Texture()
	{
		glCall(glDeleteTextures(1, &m_id));
	}

	void Texture::Bind(u32 slot) const
	{
		glCall(glActiveTexture(GL_TEXTURE0 + slot));
		glCall(glBindTexture(TYPE[m_params.dimensions], m_id));
	}

	void Texture::UnBind() const
	{
		glCall(glBindTexture(TYPE[m_params.dimensions], 0));
	}

	void Texture::SetFiltering(TextureMinFiltering min_filter, TextureMagFiltering mag_filter)
	{
		glCall(glBindTexture(TYPE[m_params.dimensions], m_id));

		switch (m_params.min_filter)
		{
		case TextureMinFiltering::MIN_NEAREST:
			glCall(glTexParameterf(TYPE[m_params.dimensions], GL_TEXTURE_MIN_FILTER, GL_NEAREST));
			break;
		case TextureMinFiltering::MIN_LINEAR:
			glCall(glTexParameterf(TYPE[m_params.dimensions], GL_TEXTURE_MIN_FILTER, GL_LINEAR));
			break;
		case TextureMinFiltering::MIN_NEAREST_MIPMAP_NEAREST:
			glCall(glTexParameterf(TYPE[m_params.dimensions], GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST));
			m_minimap = true;
			break;
		case TextureMinFiltering::MIN_LINEAR_MIPMAP_NEAREST:
			glCall(glTexParameterf(TYPE[m_params.dimensions], GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST));
			m_minimap = true;
			break;
		case TextureMinFiltering::MIN_NEAREST_MIPMAP_LINEAR:
			glCall(glTexParameterf(TYPE[m_params.dimensions], GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR));
			m_minimap = true;
			break;
		case TextureMinFiltering::MIN_LINEAR_MIPMAP_LINEAR:
			glCall(glTexParameterf(TYPE[m_params.dimensions], GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR));
			m_minimap = true;
			break;
		}

		switch (m_params.mag_filter)
		{
		case TextureMagFiltering::MAG_NEAREST:
			glCall(glTexParameteri(TYPE[m_params.dimensions], GL_TEXTURE_MAG_FILTER, GL_NEAREST));
			break;
		case TextureMagFiltering::MAG_LINEAR:
			glCall(glTexParameteri(TYPE[m_params.dimensions], GL_TEXTURE_MAG_FILTER, GL_LINEAR));
			break;
		}

		glCall(glBindTexture(TYPE[m_params.dimensions], 0));
	}

	void Texture::SetWrapping(TextureWrapping S, TextureWrapping T)
	{
		glCall(glBindTexture(TYPE[m_params.dimensions], m_id));

		switch (m_params.wrapping_s)
		{
		case TextureWrapping::REPEAT:
			glCall(glTexParameteri(TYPE[m_params.dimensions], GL_TEXTURE_WRAP_S, GL_REPEAT));
			break;
		case TextureWrapping::MIRRORED_REPEAT:
			glCall(glTexParameteri(TYPE[m_params.dimensions], GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT));
			break;
		case TextureWrapping::CLAMP_TO_EDGE:
			glCall(glTexParameteri(TYPE[m_params.dimensions], GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
			break;
		case TextureWrapping::CLAMP_TO_BORDER:
			glCall(glTexParameteri(TYPE[m_params.dimensions], GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER));
			break;
		}

		switch (m_params.wrapping_t)
		{
		case TextureWrapping::REPEAT:
			glCall(glTexParameteri(TYPE[m_params.dimensions], GL_TEXTURE_WRAP_T, GL_REPEAT));
			break;
		case TextureWrapping::MIRRORED_REPEAT:
			glCall(glTexParameteri(TYPE[m_params.dimensions], GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT));
			break;
		case TextureWrapping::CLAMP_TO_EDGE:
			glCall(glTexParameteri(TYPE[m_params.dimensions], GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
			break;
		case TextureWrapping::CLAMP_TO_BORDER:
			glCall(glTexParameteri(TYPE[m_params.dimensions], GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER));
			break;
		}

		glCall(glBindTexture(TYPE[m_params.dimensions], 0));
	}

	void Texture::SetImageData(u8* data, TextureFormat format)
	{
		glCall(glBindTexture(TYPE[m_params.dimensions], m_id));

		GLint internalFormat;
		GLenum color_format;
		GLenum type;

		switch (m_params.format)
		{
		case TextureFormat::RGBA8UB:
			internalFormat = GL_RGBA8;
			color_format = GL_RGBA;
			type = GL_UNSIGNED_BYTE;
			break;
		case TextureFormat::RGBA16F:
			internalFormat = GL_RGBA16F;
			color_format = GL_RGBA;
			type = GL_FLOAT;
			break;
		case TextureFormat::RGBA32F:
			internalFormat = GL_RGBA32F;
			color_format = GL_RGBA;
			type = GL_FLOAT;
			break;
		case TextureFormat::RGBA32UI:
			internalFormat = GL_RGBA32UI;
			color_format = GL_RGBA_INTEGER;
			type = GL_UNSIGNED_INT;
			break;
		case TextureFormat::R32UI:
			internalFormat = GL_R32UI;
			color_format = GL_RGB;
			type = GL_UNSIGNED_INT;
			break;
		case TextureFormat::R32F:
			internalFormat = GL_RED;
			color_format = GL_RGB;
			type = GL_FLOAT;
			break;
		case TextureFormat::DEPTH_COMPONENT32F:
			internalFormat = GL_DEPTH_COMPONENT32F;
			color_format = GL_DEPTH_COMPONENT;
			type = GL_FLOAT;
			break;
		}

		switch (m_params.dimensions)
		{
		case DIM_1D:
			glCall(glTexImage1D(TYPE[m_params.dimensions], 0, 
				internalFormat, m_params.size.x, 0, color_format, type, data));
			break;
		case DIM_2D:
			glCall(glTexImage2D(TYPE[m_params.dimensions], 0, 
				internalFormat, m_params.size.x, m_params.size.y, 0, color_format, type, data));
			break;
		case DIM_3D:
			glCall(glTexImage3D(TYPE[m_params.dimensions], 0, 
				internalFormat, m_params.size.x, m_params.size.y, m_params.size.z, 0, color_format, type, data));
			break;
		case DIM_2D_ARRAY:
			glCall(glTexImage3D(TYPE[m_params.dimensions], 0,
				internalFormat, m_params.size.x, m_params.size.y, m_params.size.z, 0, color_format, type, data));
			break;
		}

		if (m_minimap)
		{
			glCall(glGenerateMipmap(TYPE[m_params.dimensions]));
		}
			

		glCall(glBindTexture(TYPE[m_params.dimensions], 0));
	}

	void Texture::Resize(const TexSize& new_size)
	{
		if (IsTexSizeValid(new_size) == false) 
			return;

		m_params.size = new_size;
		SetImageData(0, m_params.format);
	}

	bool Texture::IsTexSizeValid(const TexSize& new_size)
	{
		for (u32 i = 0; i < (u32)m_params.dimensions + 1; i++)
		{
			if (m_params.size[i] == 0)
			{
				return false;
			}
		}

		return true;
	}
}


