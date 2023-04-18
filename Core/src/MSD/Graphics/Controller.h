#pragma once

#include "Graphics/Cameras.h"
#include "Graphics/Internal/Timestep.h"
#include "GLFW/glfw3.h"
#include "Events/MouseEvent.h"
#include "UserInterface/MainLayer.h"

namespace MSD {

	class Controller
	{
	public:
		Controller() {}

		static void HandleCameraInputs(OrthographicCamera* camera, Timestep* timestep);
		static void CameraOnUpdate(Timestep* timestep);
		static void CameraOnEvent(Event& event);
		static void EnableEvents();
		static void DisableEvents();
		static void ResetCameraPosition();


		static void HandleObjectInputs(Substrate* substrate, Timestep* timestep);
		static void HandleObjectInputs(Magnetron* magnetron, Timestep* timestep);
		static void ObjectOnUpdate(Timestep* timestep);

		inline static void SetCameraSpeed(float speed) { s_CameraSpeed = speed; }
	private:
		static bool CameraEventMouseScrolled(MouseScrolledEvent& event);
		static bool CameraEventMouseDoubleClicked(MouseButtonDoubleClickedEvent& event);
		static bool CameraEventMouseButtonPressed(MouseButtonPressedEvent& event);
		static bool CameraEventMouseButtonReleased(MouseButtonReleasedEvent& event);
		static bool CameraEventMouseMoved(MouseMovedEvent& event);
	private:
		static OrthographicCamera* s_Camera;
		static AngMSDObject* s_SelectedObject;

		static float s_CameraSpeed;
		static float s_CameraRotation;
		static float s_CameraRotationVertical;
		static float s_CameraRotationSpeed;
		static float s_ZoomValue;
		static glm::vec3 s_CameraPosition;

		static glm::vec2 s_CurrMousePos;
		static glm::vec2 s_LastMousePos;
		static glm::vec2 s_WindowSizeRatio;

		static bool s_Draggable;
		static bool s_EnableEvents;
		static bool s_isDragging;
	};

}