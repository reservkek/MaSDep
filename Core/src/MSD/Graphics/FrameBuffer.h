#pragma once

#include "Core.h"

namespace MSD {

	struct FrameBufferSpecification
	{
		int Width = 800, Height = 800;
		unsigned int Samples = 1;
		
		bool SwapChainTarget = false;
	};

	class FrameBuffer
	{
	public:
		FrameBuffer(const FrameBufferSpecification& spec);
		~FrameBuffer();

		FrameBuffer* Create(const FrameBufferSpecification& spec) { return new FrameBuffer(spec); };
		FrameBufferSpecification& GetSpecification() { return m_Specification; };
		const unsigned long long GetColorAttachment() const { return m_ColorAttachment; };

		void Recreate();

		void UpdateSpec(const FrameBufferSpecification& spec);

		void Bind();
		void Unbind();
	private:
		unsigned int m_FrameBufferID;
		unsigned int m_ColorAttachment, m_DepthAttachment;
		FrameBufferSpecification m_Specification;
	};

}