#include "msdpch.h"

#include "Controller.h"

#include "imgui.h"

namespace MSD {

	namespace Controller {

		float s_CameraSpeed = 1000.0f;
		float s_CameraRotation = 180.0f;
		float s_CameraRotationVertical = 0.0f;
		float s_CameraRotationSpeed = 180.0f;
		float s_ZoomValue = 1.0f;
		float s_ObjectRotation = 0.0f;
		float s_ObjectRotationRounded = 0.0f;
		int   s_CameraDraggable = 0;
		bool  s_EnableCameraEvents = true;
		bool  s_DragObject = false;
		bool  s_RotateObject = false;
		bool  s_EnableEvents = false;
		bool  s_ObjectSticking = false;

		glm::vec3 s_CameraPosition = glm::vec3(0.0f, 0.0f, 0.0f);
		glm::vec2 s_CurrMousePos = glm::vec2(0.0f, 0.0f);
		glm::vec2 s_LastMousePos = glm::vec2(0.0f, 0.0f);
		glm::vec2 s_Delta = glm::vec2(0.0f, 0.0f);
		glm::vec2 s_ObjectPos = glm::vec2(0.0f, 0.0f);
		glm::vec2 s_ObjectPosVirtual = glm::vec2(0.0f, 0.0f);
		glm::vec2 s_WindowSizeRatio = glm::vec2(0.0f, 0.0f);

		const glm::mat4			s_NullMatrix = glm::mat4(1.0f);
		std::unique_ptr<Object> s_ObjectBeforeTransform = nullptr;
		ControllerState			s_ControllerState = ControllerState::View;
		OrthographicCamera*		s_Camera = nullptr;
		AngMSDObject*			s_TransformingObject = nullptr;

	}

	static ImVec2 winSize = ImVec2(1.0f, 1.0f);
	static ImVec2 lastWinPos = ImVec2(0.0f, 0.0f);
	static vec3 term = vec3(0, 0, 0);

	//////////////////////////////
	/////// GLOBAL EVENTS ////////
	//////////////////////////////

	bool Controller::GlobalEventMouseMoved(MouseMovedEvent& event)
	{
		ImGuiIO& io = ImGui::GetIO();

		auto imguiWindow = ImGui::FindWindowByName(MSDSTR_MODELVIEWPORT);

		if (imguiWindow != nullptr)
		{
			if (imguiWindow->Pos.x != lastWinPos.x or imguiWindow->Pos.y != lastWinPos.y)
			{
				s_CameraDraggable = false;
			}
			lastWinPos = imguiWindow->Pos;
			winSize = imguiWindow->Size;
		}

		float maxWinSize = std::max(winSize.x, winSize.y);

		s_WindowSizeRatio.x = (s_Camera->m_right - s_Camera->m_left) * 1 / maxWinSize;
		s_WindowSizeRatio.y = (s_Camera->m_top - s_Camera->m_bottom) * 1 / maxWinSize;

		s_CurrMousePos.x = (float)event.GetX();
		s_CurrMousePos.y = (float)event.GetY();

		s_Delta.x = ((float)event.GetX() - s_LastMousePos.x) * s_WindowSizeRatio.x * s_ZoomValue;
		s_Delta.y = ((float)event.GetY() - s_LastMousePos.y) * s_WindowSizeRatio.y * s_ZoomValue;

		s_LastMousePos.x = s_CurrMousePos.x;
		s_LastMousePos.y = s_CurrMousePos.y;

		return true;
	}

	//////////////////////////////
	// CAMERA INPUTS AND EVENTS // 
	//////////////////////////////

	void Controller::HandleCameraInputs(OrthographicCamera* camera, Timestep* timestep)
	{
		s_Camera = camera;
		CameraOnUpdate(timestep);
	}

	void Controller::EnableCameraEvents()
	{
		s_EnableCameraEvents = true;
	}

	void Controller::DisableCameraEvents()
	{
		s_EnableCameraEvents = false;
	}

	void Controller::CameraOnUpdate(Timestep* timestep)
	{
		if (s_Camera == nullptr) return;

		if (s_EnableEvents)
		{
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
		}

		s_Camera->SetPositon(s_CameraPosition);
		s_Camera->SetRotationAroundX(s_CameraRotationVertical);
		s_Camera->SetRotationAroundZ(s_CameraRotation);
	}

	void Controller::CameraOnEvent(Event& event)
	{
		if (s_Camera == nullptr) return;

		EventDispatcher dispatcher(event);
		
		dispatcher.Dispatch<MouseMovedEvent>(BIND_EVENT_FN_STATIC(Controller::GlobalEventMouseMoved));

		dispatcher.Dispatch<MouseButtonPressedEvent>(BIND_EVENT_FN_STATIC(Controller::CameraEventMouseButtonPressed));
		dispatcher.Dispatch<MouseButtonReleasedEvent>(BIND_EVENT_FN_STATIC(Controller::CameraEventMouseButtonReleased));
		dispatcher.Dispatch<MouseMovedEvent>(BIND_EVENT_FN_STATIC(Controller::CameraEventMouseMoved));

		if (s_ControllerState == ControllerState::View)
		{
			if (!s_EnableCameraEvents or !s_EnableEvents) return;

			dispatcher.Dispatch<MouseButtonDoubleClickedEvent>(BIND_EVENT_FN_STATIC(Controller::CameraEventMouseDoubleClicked));
		}

		if (s_ControllerState == ControllerState::Tranform)
		{
			dispatcher.Dispatch<MouseButtonPressedEvent>(BIND_EVENT_FN_STATIC(Controller::ObjectEventMouseButtonPressed));
			dispatcher.Dispatch<MouseButtonReleasedEvent>(BIND_EVENT_FN_STATIC(Controller::ObjectEventMouseButtonReleased));
			dispatcher.Dispatch<KeyPressedEvent>(BIND_EVENT_FN_STATIC(Controller::ObjectEventKeyPressed));
			dispatcher.Dispatch<KeyReleasedEvent>(BIND_EVENT_FN_STATIC(Controller::ObjectEventKeyReleased));
			dispatcher.Dispatch<MouseMovedEvent>(BIND_EVENT_FN_STATIC(Controller::ObjectEventMouseMoved));
		}

		if (!s_EnableCameraEvents or !s_EnableEvents) return;
		dispatcher.Dispatch<MouseScrolledEvent>(BIND_EVENT_FN_STATIC(Controller::CameraEventMouseScrolled));

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
		if (event.GetMouseButton() == 0) Controller::ResetCameraPosition();
		return true;
	}

	static bool prevObjectDragState = false;

	bool Controller::CameraEventMouseButtonPressed(MouseButtonPressedEvent& event)
	{
		if (event.GetMouseButton() == 0)
		{
			s_CameraDraggable = true;
		}

		if (event.GetMouseButton() == 2)
		{
			s_CameraDraggable = 2;
			prevObjectDragState = s_DragObject;
			s_DragObject = false;
		}
		return true;
	}

	bool Controller::CameraEventMouseButtonReleased(MouseButtonReleasedEvent& event)
	{
		if (event.GetMouseButton() == 0)
		{
			s_CameraDraggable = false;
		}
		if (event.GetMouseButton() == 2)
		{
			s_CameraDraggable = false;
			s_DragObject = prevObjectDragState;
		}
		return true;
	}

	bool Controller::CameraEventMouseMoved(MouseMovedEvent& event)
	{
		if ((s_CameraDraggable && !s_DragObject) or s_CameraDraggable > 1)
		{
			float angle = s_CameraRotation * PI / 180;

			s_CameraPosition.x += -s_Delta.x * cos(angle) + s_Delta.y * sin(angle);
			s_CameraPosition.y += s_Delta.x * sin(angle) + s_Delta.y * cos(angle);
		}

		return true;
	}


	/////////////////////////
	///// OBJECT INPUTS /////
	/////////////////////////

	void Controller::DragObjectStart()
	{
		s_DragObject = true;
		if (s_TransformingObject != nullptr) {
			s_ObjectPos.x = s_TransformingObject->GetPos().x;
			s_ObjectPos.y = s_TransformingObject->GetPos().y;
		}
	}

	void Controller::DragObjectStop()
	{
		s_DragObject = false;
	}

	bool Controller::RotateObjectStop()
	{
		if (s_RotateObject)
		{
			s_RotateObject = false;
			return true;
		}
		return false;
	}

	void Controller::SetState(ControllerState state)
	{
		s_DragObject = false;
		s_RotateObject = false;
		s_ControllerState = state;
	}

	bool Controller::ObjectEventMouseButtonPressed(MouseButtonPressedEvent& event)
	{
		return true;
	}

	bool Controller::ObjectEventMouseButtonReleased(MouseButtonReleasedEvent& event)
	{
		return true;
	}

	bool Controller::ObjectEventKeyPressed(KeyPressedEvent& event)
	{
		if (event.GetKeyCode() == GLFW_KEY_LEFT_SHIFT)
			s_ObjectSticking = true;
		if (event.GetKeyCode() == GLFW_KEY_DELETE && s_TransformingObject != nullptr)
		{
			AngMSD& model = ApplicationCore::Get().GetModel();
			MainLayer* mainlayer = ApplicationCore::Get().GetMainLayer();
			
			mainlayer->DeleteObject(s_TransformingObject->GetID());
		}
		if (event.GetKeyCode() == GLFW_KEY_R && s_TransformingObject != nullptr)
		{
			term = s_TransformingObject->GetNormal();
			s_RotateObject = !s_RotateObject;
		}
		return true;
	}

	bool Controller::ObjectEventKeyReleased(KeyReleasedEvent& event)
	{
		if (event.GetKeyCode() == GLFW_KEY_LEFT_SHIFT)
			s_ObjectSticking = false;
		return false;
	}

	bool Controller::ObjectEventMouseMoved(MouseMovedEvent& event)
	{
		if (s_DragObject)
		{
			float angle = s_CameraRotation * PI / 180;

			float dX = 0.1f * (- s_Delta.x * cos(angle) + s_Delta.y * sin(angle));
			float dY = 0.1f * (s_Delta.x * sin(angle) + s_Delta.y * cos(angle));

			if (s_TransformingObject != nullptr)
			{
				s_ObjectPos.x = s_ObjectPos.x - dX;
				s_ObjectPos.y = s_ObjectPos.y - dY;

				if (s_ObjectSticking)
				{
					s_TransformingObject->GetPos().x = std::round(s_ObjectPos.x);
					s_TransformingObject->GetPos().y = std::round(s_ObjectPos.y);
				}
				else
				{
					s_TransformingObject->GetPos().x = s_ObjectPos.x;
					s_TransformingObject->GetPos().y = s_ObjectPos.y;
				}
			}

		}

		else if (s_RotateObject)
		{
			if (s_TransformingObject != nullptr)
			{
				s_ObjectRotation = s_Delta.y * 0.05f;

				// TO DO: REWORK STICKING TO ABSOLUTE ANGLES
				if (s_ObjectSticking)
				{
					s_ObjectRotationRounded = std::round(180 * Angle(term, vec3(0, 1, 0)) / PI);
					term = RotateAroundZ(term, s_ObjectRotation);
					bool doStick = ((int)(180 * Angle(term, vec3(0, 1, 0)) / PI) % 15) == 0;
					if (doStick)
					{
						s_TransformingObject->Rotate(s_ObjectRotation);
						term = s_TransformingObject->GetNormal();
					}
				}
				else
				{
					s_TransformingObject->Rotate(s_ObjectRotation);
				}
			}
		}

		return true;
	}

	void Controller::ObjectStartTransform(AngMSDObject* object)
	{
		s_ControllerState = ControllerState::Tranform;
		s_TransformingObject = object;
	}


	void Controller::ObjectOnUpdate(Timestep* timestep)
	{
	}

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

}
