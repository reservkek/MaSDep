#include "msdpch.h"

#include "Buffer.h"
#include "Renderer.h"

#include "glad/glad.h"

namespace MSD {

	// VertexBuffer ///////////////////////////

	VertexBuffer::VertexBuffer(float* data, uint32_t size)
	{
		glCreateBuffers(1, &m_vbID);
		glBindBuffer(GL_ARRAY_BUFFER, m_vbID);
		glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
	}


	VertexBuffer::~VertexBuffer()
	{
		glDeleteBuffers(1, &m_vbID);
	}

	void VertexBuffer::Bind() const
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_vbID);
	}

	void VertexBuffer::Unbind() const
	{
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}


	// IndexBuffer /////////////////////////////

	IndexBuffer::IndexBuffer(const void* data, unsigned int count)
		: m_Count(count)
	{
		ASSERT(sizeof(unsigned int) == sizeof(GLuint));

		glCreateBuffers(1, &m_RendererID);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(unsigned int), data, GL_STATIC_DRAW);
	}

	IndexBuffer::~IndexBuffer()
	{
		glDeleteBuffers(1, &m_RendererID);
	}

	void IndexBuffer::Bind() const
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
	}

	void IndexBuffer::Unbind() const
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

}