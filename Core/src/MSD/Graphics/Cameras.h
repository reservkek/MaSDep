#pragma once

#include "../glm/glm.hpp"

namespace MSD {

	class OrthographicCamera
	{
	public:
		const glm::vec3& GetPosition() const { return m_Position; }
		float GetRotation() const { return m_RotationAroundZ; }

		OrthographicCamera(float left, float right, float bottom, float top);
		void SetPositon(const glm::vec3& position) { m_Position = position; CalcViewMatrix(); }
		void SetRotationAroundZ(float rotation) { m_RotationAroundZ = rotation; CalcViewMatrix(); }

		const glm::mat4& GetProjectionMatrix() const { return m_ProjectionMatrix; }
		const glm::mat4& GetViewMatrix() const { return m_ViewMatrix; }
		const glm::mat4& GetViewProjectionMatrix() const { return m_ViewProjectionMatrix; }
	private:
		void CalcViewMatrix();

		glm::mat4 m_ProjectionMatrix;
		glm::mat4 m_ViewMatrix = glm::mat4(1.0f);
		glm::mat4 m_ViewProjectionMatrix;

		glm::vec3 m_Position = glm::vec3(0, 0, 0);
		float m_RotationAroundZ = 0.0f;
	};

}