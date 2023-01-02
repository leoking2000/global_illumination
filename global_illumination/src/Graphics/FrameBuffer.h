#pragma once
#include "Texture.h"
#include <optional>
#include <vector>

namespace GL
{
	enum class FrameBufferMode
	{
		ColorAttachment,
		Layered,
		Texture3D
	};

	class FrameBuffer
	{
	public:
		FrameBuffer(u32 width, u32 height, u32 depth, u32 colorAttachmentCount, 
			TextureMinFiltering min_filter, TextureMagFiltering mag_filter,
			TextureFormat format = TextureFormat::RGBA32F);

		FrameBuffer(u32 width, u32 height, u32 colorAttachmentCount,
			TextureMinFiltering min_filter, TextureMagFiltering mag_filter,
			TextureFormat format = TextureFormat::RGBA32F, FrameBufferMode fbt = FrameBufferMode::ColorAttachment);
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
		u32 Depth() const;
	private:
		void InitColorAttachmentMode(u32 width, u32 height, u32 colorAttachmentCount, 
			TextureMinFiltering min_filter, TextureMagFiltering mag_filter, TextureFormat format);
		void InitColorAttachmentMode3D(u32 width, u32 height, u32 depth, u32 colorAttachmentCount, 
			TextureMinFiltering min_filter, TextureMagFiltering mag_filter, TextureFormat format);

		void InitLayered(u32 width, u32 height, u32 colorAttachmentCount, 
			TextureMinFiltering min_filter, TextureMagFiltering mag_filter, TextureFormat format);
		void InitTexture3D(u32 width, u32 height, u32 colorAttachmentCount, 
			TextureMinFiltering min_filter, TextureMagFiltering mag_filter, TextureFormat format);

		u32 CheckColorAttachmentNumber(u32 colorAttachmentCount);
	private:
		u32 m_width;
		u32 m_height;
		u32 m_depth;

		u32 m_id;
		std::optional<Texture> m_depth_texture;
		std::vector<Texture> m_color_attachments;
	};

	u32 CheckFramebufferStatus(u32 framebuffer_object);
}