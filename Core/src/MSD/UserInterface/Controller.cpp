#include "msdpch.h"

#include "Controller.h"

#include "imgui.h"

namespace MSD {

	float Controller::s_CameraSpeed = 1000.0f;
	float Controller::s_CameraRotation = 180.0f;
	float Controller::s_CameraRotationVertical = 0.0f;
	float Controller::s_CameraRotationSpeed = 180.0f;
	float Controller::s_ZoomValue = 1.0f;


	glm::vec3 Controller::s_CameraPosition = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec2 Controller::s_CurrMousePos = glm::vec2(0.0f, 0.0f);
	glm::vec2 Controller::s_LastMousePos = glm::vec2(0.0f, 0.0f);
	glm::vec2 Controller::s_WindowSizeRatio = glm::vec2(0.0f, 0.0f);


	bool Controller::s_Draggable = false;
	bool Controller::s_EnableEvents = true;

	OrthographicCamera* Controller::s_Camera = nullptr;
	AngMSDObject* Controller::s_SelectedObject = nullptr;

	const glm::mat4 Controller::s_NullMatrix = glm::mat4(1.0f);

	///////////////////
	// CAMERA INPUTS // 
	///////////////////

	void Controller::HandleCameraInputs(OrthographicCamera* camera, Timestep* timestep)
	{
		s_Camera = camera;
		CameraOnUpdate(timestep);
	}

	void Controller::CameraOnUpdate(Timestep* timestep)
	{
		if (s_Camera == nullptr) return;

		if (Input::IsKeyPressed(GLFW_KEY_LEFT))
		{
			s_CameraPosition.x += s_CameraSpeed * (*timestep);
		}
		if (Input::IsKeyPressed(GLFW_KEY_RIGHT))
		{
			s_CameraPosition.x -= s_CameraSpeed * (*timestep);
		}
		if (Input::IsKeyPressed(GLFW_KEY_UP))
		{
			s_CameraPosition.y -= s_CameraSpeed * (*timestep);
		}
		if (Input::IsKeyPressed(GLFW_KEY_DOWN))
		{
			s_CameraPosition.y += s_CameraSpeed * (*timestep);
		}
		if (Input::IsKeyPressed(GLFW_KEY_Q))
		{
			s_CameraRotation -= s_CameraRotationSpeed * (*timestep);
		}
		if (Input::IsKeyPressed(GLFW_KEY_E))
		{
			s_CameraRotation += s_CameraRotationSpeed * (*timestep);
		}
		if (Input::IsKeyPressed(GLFW_KEY_W))
		{
			s_CameraRotationVertical -= s_CameraRotationSpeed * (*timestep);
		}
		if (Input::IsKeyPressed(GLFW_KEY_S))
		{
			s_CameraRotationVertical += s_CameraRotationSpeed * (*timestep);
		}

		s_Camera->SetPositon(s_CameraPosition);
		s_Camera->SetRotationAroundX(s_CameraRotationVertical);
		s_Camera->SetRotationAroundZ(s_CameraRotation);
	}

	void Controller::CameraOnEvent(Event& event)
	{
		if (s_Camera == nullptr) return;

		EventDispatcher dispatcher(event);
		dispatcher.Dispatch<MouseButtonPressedEvent>(BIND_EVENT_FN_STATIC(Controller::CameraEventMouseButtonPressed));
		dispatcher.Dispatch<MouseButtonReleasedEvent>(BIND_EVENT_FN_STATIC(Controller::CameraEventMouseButtonReleased));
		dispatcher.Dispatch<MouseMovedEvent>(BIND_EVENT_FN_STATIC(Controller::CameraEventMouseMoved));
		
		if (!s_EnableEvents) return;
		
		dispatcher.Dispatch<MouseScrolledEvent>(BIND_EVENT_FN_STATIC(Controller::CameraEventMouseScrolled));
		dispatcher.Dispatch<MouseButtonDoubleClickedEvent>(BIND_EVENT_FN_STATIC(Controller::CameraEventMouseDoubleClicked));
	}

	void Controller::EnableEvents()
	{
		s_EnableEvents = true;
	}

	void Controller::DisableEvents()
	{
		s_EnableEvents = false;
	}

	void Controller::ResetCameraPosition()
	{
		s_ZoomValue = 1.0f;
		s_CameraRotation = 180.0f;
		s_CameraRotationVertical = 0.0f;
		s_CameraPosition = glm::vec3(0.0f, 0.0f, 0.0f);

		s_Camera->SetPositon(s_CameraPosition);
		s_Camera->SetRotationAroundZ(s_CameraRotation);
		s_Camera->SetRotationAroundX(s_CameraRotationVertical);
		s_Camera->SetZoomLevel(s_ZoomValue);
	}

	bool Controller::CameraEventMouseScrolled(MouseScrolledEvent& event)
	{
		s_ZoomValue -= (float)event.GetYoffset()*0.1f;
		if (s_ZoomValue < 0.1f) s_ZoomValue = 0.1f;
		s_Camera->SetZoomLevel(s_ZoomValue);
		return true;
	}

	bool Controller::CameraEventMouseDoubleClicked(MouseButtonDoubleClickedEvent& event)
	{
		Controller::ResetCameraPosition();
		return true;
	}

	bool Controller::CameraEventMouseButtonPressed(MouseButtonPressedEvent& event)
	{
		if (event.GetMouseButton() == 0)
		{
			s_LastMousePos.x = s_CurrMousePos.x;
			s_LastMousePos.y = s_CurrMousePos.y;
			s_Draggable = true;
		}
		return true;
	}

	bool Controller::CameraEventMouseButtonReleased(MouseButtonReleasedEvent& event)
	{
		if (event.GetMouseButton() == 0)
		{
			s_Draggable = false;
		}
		return true;
	}

	static ImVec2 winSize = ImVec2(1.0f,1.0f);
	static ImVec2 lastWinPos = ImVec2(0.0f, 0.0f);

	bool Controller::CameraEventMouseMoved(MouseMovedEvent& event)
	{
		ImGuiIO& io = ImGui::GetIO();

		auto imguiWindow = ImGui::FindWindowByName("Model Viewport");

		if (imguiWindow != nullptr)
		{
			if (imguiWindow->Pos.x != lastWinPos.x or imguiWindow->Pos.y != lastWinPos.y)
			{
				s_Draggable = false;
			}
			lastWinPos = imguiWindow->Pos;
			winSize = imguiWindow->Size;
		}

		float maxWinSize = std::max(winSize.x, winSize.y);

		s_WindowSizeRatio.x = (s_Camera->m_right - s_Camera->m_left)*1 / maxWinSize;
		s_WindowSizeRatio.y = (s_Camera->m_top - s_Camera->m_bottom)*1 / maxWinSize;

		s_CurrMousePos.x = (float)event.GetX();
		s_CurrMousePos.y = (float)event.GetY();

		if (s_Draggable)
		{
			float deltaX = ((float)event.GetX() - s_LastMousePos.x) * s_WindowSizeRatio.x * s_ZoomValue;
			float deltaY = ((float)event.GetY() - s_LastMousePos.y) * s_WindowSizeRatio.y * s_ZoomValue;

			float angle = s_CameraRotation * PI / 180;

			s_CameraPosition.x += -deltaX*cos(angle)+deltaY*sin(angle);
			s_CameraPosition.y += deltaX*sin(angle)+deltaY*cos(angle);
		}
		s_LastMousePos.x = s_CurrMousePos.x;
		s_LastMousePos.y = s_CurrMousePos.y;

		return true;
	}


	/////////////////////////
	///// OBJECT INPUTS /////
	/////////////////////////

	const glm::mat4& Controller::GetViewMatrix()
	{
		if (s_Camera == nullptr) return s_NullMatrix;
		return s_Camera->GetViewMatrix();
	}

	const glm::mat4& Controller::GetProjectionMatrix()
	{
		if (s_Camera == nullptr) return s_NullMatrix;
		return s_Camera->GetProjectionMatrix();
	}

	const glm::mat4& Controller::GetViewProjectionMatrix()
	{
		if (s_Camera == nullptr) return s_NullMatrix;
		return s_Camera->GetViewProjectionMatrix();
	}

	void Controller::HandleObjectInputs(Substrate* object, Timestep* timestep)
	{
		s_SelectedObject = object;
		ObjectOnUpdate(timestep);
	}

	void Controller::HandleObjectInputs(Magnetron* object, Timestep* timestep)
	{
		s_SelectedObject = object;
		ObjectOnUpdate(timestep);
	}

	void Controller::ObjectOnUpdate(Timestep* timestep)
	{
		if (s_SelectedObject == nullptr) return;

		/*Substrate* obj = (Substrate*)(void*)s_SelectedObject;*/

		//auto objectPosition = glm::vec3(s_SelectedObject->GetPosX(), s_SelectedObject->GetPosY(), s_SelectedObject->GetPosZ()); ;

		//if (Input::IsKeyPressed(GLFW_KEY_LEFT))
		//{
		//	*obj->GetRPM() = 12.0f;
		//}

		//if (Input::IsKeyPressed(GLFW_KEY_RIGHT))
		//{
		//	s_CameraPosition.x -= s_CameraSpeed * (*timestep);
		//}
		//if (Input::IsKeyPressed(GLFW_KEY_UP))
		//{
		//	s_CameraPosition.y -= s_CameraSpeed * (*timestep);
		//}
		//if (Input::IsKeyPressed(GLFW_KEY_DOWN))
		//{
		//	s_CameraPosition.y += s_CameraSpeed * (*timestep);
		//}
		//if (Input::IsKeyPressed(GLFW_KEY_Q))
		//{
		//	s_CameraRotation -= s_CameraRotationSpeed * (*timestep);
		//}
		//if (Input::IsKeyPressed(GLFW_KEY_E))
		//{
		//	s_CameraRotation += s_CameraRotationSpeed * (*timestep);
		//}
		//if (Input::IsKeyPressed(GLFW_KEY_W))
		//{
		//	s_CameraRotationVertical -= s_CameraRotationSpeed * (*timestep);
		//}
		//if (Input::IsKeyPressed(GLFW_KEY_S))
		//{
		//	s_CameraRotationVertical += s_CameraRotationSpeed * (*timestep);
		//}

		//s_Camera->SetPositon(s_CameraPosition);
		//s_Camera->SetRotationAroundX(s_CameraRotationVertical);
		//s_Camera->SetRotationAroundZ(s_CameraRotation);
	}
}
