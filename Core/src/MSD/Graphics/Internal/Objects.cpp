#include "msdpch.h"
#include "Objects.h"

namespace MSD 
{
	void Object::SetPosition(const glm::vec3& pos)
	{
		m_ModelMatrix = glm::translate(glm::mat4(1.0f), pos);
	}

	void Object::SetColor(const glm::vec4& color)
	{
		m_Color = color; 
	}

	void Object::SetScale(float x, float y, float z)
	{

	}

	void Object::SetScale(float scale)
	{

	}

	float Rect::coords[12] =
	{
		-200.0f, -200.0f, 0.0f, // 0 
		 200.0f, -200.0f, 0.0f, // 1 
		-200.0f,  200.0f, 0.0f, // 2 
		 200.0f,  200.0f, 0.0f  // 3
	};

	unsigned int Rect::indices[6] = { 0, 1, 2, 1, 2, 3 };

	float Cube::coords[24] =
	{
		-200.0f, -200.0f, -200.0f, //0
		 200.0f, -200.0f, -200.0f, //1
		-200.0f,  200.0f, -200.0f, //2
		 200.0f,  200.0f, -200.0f, //3
		-200.0f, -200.0f,  200.0f, //4
		 200.0f, -200.0f,  200.0f, //5
		-200.0f,  200.0f,  200.0f, //6
		 200.0f,  200.0f,  200.0f  //7
	};

	unsigned int Cube::indices[36] =
	{ 0, 1, 2, 1, 2, 3,
	  4, 5, 6, 5, 6, 7,
	  0, 2, 4, 2, 4, 6,
	  1, 3, 5, 3, 5, 6,
	  0, 1, 4, 1, 4, 5,
	  2, 3, 7, 3, 7, 6
	};

	std::vector<glm::vec3> Grid::u_Vertices = std::vector<glm::vec3>();
	std::vector<glm::uvec4> Grid::u_Indices = std::vector<glm::uvec4>();
	glm::mat4 Grid::u_ModelMatrix = glm::mat4(1.0f);
	glm::vec4 Grid::u_Color = glm::vec4(1.0f, 0.0f, 1.0f, 1.0f);
	

	BufferLayout Grid::u_BasicLayout = {
		{ ShaderDataType::Float3, "a_Position"}
	};
}