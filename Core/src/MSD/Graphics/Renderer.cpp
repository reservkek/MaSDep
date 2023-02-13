#include "msdpch.h"

#include "Renderer.h"
#include "glad/glad.h"

namespace MSD {

	void Renderer::Clear() const
	{
		//glClearColor(0.94f, 0.94f, 0.94f, 1.0f);
		glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
	}

	void Renderer::BeginScene()
	{
	}

	void Renderer::EndScene()
	{
	}

	void Renderer::Submit(const std::shared_ptr<VertexArray> va)
	{
		va->Bind();
		Draw(va);
	}

	void Renderer::Draw(const std::shared_ptr<VertexArray> va)
	{
		glDrawElements(GL_TRIANGLES, va->GetIndexBuffer()->GetCount(), GL_UNSIGNED_INT, nullptr);
	}

	void Renderer::Flush()
	{
	}

}
