#include "msdpch.h"
#include "Objects.h"

namespace MSD 
{
	std::vector<std::shared_ptr<Object>> Object::m_GraphicsObjectBuffer = {};

	Object::Object()
	{
		AddObject(this);
		m_ObjectID = -1;
	}

	void Object::SetPosition(const glm::vec3& pos)
	{
		m_Pos = pos;
	}

	void Object::SetColor(const glm::vec4& color)
	{
		m_Color = color;
		m_OutlineColor = glm::vec4(1.0f-color.x, 1.0f-color.y, 1.0f-color.z, 0.7f);
	}


	void Object::SetScale(const glm::vec3& scale)
	{
		m_Scale = scale;
	}

	void Object::SetAngle(float angle)
	{
		m_Angle = angle;
	}

	void Object::CalcModelMatrix()
	{
		auto scale = glm::scale(glm::mat4(1.0f), m_Scale);
		auto rotation = glm::rotate(glm::mat4(1.0f), m_Angle, glm::vec3(0, 0, 1));
		auto translation = glm::translate(glm::mat4(1.0f), m_Pos);
		m_ModelMatrix = translation * rotation * scale;
	}

	void Object::AddObject(Object* obj)
	{
		m_GraphicsObjectBuffer.push_back(std::unique_ptr<Object>(obj));
	}

	float Rect::coords[12] =
	{
		-100.0f, -200.0f, 0.0f, // 0 
		 100.0f, -200.0f, 0.0f, // 1 
		-100.0f,  0.0f, 0.0f, // 2 
		 100.0f,  0.0f, 0.0f  // 3
	};

	unsigned int Rect::indices[6] = { 0, 1, 2, 1, 2, 3 };

	unsigned int Rect::outlineIndices[8] = { 0, 1, 1, 3, 3, 2, 2, 0 };

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
	{
	  0, 1, 2, 1, 2, 3,
	  4, 5, 6, 5, 6, 7,
	  0, 2, 4, 2, 4, 6,
	  1, 3, 5, 3, 5, 6,
	  0, 1, 4, 1, 4, 5,
	  2, 3, 7, 3, 7, 6
	};

	float Arrow::coords[3 * 5] =
	{
		 0.0f,  0.0f, 0.0f, // Arrow line start
		 0.0f,  75.0f, 0.0f, // Arrow line end
		 0.0f,  100.0f, 0.0f, // Arrow head vertex 1
		-10.0f, 75.0f, 0.0f, // Arrow head vertex 2
		 10.0f, 75.0f, 0.0f  // Arrow head vertex 3
	};

	unsigned int Arrow::indicesLine[2] = { 0, 1 };

	unsigned int Arrow::indicesHead[3] = { 2, 3, 4 };

	unsigned int Arrow::outlineIndices[4] = { 0, 2, 3, 4 };

	std::vector<glm::vec3> Grid::u_Vertices = std::vector<glm::vec3>();
	std::vector<glm::uvec4> Grid::u_Indices = std::vector<glm::uvec4>();
	glm::mat4 Grid::u_ModelMatrix = glm::mat4(1.0f);
	glm::vec4 Grid::u_Color = glm::vec4(1.0f, 0.0f, 1.0f, 1.0f);
	

	BufferLayout Grid::u_BasicLayout = {
		{ ShaderDataType::Float3, "a_Position"}
	};

	void Arrow::SetPosition(const glm::vec3& pos)
	{
		m_Pos = pos;
	}
}