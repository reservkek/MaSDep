#pragma once

#include "Buffer.h"
#include "Shader.h"
#include "Cameras.h"
#include "FrameBuffer.h"
#include "VertexArray.h"

#include "Internal/Objects.h"

namespace MSD {

	class Renderer
	{
	public:
		void Clear() const;
		void BeginScene(OrthographicCamera* camera, Shader* shader);
		void EndScene();

		Rect* CreateRect();

		void Submit(Shader* shader, const std::shared_ptr<VertexArray> va, glm::mat4 ModelMatrix = glm::mat4(1.0f));
		void Draw(const std::shared_ptr<VertexArray> va);
		void DrawLines(const std::shared_ptr<VertexArray> va, float thickness = 2.0f);
		void DrawGrid(Shader* shader);
		void DrawRect(Object* rect = nullptr, glm::vec3 position = glm::vec3(0, 0, 0));
		void DrawRectOutline(Object* rect = nullptr, glm::vec3 position = glm::vec3(0, 0, 0));
		void DrawCube(glm::vec3 position = glm::vec3(0, 0, 0));
		void DrawArrow(Arrow* arrow = nullptr, glm::vec3 position = glm::vec3(0, 0, 0));
		void DrawScene();

		void SetObjectID(int index);

		void CalculateGrid();
		void Flush();


		static int& GetSelectedItem() { return s_SelectedID; }
		static void GetOpenGLVersion();
		//void CreateElement(const MSMagnetron& object, VertexArray& va, const VertexBufferLayout& layout);
	private:
		std::shared_ptr<VertexArray> m_va;
		std::shared_ptr<VertexBuffer> m_vb;
		std::shared_ptr<IndexBuffer> m_ib;

		Shader* m_shader;
		OrthographicCamera* m_Camera = nullptr;

		std::vector<Object*> m_Objects;

		static int s_SelectedID;
	};

}
