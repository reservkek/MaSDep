#include "msdpch.h"

#include "GraphicsLayer.h"
#include "GLFW/glfw3.h"

#include "Controller.h"
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
		Controller::CameraOnEvent(event);
	}

	void GraphicsLayer::OnAttach()
	{
		va.reset(new VertexArray);
		vb.reset(new VertexBuffer(Rect::coords, 4*3*sizeof(float)));
		ib.reset(new IndexBuffer(Rect::indices, 6));

		fbSpec.Attachments = { FrameBufferTextureFormat::RGBA8, FrameBufferTextureFormat::RED_INTEGER,
			FrameBufferTextureFormat::RED_INTEGER, FrameBufferTextureFormat::Depth };

		fb.reset(new FrameBuffer(fbSpec));

		vb->SetLayout(layout);
		va->AddVertexBuffer(vb);
		va->SetIndexBuffer(ib);

		camera = new OrthographicCamera(-400.0f, 400.0f, -400.0f, 400.0f);

		shader = new Shader("../assets/shaders/Basic.glsl");
		shader->Bind();
		shader->SetUniform4f("u_Color", 0.7f, 0.2f, 0.7f, 1.0f);

		gridShader = new Shader("../assets/shaders/Grid.glsl");


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
			Controller::HandleObjectInputs(m_Model->m_Substrate, &ts);
		}

		fb->Bind();

		renderer.BeginScene(camera, shader);
		renderer.Clear();

		fb->ClearAttachment(1, -1);
		fb->ClearAttachment(2, 0);

		Arrow myFirstArrow;

 
		glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.1f));
		
		gridShader->Bind();
		gridShader->SetUniformMat4("u_ViewProjection", camera->GetViewProjectionMatrix());
		renderer.DrawGrid(gridShader);

		shader->Bind();
		shader->SetUniform4fv("u_Color", color);

		renderer.DrawArrow(&myFirstArrow);
		renderer.DrawScene();

		/*renderer.DrawRect(nullptr, { 0.0f, 0.0f, 0.0f });*/

		shader->SetUniformMat4("u_ViewProjection", camera->GetViewProjectionMatrix());

		fb->Unbind();
	}
	void GraphicsLayer::SetSelectedItem(int id)
	{
		Renderer::GetSelectedItem() = id;
	}

	void GraphicsLayer::UpdateObjects()
	{
		renderer.Flush();

		auto s = renderer.CreateRect();
		s->SetID(99999);
		s->SetPosition(m_Model->m_Substrate->GetPos() * 10.0f);
		s->SetColor({ 0.9f, 0.05f, 0.05f, 1.0f });
		s->SetScale({ 0.2f, 0.03f, 1.0f });
		s->SetAngle(m_Model->m_Substrate->CalcAngle());
		s->CalcModelMatrix();

		m_Model->m_Substrate->SetGraphicsObject(s);

		for (auto magnetron : m_Model->m_Magnetrons)
		{
			auto m = renderer.CreateRect();
			m->SetID(magnetron->GetIndex());
			auto pos = magnetron->GetPos() * 10.0f;
			m->SetPosition(pos);
			m->SetScale({ 0.45f, 0.04f, 1.0f });
			m->SetAngle(magnetron->CalcAngle());
			m->CalcModelMatrix();
			magnetron->SetGraphicsObject(m);
		}
	}
	void GraphicsLayer::UpdateObjectStates()
	{
		auto& substrate = m_Model->m_Substrate;
		auto s = substrate->GetGraphicsObject();
		s->SetPosition(substrate->GetPos() * 10.0f);
		s->SetAngle(m_Model->m_Substrate->CalcAngle());
		s->CalcModelMatrix();

		for (auto magnetron : m_Model->m_Magnetrons)
		{
			auto m = magnetron->GetGraphicsObject();
			float scaleX = *magnetron->GetRadius() / 10.0f;
			m->SetPosition(magnetron->GetPos() * 10.0f);
			m->SetScale({ scaleX , 0.04f, 1.0f });
			m->SetAngle(magnetron->CalcAngle());
			m->CalcModelMatrix();
		}
	}
}