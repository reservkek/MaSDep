#include "msdpch.h"

#include "Renderer.h"
#include "glad/glad.h"

namespace MSD {

	int Renderer::s_SelectedID = -1;

	void Renderer::Clear() const
	{
		//glClearColor(0.94f, 0.94f, 0.94f, 1.0f);
		glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
	}

	void Renderer::BeginScene(OrthographicCamera* camera, Shader* shader)
	{
		m_Camera = camera;
		m_shader = shader;
	}

	void Renderer::EndScene()
	{
	}

	void Renderer::Submit(Shader* shader, const std::shared_ptr<VertexArray> va, glm::mat4 ModelMatrix)
	{
		shader->Bind();
		shader->SetUniformMat4("u_Model", ModelMatrix);
		va->Bind();
		Draw(va);
	}

	void Renderer::Draw(const std::shared_ptr<VertexArray> va)
	{
		glDrawElements(GL_TRIANGLES, va->GetIndexBuffer()->GetCount(), GL_UNSIGNED_INT, nullptr);
	}

	void Renderer::DrawLines(const std::shared_ptr<VertexArray> va)
	{
		glLineWidth(2);
		glDrawElements(GL_LINES, va->GetIndexBuffer()->GetCount(), GL_UNSIGNED_INT, nullptr);
	}

	void Renderer::DrawGrid()
	{
		//m_va.reset(new VertexArray());
		//m_vb.reset(new VertexBuffer(Grid::u_Vertices));
		//m_ib.reset(new IndexBuffer(Grid::u_Indices));

		//m_vb->SetLayout(Grid::u_BasicLayout);
		//m_va->AddVertexBuffer(m_vb);
		//m_va->SetIndexBuffer(m_ib);

		//m_shader->Bind();
		//m_shader->SetUniform4fv("u_Color", Grid::u_Color);
		//m_shader->SetUniformMat4("u_Model", Grid::u_ModelMatrix);

		//m_va->Bind();
		//DrawLines(m_va);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 6);
	}

	Rect* Renderer::CreateRect()
	{
		Rect* obj = new Rect();
		m_Objects.push_back(obj);
		return obj;
	}

	void Renderer::DrawRect(Object* rect, glm::vec3 position)
	{
		if (rect == nullptr)
			rect = m_Objects.back();

		rect->SetPosition(position);

		m_va.reset(new VertexArray());
		m_vb.reset(new VertexBuffer(Rect::coords, 3 * 4 * sizeof(float)));
		m_ib.reset(new IndexBuffer(Rect::indices, 6));


		m_vb->SetLayout(rect->GetLayout());
		m_va->AddVertexBuffer(m_vb);
		m_va->SetIndexBuffer(m_ib);

		m_shader->Bind();
		m_shader->SetUniform4fv("u_Color", rect->GetColor());
		m_shader->SetUniformMat4("u_Model", rect->GetModelMatrix());
		m_shader->SetUniform1i("u_ID", rect->GetID());

		m_va->Bind();
		Draw(m_va);

		if (s_SelectedID == rect->GetID())
		{
			m_ib.reset(new IndexBuffer(Rect::outlineIndices, 8));
			m_va->SetIndexBuffer(m_ib);
			m_shader->SetUniform4fv("u_Color", rect->GetOutlineColor());
			m_shader->SetUniform1i("u_Border", 1);
			DrawLines(m_va);
		}
	}

	void Renderer::DrawCube(glm::vec3 position)
	{
		Rect* obj = new Rect();
		m_Objects.push_back(obj);
		obj->SetPosition(position);

		m_va.reset(new VertexArray());
		m_vb.reset(new VertexBuffer(Rect::coords, 3 * 8 * sizeof(float)));
		m_ib.reset(new IndexBuffer(Rect::indices, 36));


		m_vb->SetLayout(obj->GetLayout());
		m_va->AddVertexBuffer(m_vb);
		m_va->SetIndexBuffer(m_ib);

		m_shader->Bind();
		m_shader->SetUniform4fv("u_Color", obj->GetColor());
		m_shader->SetUniformMat4("u_Model", obj->GetModelMatrix());

		m_va->Bind();
		Draw(m_va);
	}

	void Renderer::DrawScene()
	{
		for (auto obj : m_Objects)
		{
			DrawRect(obj);
		}
	}

	void Renderer::SetObjectID(int index)
	{
		m_Objects.back()->SetID(index);
	}

	static int slices = 14;

	void Renderer::CalculateGrid()
	{
		for (int i = 0; i <= slices; i++)
			for (int j = 0; j <= slices ; j++)
			{
				float x = (1000.0f*(float)i / (float)slices) - 500.0f;
				float y = (1000.0f*(float)j / (float)slices) - 500.0f;
				float z = 0;
				Grid::u_Vertices.push_back(glm::vec3(x, y, z));
			}

		for (int j = 0; j < slices; ++j)
			for (int i = 0; i < slices; ++i)
			{
				int row1 = j * (slices + 1);
				int row2 = (j + 1) * (slices + 1);
				Grid::u_Indices.push_back(glm::uvec4(row1 + i, row1 + i + 1, row1 + i + 1, row2 + i + 1));
				Grid::u_Indices.push_back(glm::uvec4(row2 + i + 1, row2 + i, row2 + i, row1 + i));
			}
	}

	void Renderer::Flush()
	{
		m_Objects.clear();
	}

	void Renderer::GetOpenGLVersion()
	{
		std::cout << "" << std::endl;
		std::cout << "" << "OpenGL Vendor: " << glGetString(GL_VENDOR) << std::endl;
		std::cout << "" << "OpenGL Renderer: " << glGetString(GL_RENDERER) << std::endl;
		std::cout << "" << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;
		std::cout << "" << "OpenGL Shading Language Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
		std::cout << "" << std::endl;
	}

}
