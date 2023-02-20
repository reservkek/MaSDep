#pragma once

#include "Graphics/Cameras.h"
#include "Input.h"
#include "Graphics/Internal/Timestep.h"
#include "GLFW/glfw3.h"
#include "Events/MouseEvent.h"
#include "LayerSystem/MainLayer.h"

namespace MSD {

	class Controller
	{
	public:
		Controller() {}

		static void HandleCameraInputs(OrthographicCamera* camera, Timestep* timestep);
		static void CameraOnUpdate(Timestep* timestep);
		static void CameraOnEvent(Event& event);
		static void ResetCameraPosition();

		inline static void SetCameraSpeed(float speed) { s_CameraSpeed = speed; }
	private:
		static bool CameraEventMouseScrolled(MouseScrolledEvent& event);
		static bool CameraEventMouseDoubleClicked(MouseButtonDoubleClickedEvent& event);
		static bool CameraEventMouseButtonPressed(MouseButtonPressedEvent& event);
		static bool CameraEventMouseButtonReleased(MouseButtonReleasedEvent& event);
		static bool CameraEventMouseMoved(MouseMovedEvent& event);
	private:
		static Controller* s_Instance;
		static OrthographicCamera* s_Camera;
		static float s_CameraSpeed;
		static float s_CameraRotation;
		static float s_CameraRotationSpeed;
		static float s_ZoomValue;
		static glm::vec3 s_CameraPosition;

		static glm::vec2 s_CurrMousePos;
		static glm::vec2 s_LastMousePos;
		static glm::vec2 s_WindowSizeRatio;

		static bool s_Draggable;
	};

}