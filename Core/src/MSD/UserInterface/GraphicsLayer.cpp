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

	static Shader* circleShader;

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

		circleShader = new Shader("../assets/shaders/Circle.glsl");
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

		fb->ClearAttachment(1, -1);
		fb->ClearAttachment(2, 0);
 
		glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.1f));
		
		gridShader->Bind();
		gridShader->SetUniformMat4("u_ViewProjection", camera->GetViewProjectionMatrix());
		renderer.DrawGrid();

		shader->Bind();
		shader->SetUniform4fv("u_Color", color);

		circleShader->Bind();
		

		renderer.DrawScene();

		/*renderer.DrawRect(nullptr, { 0.0f, 0.0f, 0.0f });*/

		shader->SetUniformMat4("u_ViewProjection", camera->GetViewProjectionMatrix());

		fb->Unbind();
	}
	void GraphicsLayer::SetSelectedItem(int id)
	{
		Renderer::GetSelectedItemID() = id;
	}

	void GraphicsLayer::UpdateObjects()
	{
		renderer.Flush();

		// Adding substrate
		auto substrate = m_Model->m_Substrate;
		auto s = substrate->GetGraphicsObject();
		renderer.AddExistingObject(s);

		s->SetID(substrate->GetID());
		s->SetColor({ 0.9f, 0.25f, 0.05f, 1.0f });
		s->SetScale({ 0.2f, 0.03f, 1.0f });

		// Adding substrate normal vector
		auto s_arrow = substrate->GetArrow();
		renderer.AddExistingObject(s_arrow);

		s_arrow->SetID(999999);
		s_arrow->SetColor({ 0.9f, 0.05f, 0.9f, 1.0f });
		s_arrow->SetScale({ 0.4f, 0.4f, 1.0f });

		// Adding magnetrons and magnetron normal vectors
		for (auto mpair : m_Model->m_Magnetrons)
		{
			auto& magnetron = mpair.second;

			auto m = magnetron->GetGraphicsObject();
			auto m_arrow = magnetron->GetArrow();

			renderer.AddExistingObject(m);
			renderer.AddExistingObject(m_arrow);

			m->SetID(magnetron->GetID());

			m_arrow->SetID(magnetron->GetID() + 50000);
			m_arrow->SetColor({ 0.9f, 0.05f, 0.9f, 1.0f });
			m_arrow->SetScale({ 0.4f, 0.4f, 1.0f });
		}

		UpdateObjectStates();
	}

	void GraphicsLayer::UpdateObjectStates()
	{
		auto& substrate = m_Model->m_Substrate;
		auto s = substrate->GetGraphicsObject();
		auto s_arrow = substrate->GetArrow();

		vec3 pos; float angle;
		if (m_Model->m_ShowMovementRealTime and m_Model->m_ModelRunning)
		{
			auto& substratebuffer = m_Model->m_SubstrateBuffer;
			pos = substratebuffer->GetPos() * 10.0f;
			angle = substratebuffer->CalcAngle();
		}
		else
		{
			pos = substrate->GetPos() * 10.0f;
			angle = substrate->CalcAngle();
		};

		s->SetPosition(pos);
		s->SetAngle(angle);

		s->CalcModelMatrix();

		s_arrow->SetPosition(pos);
		s_arrow->SetAngle(angle);

		s_arrow->CalcModelMatrix();

		for (auto mpair : m_Model->m_Magnetrons)
		{
			auto& magnetron = mpair.second;

			auto m = magnetron->GetGraphicsObject();
			auto m_arrow = magnetron->GetArrow();
			float scaleX = *magnetron->GetRadius() / 10.0f;

			auto pos = magnetron->GetPos() * 10.0f;
			auto angle = magnetron->CalcAngle();

			m->SetPosition(pos);
			m->SetScale({ scaleX , 0.04f, 1.0f });
			m->SetAngle(angle);

			m_arrow->SetPosition(pos);
			m_arrow->SetAngle(angle);
			m_arrow->CalcModelMatrix();

			m->CalcModelMatrix();
		}
	}
}