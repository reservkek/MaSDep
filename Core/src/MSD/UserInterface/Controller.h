#pragma once

#include "Graphics/Cameras.h"
#include "Graphics/Internal/Timestep.h"
#include "GLFW/glfw3.h"
#include "Events/MouseEvent.h"
#include "UserInterface/MainLayer.h"

namespace MSD {

	// This class is used to unite the controls to move camera or object in the viewport
	// Controller has two states: View means that you move your camera and Transform is used
	// to move selected object in your AngMSD space

	enum class ControllerState
	{
		View = 0, Tranform = 1
	};

	class Controller
	{
	public:
		Controller() {}

		static void HandleCameraInputs(OrthographicCamera* camera, Timestep* timestep);
		static void EnableCameraEvents();
		static void DisableCameraEvents();
		static void ResetCameraPosition();
		static void CameraOnUpdate(Timestep* timestep);
		static void CameraOnEvent(Event& event);

		static void DragObjectStart();
		static void DragObjectStop();

		static void SetState(ControllerState state);

		static const float GetCameraZoomLevel() { return s_ZoomValue; };
		static const float GetCameraRotation() { return s_CameraRotation; };
		static const glm::vec3& GetCameraPosition() { return s_CameraPosition;  }
		static const glm::mat4& GetViewMatrix();
		static const glm::mat4& GetProjectionMatrix();
		static const glm::mat4& GetViewProjectionMatrix();

		static void ObjectOnUpdate(Timestep* timestep);

		static void ObjectStartTransform(AngMSDObject* object);

		inline static void SetCameraSpeed(float speed) { s_CameraSpeed = speed; }

	private:
	// GLOBAL EVENTS 
		static bool GlobalEventMouseMoved(MouseMovedEvent& event);
	// CAMERA EVENTS
		static bool CameraEventMouseScrolled(MouseScrolledEvent& event);
		static bool CameraEventMouseDoubleClicked(MouseButtonDoubleClickedEvent& event);
		static bool CameraEventMouseButtonPressed(MouseButtonPressedEvent& event);
		static bool CameraEventMouseButtonReleased(MouseButtonReleasedEvent& event);
		static bool CameraEventMouseMoved(MouseMovedEvent& event);

	// OBJECT EVENTS
		static bool ObjectEventMouseButtonPressed(MouseButtonPressedEvent& event);
		static bool ObjectEventMouseButtonReleased(MouseButtonReleasedEvent& event);
		static bool ObjectEventKeyPressed(KeyPressedEvent& event);
		static bool ObjectEventKeyReleased(KeyReleasedEvent& event);
		static bool ObjectEventMouseMoved(MouseMovedEvent& event);
	private:
		static OrthographicCamera* s_Camera;

		static const glm::mat4 s_NullMatrix;

		static float s_CameraSpeed;
		static float s_CameraRotation;
		static float s_CameraRotationVertical;
		static float s_CameraRotationSpeed;
		static float s_ZoomValue;
		static glm::vec3 s_CameraPosition;

		static glm::vec2 s_CurrMousePos;
		static glm::vec2 s_LastMousePos;
		static glm::vec2 s_Delta;

		static glm::vec2 s_ObjectPos;
		static glm::vec2 s_ObjectPosVirtual;
		static glm::vec2 s_WindowSizeRatio;

		static float s_ObjectRotation;
		static float s_ObjectRotationRounded;

		static bool s_CameraDraggable;
		static bool s_EnableCameraEvents;

		static ControllerState s_ControllerState;

		// Containers
		static std::unique_ptr<Object> s_ObjectBeforeTransform;
		static AngMSDObject* s_TransformingObject;

		static bool s_ObjectSticking;

	public:
		static bool s_DragObject;
		static bool s_RotateObject;
	};

}