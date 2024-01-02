#include "msdpch.h"

#include "Renderer.h"
#include "glad/glad.h"

namespace MSD {

	int Renderer::s_SelectedID = -1;

	void Renderer::Clear() const
	{
		//glClearColor(0.94f, 0.94f, 0.94f, 1.0f);
		//glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		glClear(GL_DEPTH_BUFFER_BIT);
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

	void Renderer::DrawObject(Object* obj)
	{
		auto a = obj->GetType();
		switch (obj->GetType())
		{
		case ObjectType::Rect: DrawRect(obj);
			break;
		case ObjectType::Arrow: DrawArrow(obj);
			break;
		case ObjectType::Cube: DrawCube(obj);
		default: return;
		}
	}

	void Renderer::DrawLines(const std::shared_ptr<VertexArray> va, float thickness)
	{
		glLineWidth(thickness);
		glDrawElements(GL_LINES, va->GetIndexBuffer()->GetCount(), GL_UNSIGNED_INT, nullptr);
	}

	void Renderer::DrawPoints(const std::shared_ptr<VertexArray> va, float pointSize)
	{
		glPointSize(pointSize);
		glDrawElements(GL_POINTS, va->GetIndexBuffer()->GetCount(), GL_UNSIGNED_INT, nullptr);
	}

	void Renderer::DrawGrid(Shader* shader)
	{
		glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
		glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);

		glEnable(GL_LINE_SMOOTH);
		glEnable(GL_POLYGON_SMOOTH);

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

	void Renderer::DrawRect(Object* rect)
	{
		if (rect == nullptr)
			rect = m_Objects.back();

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

		if (s_SelectedID == rect->GetID() && s_SelectedID != -1)
		{
			m_ib.reset(new IndexBuffer(Rect::outlineIndices, 8));
			m_va->SetIndexBuffer(m_ib);
			m_shader->SetUniform4fv("u_Color", rect->GetOutlineColor());
			DrawLines(m_va, 3.0f);
		}
	}

	void Renderer::DrawCube(Object* cube)
	{
		m_va.reset(new VertexArray());
		m_vb.reset(new VertexBuffer(Cube::coords, 24 * sizeof(float)));
		m_ib.reset(new IndexBuffer(Cube::indices, 36));


		m_vb->SetLayout(cube->GetLayout());
		m_va->AddVertexBuffer(m_vb);
		m_va->SetIndexBuffer(m_ib);

		m_shader->Bind();
		m_shader->SetUniform4fv("u_Color", cube->GetColor());
		m_shader->SetUniformMat4("u_Model", cube->GetModelMatrix());

		m_va->Bind();
		Draw(m_va);

		{
			m_ib.reset(new IndexBuffer(Cube::outlineIndices, 24));
			m_va->SetIndexBuffer(m_ib);
			m_shader->SetUniform4fv("u_Color", cube->GetOutlineColor());
			DrawLines(m_va, 10.0f);
		}
	}

	void Renderer::DrawArrow(Object* arrow)
	{
		if (arrow == nullptr)
			return;

		m_va.reset(new VertexArray());
		m_vb.reset(new VertexBuffer(Arrow::coords, 3 * 5 * sizeof(float)));
		m_ib.reset(new IndexBuffer(Arrow::indicesLine, 2));

		m_vb->SetLayout(arrow->GetLayout());
		m_va->AddVertexBuffer(m_vb);
		m_va->SetIndexBuffer(m_ib);

		m_shader->Bind();
		m_shader->SetUniform4fv("u_Color", arrow->GetColor());
		m_shader->SetUniformMat4("u_Model", arrow->GetModelMatrix());
		m_shader->SetUniform1i("u_ID", arrow->GetID());


		m_va->Bind();
		DrawLines(m_va, 4);

		m_ib.reset(new IndexBuffer(Arrow::indicesHead, 3));
		m_va->SetIndexBuffer(m_ib);

		Draw(m_va);

		if (s_SelectedID == arrow->GetID() && s_SelectedID != -1)
		{
			m_ib.reset(new IndexBuffer(Arrow::outlineIndices, 4));
			m_va->SetIndexBuffer(m_ib);
			m_shader->SetUniform4fv("u_Color", arrow->GetOutlineColor());
			DrawPoints(m_va, 7.0f);
		}
	}

	void Renderer::DrawScene()
	{
		glEnable(GL_DEPTH_TEST);

		for (auto obj : m_Objects)
		{
			DrawObject(obj);
		}
	}

	void Renderer::DrawScene3D()
	{
		for (auto obj : m_Objects3D)
		{
			DrawObject(obj);
		}
	}

	void Renderer::AddExistingObject(Object* obj)
	{
		m_Objects.push_back(obj);
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
		m_Objects3D.clear();
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
