#pragma once

#include "../../vendor/glm/glm.hpp"
#include "../../vendor/glm/gtc/matrix_transform.hpp"

#include "Graphics/Buffer.h"

namespace MSD {

	enum class ObjectType
	{
		None, Rect, Arrow, Circle, Cube
	};

	class Object
	{
	public:
		Object(int id = 0);

		virtual void SetPosition(const glm::vec3& pos);
		void SetColor(const glm::vec4& color);
		void SetScale(const glm::vec3& scale);
		void SetAngle(float angle);
		void SetID(unsigned int id) { m_ObjectID = id; }

		void CalcModelMatrix();

		unsigned int GetID() { return m_ObjectID; }

		virtual const ObjectType GetType() const { return ObjectType::None; };

		const glm::vec4& GetColor() const { return m_Color; };
		const glm::vec4& GetOutlineColor() const { return m_OutlineColor; };
		const glm::mat4& GetModelMatrix() const { return m_ModelMatrix; };
		const glm::vec3& GetPosition() const { return m_Pos; };
		const BufferLayout& GetLayout() const { return m_BasicLayout; };

		static void AddObject(Object* obj);

		static const std::vector<std::shared_ptr<Object>>& GetObjects() { return m_GraphicsObjectBuffer; }
	protected:
		glm::vec3 m_Scale = glm::vec3(1.0f,1.0f,1.0f);
		glm::vec3 m_Pos = glm::vec3(0.0f);
		glm::vec4 m_Color = glm::vec4(0.2f, 0.2f, 1.0f, 1.0f);
		glm::vec4 m_OutlineColor = glm::vec4(0.8f, 0.8f, 0.0f, 0.7f);
		glm::mat4 m_ModelMatrix = glm::mat4(1.0f);

		float m_Angle = 0.0f;

		unsigned int m_ObjectID;

		BufferLayout m_BasicLayout =
		{
			{ ShaderDataType::Float3, "a_Position"}
		};

	private:
		static std::vector<std::shared_ptr<Object>> m_GraphicsObjectBuffer;
	};

	class Rect : public Object {
	public:

		unsigned int m_ObjectID;
		static float coords[12];
		static unsigned int indices[6];
		static unsigned int outlineIndices[8];

		virtual const ObjectType GetType() const { return ObjectType::Rect;};
	};

	class Cube : public Object {
	public:
		Cube(int id = 0);

		unsigned int m_ObjectID;

		static float coords[24];
		static unsigned int indices[36];
		static unsigned int outlineIndices[24];
	};

	class Arrow : public Object
	{
	public:
		virtual void SetPosition(const glm::vec3& pos) override;

		static float coords[3*5];
		static unsigned int indicesLine[2];
		static unsigned int indicesHead[3];

		static unsigned int outlineIndices[4];

		virtual const ObjectType GetType() const { return ObjectType::Arrow; };
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

