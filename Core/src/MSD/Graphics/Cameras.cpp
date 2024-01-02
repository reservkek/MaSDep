#include "msdpch.h"

#include "Cameras.h"

#include "glm/gtc/matrix_transform.hpp"

namespace MSD {

	OrthographicCamera::OrthographicCamera(float left, float right, float bottom, float top, float close, float far)
		: m_left(left), m_right(right), m_bottom(bottom), m_top(top), m_close(close), m_far(far)
	{
		m_ProjectionMatrix = glm::ortho(left*m_Zoom, right*m_Zoom, bottom*m_Zoom, top*m_Zoom, m_close*m_Zoom, m_far*m_Zoom);
		m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
	}

	OrthographicCamera::OrthographicCamera(float dimension = 400.0f)
		: m_left(-dimension), m_right(dimension), m_bottom(-dimension), m_top(dimension), m_close(-dimension), m_far(dimension)
	{
		m_ProjectionMatrix = glm::ortho(m_left * m_Zoom, m_right * m_Zoom, m_bottom * m_Zoom, m_top * m_Zoom, m_close * m_Zoom, m_far * m_Zoom);
		m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
	}

	void OrthographicCamera::SetPositon(const glm::vec3& position)
	{
		m_Position = position;
		CalcViewMatrix();
	}

	void OrthographicCamera::SetRotationAroundZ(float rotation)
	{
		m_RotationAroundZ = rotation; 
		CalcViewMatrix();
	}

	void OrthographicCamera::CalcViewMatrix()
	{
		glm::mat4 translate = glm::translate(glm::mat4(1.0f), m_Position);
		glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), glm::radians(m_RotationAroundZ), glm::vec3(0, 0, -1));
		glm::mat4 rotationX = glm::rotate(glm::mat4(1.0f), glm::radians(m_RotationAroundX), glm::vec3(-1, 0, 0));

		m_ViewMatrix = glm::inverse(translate*rotation*rotationX);
		m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
	}

	void OrthographicCamera::CalcProjectionMatrix()
	{
		m_ProjectionMatrix = glm::ortho(m_left * m_Zoom, m_right * m_Zoom, m_bottom * m_Zoom, m_top * m_Zoom, 10.0f * m_close * m_Zoom, 10.0f * m_far * m_Zoom);
		m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
	}
}
