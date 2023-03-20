#include "msdpch.h"

#include "FrameBuffer.h"

#include "glad/glad.h"

namespace MSD {

	static const unsigned int s_MaxBufferSize = 8192;

	static bool isDepthFormat(FrameBufferTextureFormat format)
	{
		switch (format)
		{
		case FrameBufferTextureFormat::DEPTH24STENCIL8: return true;
		}

		return false;
	}

	static GLenum TextureTarget(bool multisampled)
	{
		return multisampled ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
	}

	static void CreateTextures(unsigned int* outID, unsigned int count, bool multisampled = 0)
	{
		glCreateTextures(TextureTarget(multisampled), count, outID);
	}

	static void BindTexture(bool multisampled, unsigned int id)
	{
		glBindTexture(TextureTarget(multisampled), id);
	}

	static void AttachColorTexture(unsigned int id, int samples, GLenum internalFormat,
		GLenum format, GLenum type, unsigned int width, unsigned int height, int index)
	{
		bool multisampled = samples > 1;
		if (multisampled)
		{
			glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, internalFormat, width, height, GL_FALSE);
		}
		else
		{
			glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, type, nullptr);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		}

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index,
			TextureTarget(multisampled), id, 0);
	}

	static void AttachDepthTexture(unsigned int id, int samples, GLenum format,
		GLenum attachmenttype, unsigned int width, unsigned int height)
	{
		bool multisampled = samples > 1;
		if (multisampled)
		{
			glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, format, width, height, GL_FALSE);
		}
		else
		{
			glTexStorage2D(GL_TEXTURE_2D, 1, format, width, height);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		}

		glFramebufferTexture2D(GL_FRAMEBUFFER, attachmenttype, TextureTarget(multisampled), id, 0);
	}

	FrameBuffer::FrameBuffer(const FrameBufferSpecification& spec)
	{
		m_Specification = spec;

		m_ColorAttachmentSpecifications.clear();
		for (auto spec : m_Specification.Attachments.Attachments)
		{
			if (!isDepthFormat(spec.TextureFormat))
				m_ColorAttachmentSpecifications.push_back(spec.TextureFormat);
			else m_DepthAttachmentSpecification = spec.TextureFormat;
		}

		Recreate();

	}

	FrameBuffer::~FrameBuffer()
	{
		glDeleteFramebuffers(1, &m_FrameBufferID);
		glDeleteBuffers((GLsizei)m_ColorAttachments.size(), m_ColorAttachments.data());
		glDeleteBuffers(1, &m_DepthAttachment);
	}

	void FrameBuffer::Recreate()
	{
		if (m_FrameBufferID)
		{
			glDeleteFramebuffers(1, &m_FrameBufferID);
			glDeleteBuffers((GLsizei)m_ColorAttachments.size(), m_ColorAttachments.data());
			glDeleteBuffers(1, &m_DepthAttachment);

			m_ColorAttachments.clear();
			m_DepthAttachment = 0;
		}

		glCreateFramebuffers(1, &m_FrameBufferID);
		glBindFramebuffer(GL_FRAMEBUFFER, m_FrameBufferID);

		// Attachments
		static bool multisample = m_Specification.Samples > 1;

		if (m_ColorAttachmentSpecifications.size())
		{
			m_ColorAttachments.resize(m_ColorAttachmentSpecifications.size());
			CreateTextures(m_ColorAttachments.data(), (unsigned int)m_ColorAttachments.size(), multisample);

			for (size_t i = 0; i < m_ColorAttachments.size(); i++)
			{
				BindTexture(multisample, m_ColorAttachments[i]);
				switch (m_ColorAttachmentSpecifications[i].TextureFormat)
				{
				case FrameBufferTextureFormat::RGBA8:
					AttachColorTexture(m_ColorAttachments[i], m_Specification.Samples,
						GL_RGBA8, GL_RGBA, GL_FLOAT, m_Specification.Width, m_Specification.Height, (int)i);
					break;
				case FrameBufferTextureFormat::RED_INTEGER:
					AttachColorTexture(m_ColorAttachments[i], m_Specification.Samples,
						GL_R32I, GL_RED_INTEGER, GL_INT, m_Specification.Width, m_Specification.Height, (int)i);
					break;
				}
			}
		}

		if (m_DepthAttachmentSpecification.TextureFormat != FrameBufferTextureFormat::None)
		{
			CreateTextures(&m_DepthAttachment, 1, multisample);
			BindTexture(multisample, m_DepthAttachment);
			switch (m_DepthAttachmentSpecification.TextureFormat)
			{
			case FrameBufferTextureFormat::DEPTH24STENCIL8:
				AttachDepthTexture(m_DepthAttachment, m_Specification.Samples, GL_DEPTH24_STENCIL8,
					GL_DEPTH_STENCIL_ATTACHMENT, m_Specification.Width, m_Specification.Height);
				break;
			}
		}

		if (m_ColorAttachments.size() > 1)
		{
			GLenum buffers[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
			glDrawBuffers((GLsizei)m_ColorAttachments.size(), buffers);
		}
		else if (m_ColorAttachments.empty())
		{
			// Only Depth
			glDrawBuffer(GL_NONE);
		}

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		{
			while (1) std::cout << "FRAMEBUFFER ERROR";
		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void FrameBuffer::ChangeFrameBufferSize(int width, int height)
	{
		m_Specification.Width = width;
		m_Specification.Height = height;
	}

	void FrameBuffer::ClearAttachment(unsigned int attachmentIndex, int clearValue)
	{
		glClearBufferiv(GL_COLOR, attachmentIndex, &clearValue);
	}

	void FrameBuffer::UpdateSpec(const FrameBufferSpecification& spec)
	{
		m_Specification = spec;
	}

	int FrameBuffer::ReadPixel(unsigned int attachmentIndex, int x, int y)
	{
		GLenum buffers[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
		glReadBuffer(buffers[attachmentIndex]);
		glReadPixels(x, y, 1, 1, GL_RED_INTEGER, GL_INT, &m_PixelData);
		return m_PixelData;
	}

	void FrameBuffer::Bind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_FrameBufferID);
		glViewport(0, 0, m_Specification.Width, m_Specification.Height);

	}

	void FrameBuffer::Unbind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

}