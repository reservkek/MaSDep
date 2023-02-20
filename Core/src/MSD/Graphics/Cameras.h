#pragma once

#include "../glm/glm.hpp"

namespace MSD {

	class OrthographicCamera
	{
	public:
		const glm::vec3& GetPosition() const { return m_Position; }
		float GetRotation() const { return m_RotationAroundZ; }

		OrthographicCamera(float left, float right, float bottom, float top);
		void SetPositon(const glm::vec3& position);
		void SetRotationAroundZ(float rotation) { m_RotationAroundZ = rotation; CalcViewMatrix(); }
		void SetZoomLevel(float zoom) { m_Zoom = zoom; CalcProjectionMatrix(); }

		const glm::mat4& GetProjectionMatrix() const { return m_ProjectionMatrix; }
		const glm::mat4& GetViewMatrix() const { return m_ViewMatrix; }
		const glm::mat4& GetViewProjectionMatrix() const { return m_ViewProjectionMatrix; }

		float m_left, m_right, m_bottom, m_top = 0.0f;
	private:
		void CalcViewMatrix();
		void CalcProjectionMatrix();

		glm::mat4 m_ProjectionMatrix;
		glm::mat4 m_ViewMatrix = glm::mat4(1.0f);
		glm::mat4 m_ViewProjectionMatrix;

		glm::vec3 m_Position = glm::vec3(0, 0, 0);
		float m_RotationAroundZ = 0.0f;
		float m_Zoom = 1.0f;
	};

}