#pragma once
#include "Texture.h"
#include <vector>

namespace GL
{
	class FrameBuffer
	{
	public:
		FrameBuffer(u32 width, u32 height, u8 colorAttachmentCount, 
			TextureFormat format = TextureFormat::RGBA32F, bool layered = false);
		~FrameBuffer();

		FrameBuffer(FrameBuffer&& other);
	public:
		void Bind() const;
		void UnBind() const;

		void BindColorTexture(u8 index, u32 slot) const;
		void BindDepthTexture(u32 slot) const;

		void UnBindColorTexture(u8 index) const;
		void UnBindDepthTexture() const;

		void Resize(u32 width, u32 height);

		u8 NumberOfColorAttachments() const;

		u32 Width() const;
		u32 Height() const;
	private:
		u32 m_width;
		u32 m_height;

		u32 m_id;
		Texture m_depth_texture;
		std::vector<Texture> m_color_attachments;
	};


}