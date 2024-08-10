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

	namespace Controller
	{
		extern OrthographicCamera* s_Camera;
		extern float s_CameraSpeed;
		extern float s_CameraRotation;
		extern float s_CameraRotationVertical;
		extern float s_CameraRotationSpeed;
		extern float s_ZoomValue;
		extern glm::vec3 s_CameraPosition;
		extern glm::vec2 s_CurrMousePos;
		extern glm::vec2 s_LastMousePos;
		extern glm::vec2 s_Delta;
		extern glm::vec2 s_ObjectPos;
		extern glm::vec2 s_ObjectPosVirtual;
		extern glm::vec2 s_WindowSizeRatio;
		extern float s_ObjectRotation;
		extern float s_ObjectRotationRounded;
		extern int s_CameraDraggable;
		extern bool s_EnableCameraEvents;
		extern ControllerState s_ControllerState;
		extern AngMSDObject* s_TransformingObject;
		extern bool s_ObjectSticking;
		extern bool s_DragObject;
		extern bool s_RotateObject;
		extern bool s_EnableEvents;
		extern const glm::mat4 s_NullMatrix;
		extern std::unique_ptr<Object> s_ObjectBeforeTransform;

		void HandleCameraInputs(OrthographicCamera* camera, Timestep* timestep);
		void EnableCameraEvents();
		void DisableCameraEvents();
		void ResetCameraPosition();
		void CameraOnUpdate(Timestep* timestep);
		void CameraOnEvent(Event& event);
		void DragObjectStart();
		void DragObjectStop();
		bool RotateObjectStop();
		void SetState(ControllerState state);

		inline bool						isDragging()			{ return s_DragObject; };
		inline const ControllerState	GetState()				{ return s_ControllerState; }
		inline const float				GetCameraZoomLevel()	{ return s_ZoomValue; };
		inline const float				GetCameraRotation()		{ return s_CameraRotation; };
		inline const glm::vec3&			GetCameraPosition()		{ return s_CameraPosition;  }
		const glm::mat4& GetViewMatrix();
		const glm::mat4& GetProjectionMatrix();
		const glm::mat4& GetViewProjectionMatrix();

		void ObjectOnUpdate(Timestep* timestep);
		void ObjectStartTransform(AngMSDObject* object);
		inline void SetCameraSpeed(float speed) { s_CameraSpeed = speed; }

	// GLOBAL EVENTS 
		bool GlobalEventMouseMoved(MouseMovedEvent& event);
	// CAMERA EVENTS
		bool CameraEventMouseScrolled(MouseScrolledEvent& event);
		bool CameraEventMouseDoubleClicked(MouseButtonDoubleClickedEvent& event);
		bool CameraEventMouseButtonPressed(MouseButtonPressedEvent& event);
		bool CameraEventMouseButtonReleased(MouseButtonReleasedEvent& event);
		bool CameraEventMouseMoved(MouseMovedEvent& event);
	// OBJECT EVENTS
		bool ObjectEventMouseButtonPressed(MouseButtonPressedEvent& event);
		bool ObjectEventMouseButtonReleased(MouseButtonReleasedEvent& event);
		bool ObjectEventKeyPressed(KeyPressedEvent& event);
		bool ObjectEventKeyReleased(KeyReleasedEvent& event);
		bool ObjectEventMouseMoved(MouseMovedEvent& event);

	};

}