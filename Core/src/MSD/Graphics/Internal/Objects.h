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
		void SetScale(float x, float y, float z = 1.0f);
		void SetScale(float scale);

		const glm::vec4& GetColor() const { return m_Color; };
		const glm::mat4& GetModelMatrix() const { return m_ModelMatrix; };
		const BufferLayout& GetLayout() const { return m_BasicLayout; };
	private:
		glm::mat4 m_ModelMatrix = glm::mat4(1.0f);
		glm::vec4 m_Color = glm::vec4(1.0f, 0.0f, 1.0f, 1.0f);
		unsigned int m_ObjectID;

		BufferLayout m_BasicLayout =
		{
			{ ShaderDataType::Float3, "a_Position"}
		};
	};

	class Rect : public Object {
	public:
		static float coords[12];
		static unsigned int indices[6];
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

