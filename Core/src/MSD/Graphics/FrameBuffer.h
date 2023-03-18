#pragma once

#include "Core.h"

namespace MSD {

	enum class FrameBufferTextureFormat
	{
		None = 0,

		// Color
		RGBA8,
		RED_INTEGER,

		// Depth/Stencil
		DEPTH24STENCIL8,

		// Defaults
		Depth = DEPTH24STENCIL8
	};

	struct FrameBufferTextureSpecification
	{
		FrameBufferTextureSpecification() = default;
		FrameBufferTextureSpecification(FrameBufferTextureFormat format)
			: TextureFormat(format) {}

		FrameBufferTextureFormat TextureFormat = FrameBufferTextureFormat::None;
	};

	struct FrameBufferAttachmentSpecification
	{
		FrameBufferAttachmentSpecification() = default;
		FrameBufferAttachmentSpecification(std::initializer_list<FrameBufferTextureSpecification> attachments)
			: Attachments(attachments) {};

		std::vector <FrameBufferTextureSpecification> Attachments;
	};

	struct FrameBufferSpecification
	{
		unsigned int Width = 800, Height = 800;
		unsigned int Samples = 1;

		FrameBufferAttachmentSpecification Attachments;
		
		bool SwapChainTarget = false;
	};

	class FrameBuffer
	{
	public:
		FrameBuffer(const FrameBufferSpecification& spec);
		~FrameBuffer();

		FrameBuffer* Create(const FrameBufferSpecification& spec) { return new FrameBuffer(spec); };
		FrameBufferSpecification& GetSpecification() { return m_Specification; };
		const unsigned long long GetColorAttachment(unsigned int index = 0) const { return m_ColorAttachments[index]; };

		void Recreate();
		void ChangeFrameBufferSize(int width, int height);

		void ClearObjectIndices(unsigned int attachmentIndex, int clearValue);

		void UpdateSpec(const FrameBufferSpecification& spec);

		int ReadPixel(unsigned int attachmentIndex, int x, int y);

		void Bind();
		void Unbind();
	private:
		unsigned int m_FrameBufferID;
		FrameBufferSpecification m_Specification;

		std::vector<FrameBufferTextureSpecification> m_ColorAttachmentSpecifications;
		FrameBufferTextureSpecification m_DepthAttachmentSpecification;

		std::vector<unsigned int> m_ColorAttachments;
		unsigned int m_DepthAttachment = 0;

		int m_PixelData;
	};

}