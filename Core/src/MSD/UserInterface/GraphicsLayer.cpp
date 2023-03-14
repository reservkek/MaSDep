#include "msdpch.h"

#include "GraphicsLayer.h"
#include "GLFW/glfw3.h"
#include "Input/Controller.h"

#include "Graphics/Internal/Objects.h"

namespace MSD {

	bool GraphicsLayer::m_HandleInputs = false;

	GraphicsLayer::GraphicsLayer() : Layer("GraphicsLayer")
	{
	}

	GraphicsLayer::~GraphicsLayer()
	{
	}

	void GraphicsLayer::OnEvent(Event& event)
	{
		if (m_HandleInputs)
		{
			Controller::CameraOnEvent(event);
		}
	}

	void GraphicsLayer::OnAttach()
	{
		va.reset(new VertexArray);
		vb.reset(new VertexBuffer(Rect::coords, 4*3*sizeof(float)));
		ib.reset(new IndexBuffer(Rect::indices, 6));

		fbSpec.Attachments = { FrameBufferTextureFormat::RGBA8, FrameBufferTextureFormat::RED_INTEGER, FrameBufferTextureFormat::Depth };

		fb.reset(new FrameBuffer(fbSpec));

		vb->SetLayout(layout);
		va->AddVertexBuffer(vb);
		va->SetIndexBuffer(ib);

		camera = new OrthographicCamera(-400.0f, 400.0f, -400.0f, 400.0f);

		shader = new Shader("../assets/shaders/Basic.glsl");
		shader->Bind();
		shader->SetUniform4f("u_Color", 0.7f, 0.2f, 0.7f, 1.0f);

		gridShader = new Shader("../assets/shaders/Grid.glsl");

		renderer.CalculateGrid();
	}

	void GraphicsLayer::OnDetach()
	{
	}

	void GraphicsLayer::OnUpdate(Timestep ts)
	{
		if (!m_Updating) return;

		if (m_HandleInputs)
		{
			Controller::HandleCameraInputs(camera, &ts);
		}

		fb->Bind();

		renderer.BeginScene(camera, shader);

		renderer.Clear();

		glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.1f));
		
		gridShader->Bind();
		gridShader->SetUniformMat4("u_ViewProjection", camera->GetViewProjectionMatrix());

		shader->Bind();
		shader->SetUniform4fv("u_Color", color);
		for (int i = 0; i < 10; i++)
		{
			for (int j = 0; j < 10; j++)
			{
				glm::vec3 pos(i * 50.0f, j * 50.0f, 0.0f);
				glm::mat4 transform = glm::translate(glm::mat4(1.0f), pos)*scale;
				renderer.Submit(shader, va, transform);
			}
		}

		renderer.DrawRect({ -100.0f, -100.0f, 0.0f });

		shader->SetUniformMat4("u_ViewProjection", camera->GetViewProjectionMatrix());

		fb->Unbind();
	}
}