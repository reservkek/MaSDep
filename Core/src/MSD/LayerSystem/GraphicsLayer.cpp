#include "msdpch.h"

#include "GraphicsLayer.h"
#include "GLFW/glfw3.h"

namespace MSD {
	GraphicsLayer::GraphicsLayer() : Layer("GraphicsLayer")
	{
	}

	GraphicsLayer::~GraphicsLayer()
	{
	}

	void GraphicsLayer::OnEvent(Event& event)
	{
		EventDispatcher dispatcher(event);
		dispatcher.Dispatch<KeyPressedEvent>(BIND_EVENT_FN(GraphicsLayer::OnKeyPressedEvent));
	}

	bool GraphicsLayer::OnKeyPressedEvent(KeyPressedEvent& event)
	{
		return false;
	}

	void GraphicsLayer::OnAttach()
	{
		va.reset(new VertexArray);
		vb.reset(new VertexBuffer(coords, 4 * 3 * sizeof(float)));
		ib.reset(new IndexBuffer(indices, 6));
		fb.reset(new FrameBuffer(spec));

		vb->SetLayout(layout);
		va->AddVertexBuffer(vb);
		va->SetIndexBuffer(ib);

		camera = new OrthographicCamera(-400.0f, 400.0f, -400.0f, 400.0f);

		shader = new Shader("C:/Users/eeo5/Documents/Научная работа/VSProjects/CppGUI/Core/res/shaders/Basic.shader");
		shader->Bind();
		shader->SetUniform4f("u_Color", 0.7f, 0.2f, 0.7f, 1.0f);
		shader->SetUniformMat4("u_MVP", mvp);
	}

	void GraphicsLayer::OnDetach()
	{
	}

	void GraphicsLayer::OnUpdate(Timestep ts)
	{
		if (Input::IsKeyPressed(GLFW_KEY_LEFT))
		{
			m_CameraPosition.x -= m_CameraSpeed*ts;
		}
		if (Input::IsKeyPressed(GLFW_KEY_RIGHT))
		{
			m_CameraPosition.x += m_CameraSpeed*ts;
		}
		if (Input::IsKeyPressed(GLFW_KEY_UP))
		{
			m_CameraPosition.y += m_CameraSpeed*ts;
		}
		if (Input::IsKeyPressed(GLFW_KEY_DOWN))
		{
			m_CameraPosition.y -= m_CameraSpeed*ts;
		}
		fb->Bind();

		renderer.Clear();

		camera->SetPositon(m_CameraPosition);

		renderer.Submit(va);

		shader->SetUniform4fv("u_Color", color);

		shader->SetUniformMat4("u_MVP", camera->GetViewProjectionMatrix());

		fb->Unbind();
	}
}