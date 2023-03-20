#pragma once

#include "../../vendor/glm/glm.hpp"
#include "../../vendor/glm/gtc/matrix_transform.hpp"

#include "Graphics/Buffer.h"

namespace MSD {

	class Object
	{
	public:
		void SetPosition(const glm::vec3& pos);
		void SetColor(const glm::vec4& color);
		void SetScale(const glm::vec3& scale);
		void SetAngle(float angle);
		void SetID(unsigned int id) { m_ObjectID = id; }

		void CalcModelMatrix();

		unsigned int GetID() { return m_ObjectID; }


		const glm::vec4& GetColor() const { return m_Color; };
		const glm::vec4& GetOutlineColor() const { return m_OutlineColor; };
		const glm::mat4& GetModelMatrix() const { return m_ModelMatrix; };
		const BufferLayout& GetLayout() const { return m_BasicLayout; };
	private:
		glm::vec3 m_Scale = glm::vec3(1.0f,1.0f,1.0f);
		glm::vec3 m_Pos = glm::vec3(0.0f);
		glm::vec4 m_Color = glm::vec4(0.1f, 0.5f, 0.0f, 1.0f);
		glm::vec4 m_OutlineColor = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
		glm::mat4 m_ModelMatrix = glm::mat4(1.0f);

		float m_Angle = 0.0f;

		unsigned int m_ObjectID;

		BufferLayout m_BasicLayout =
		{
			{ ShaderDataType::Float3, "a_Position"}
		};
	};

	class Rect : public Object {
	public:
		Rect(unsigned int id = 0);

		unsigned int m_ObjectID;
		static float coords[12];
		static unsigned int indices[6];
		static unsigned int outlineIndices[8];
	};

	class Cube : public Object {
	public:
		static float coords[3*4*2];
		static unsigned int indices[6*6];
	};

	class Grid
	{
	public:
		static glm::mat4 u_ModelMatrix;
		static glm::vec4 u_Color;
		static std::vector<glm::vec3> u_Vertices;
		static std::vector<glm::uvec4> u_Indices;

		static BufferLayout u_BasicLayout;
	};

}

