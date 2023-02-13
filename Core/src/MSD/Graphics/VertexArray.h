#pragma once

#include "Buffer.h"

namespace MSD {

	class VertexBufferLayout;

	class VertexArray
	{
	public:
		VertexArray();
		~VertexArray();

		void AddVertexBuffer(const std::shared_ptr<VertexBuffer> vb);
		void SetIndexBuffer(const std::shared_ptr<IndexBuffer> ib);
		std::shared_ptr<IndexBuffer> GetIndexBuffer() const { return m_IndexBuffer; };

		void Bind() const;
		void Unbind() const;
	private:
		uint32_t m_RendererID;
		std::vector<std::shared_ptr<VertexBuffer>> m_VertexBuffers;
		std::shared_ptr<IndexBuffer> m_IndexBuffer;
	};

}


