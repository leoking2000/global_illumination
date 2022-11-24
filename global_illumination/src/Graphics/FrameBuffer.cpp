#include "OpenGL.h"
#include "FrameBuffer.h"
#include "Global/Logger.h"

u32 CheckFramebufferStatus(u32 framebuffer_object);

namespace GL
{
    FrameBuffer::FrameBuffer(u32 width, u32 height, u8 colorAttachmentCount, TextureFormat format, bool layered)
        :
        m_depth_texture(DIM_2D, { width, height, 0 },
            TextureFormat::DEPTH_COMPONENT32F,
            TextureMinFiltering::MIN_NEAREST, TextureMagFiltering::MAG_NEAREST, 
            TextureWrapping::CLAMP_TO_EDGE, TextureWrapping::CLAMP_TO_EDGE, 0),
        m_width(width),
        m_height(height)
    {
        LOGDEBUG("FrameBuffer: Created with size (%i,%i) and %i color attachments", m_width, m_height, colorAttachmentCount);

        glCall(glGenFramebuffers(1, &m_id));

        glCall(glBindFramebuffer(GL_FRAMEBUFFER, m_id));

        GLint maxDrawBuf = 0;
        glCall(glGetIntegerv(GL_MAX_DRAW_BUFFERS, &maxDrawBuf));

        if (colorAttachmentCount == 0)
        {
            glCall(glDrawBuffer(GL_NONE));
            glCall(glReadBuffer(GL_NONE));
        }
        else if (colorAttachmentCount > maxDrawBuf)
        {
            LOGWARNING("FrameBuffer: Attempted to create with %i color attachments, %i will be created insted.", colorAttachmentCount, maxDrawBuf);
            colorAttachmentCount = maxDrawBuf;
        }

        if (layered)
        {
            Texture& tex = m_color_attachments.emplace_back(DIM_2D_ARRAY, Texture::TexSize(m_width, m_height, colorAttachmentCount), format,
                TextureMinFiltering::MIN_NEAREST, TextureMagFiltering::MAG_NEAREST,
                TextureWrapping::CLAMP_TO_EDGE, TextureWrapping::CLAMP_TO_EDGE, (u8*)0u
            );

            glCall(glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, tex.m_id, 0));
        }
        else
        {
            for (u8 i = 0; i < colorAttachmentCount; i++)
            {
                Texture& tex = m_color_attachments.emplace_back(DIM_2D, Texture::TexSize(m_width, m_height, 0), format,
                    TextureMinFiltering::MIN_NEAREST, TextureMagFiltering::MAG_NEAREST,
                    TextureWrapping::CLAMP_TO_EDGE, TextureWrapping::CLAMP_TO_EDGE, (u8*)0u
                );

                glCall(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, Texture::TYPE[tex.m_params.dimensions], tex.m_id, 0));
            }

            glCall(glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_depth_texture.m_id, 0));
        }

        CheckFramebufferStatus(m_id);
    }

    FrameBuffer::~FrameBuffer()
    {
        m_color_attachments.clear();

        glCall(glDeleteTextures(1, &m_depth_texture.m_id));
        m_depth_texture.m_id = 0;

        glCall(glDeleteFramebuffers(1, &m_id));
    }

    FrameBuffer::FrameBuffer(FrameBuffer&& other)
        :
        m_width(other.m_width),
        m_height(other.m_height),
        m_id(other.m_id),
        m_depth_texture(std::move(other.m_depth_texture)),
        m_color_attachments(std::move(other.m_color_attachments))
    {
        other.m_width = 0;
        other.m_height = 0;
        other.m_id = 0;
    }

    void FrameBuffer::Bind() const
    {
        glCall(glBindFramebuffer(GL_FRAMEBUFFER, m_id));

        constexpr static GLenum drawbuffers[32] = {
            GL_COLOR_ATTACHMENT0,
            GL_COLOR_ATTACHMENT1,
            GL_COLOR_ATTACHMENT2,
            GL_COLOR_ATTACHMENT3,
            GL_COLOR_ATTACHMENT4,
            GL_COLOR_ATTACHMENT5,
            GL_COLOR_ATTACHMENT6,
            GL_COLOR_ATTACHMENT7,
            GL_COLOR_ATTACHMENT8,
            GL_COLOR_ATTACHMENT9,
            GL_COLOR_ATTACHMENT10,
            GL_COLOR_ATTACHMENT11,
            GL_COLOR_ATTACHMENT12,
            GL_COLOR_ATTACHMENT13,
            GL_COLOR_ATTACHMENT14,
            GL_COLOR_ATTACHMENT15,
            GL_COLOR_ATTACHMENT16,
            GL_COLOR_ATTACHMENT17,
            GL_COLOR_ATTACHMENT18,
            GL_COLOR_ATTACHMENT19,
            GL_COLOR_ATTACHMENT20,
            GL_COLOR_ATTACHMENT21,
            GL_COLOR_ATTACHMENT22,
            GL_COLOR_ATTACHMENT23,
            GL_COLOR_ATTACHMENT24,
            GL_COLOR_ATTACHMENT25,
            GL_COLOR_ATTACHMENT26,
            GL_COLOR_ATTACHMENT27,
            GL_COLOR_ATTACHMENT28,
            GL_COLOR_ATTACHMENT29,
            GL_COLOR_ATTACHMENT30,
            GL_COLOR_ATTACHMENT31
        };

        glCall(glDrawBuffers((GLsizei)m_color_attachments.size(), drawbuffers));
    }

    void FrameBuffer::UnBind() const
    {
        glCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));
    }

    void FrameBuffer::BindColorTexture(u8 index, u32 slot) const
    {
        m_color_attachments[index].Bind(slot);
    }

    void FrameBuffer::BindDepthTexture(u32 slot) const
    {
        m_depth_texture.Bind(slot);
    }

    void FrameBuffer::UnBindColorTexture(u8 index) const
    {
        m_color_attachments[index].Bind();
    }

    void FrameBuffer::UnBindDepthTexture() const
    {
        m_depth_texture.UnBind();
    }

    void FrameBuffer::Resize(u32 width, u32 height)
    {
        m_width = width;
        m_height = height;

        m_depth_texture.Resize(Texture::TexSize(m_width, m_height, 0));

        for (Texture& tex : m_color_attachments)
        {
            tex.Resize(Texture::TexSize(m_width, m_height, 0));
        }

        GLenum status = CheckFramebufferStatus(m_id);
    }

    u8 FrameBuffer::NumberOfColorAttachments() const
    {
        return (u8)m_color_attachments.size();
    }

    u32 FrameBuffer::Width() const
    {
        return m_width;
    }

    u32 FrameBuffer::Height() const
    {
        return m_height;
    }
}

u32 CheckFramebufferStatus(u32 framebuffer_object)
{
    glCall(glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_object));
    u32 status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE)
    {
        LOGERROR("glCheckFramebufferStatus: error %i", status);
        switch (status)
        {
        case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
            LOGERROR("Incomplete attatchement");
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
            LOGERROR("Incomplete missing attachment");
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
            LOGERROR("Incomplete draw buffer");
            break;
        case GL_FRAMEBUFFER_UNSUPPORTED:
            LOGERROR("Unsupported");
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS:
            LOGERROR("Incomplete layer targets");
            break;
        default:
            LOGERROR("Default error");
            break;
        }
        assert(false);
        throw std::runtime_error("Frame Buffer Error");
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    return status;
}
