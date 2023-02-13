#include "msdpch.h"

#include "VertexArray.h"
#include "Buffer.h"
#include "Renderer.h"

#include <glad/glad.h>

namespace MSD {

	VertexArray::VertexArray()
	{
		glCreateVertexArrays(1, &m_RendererID);
	}

	VertexArray::~VertexArray()
	{
		glDeleteVertexArrays(1, &m_RendererID);
	}

	void VertexArray::Bind() const
	{
		glBindVertexArray(m_RendererID);
	}

	void VertexArray::Unbind() const
	{
		glBindVertexArray(0);
	}

	void VertexArray::AddVertexBuffer(const std::shared_ptr<VertexBuffer> vb)
	{
		glBindVertexArray(m_RendererID);
		vb->Bind();

		const auto& elements = vb->GetLayout().GetElements();

		unsigned int offset = 0;

		for (unsigned int i = 0; i < elements.size(); i++)
		{
			const auto& element = elements[i];
			glEnableVertexAttribArray(i);
			glVertexAttribPointer(i, 3, GL_FLOAT,
				GL_FALSE, 12, nullptr);
			offset += element.size;
		}

		m_VertexBuffers.push_back(vb);
	}

	void VertexArray::SetIndexBuffer(const std::shared_ptr<IndexBuffer> ib)
	{
		glBindVertexArray(m_RendererID);
		ib->Bind();
		m_IndexBuffer = ib;
	}

}
