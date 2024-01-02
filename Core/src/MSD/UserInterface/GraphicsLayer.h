#pragma once

#include "Core.h"

#include "../LayerSystem/Layer.h"
#include "../Events/MouseEvent.h"
#include "../Events/KeyEvent.h"
#include "../Events/ApplicationEvent.h"

#include "Input/Input.h"

#include "Graphics/Renderer.h"
#include "../glm/gtc/matrix_transform.hpp"

#include "MathModels/AngularMSD/AngularMSD.h"

namespace MSD {

	class GraphicsLayer : public Layer
	{
	public:
		GraphicsLayer();
		~GraphicsLayer();

		std::shared_ptr<FrameBuffer> GetFrameBuffer() { return fb; }

		void OnEvent(Event& event) override;

		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnUpdate(Timestep ts) override;

		void SetModel(AngMSD* model) { m_Model = model; };

		bool& isUpdating() { return m_Updating; }
		void SetSelectedItem(int id);

		void UpdateObjects();
		void UpdateObjectStates();

		static bool m_HandleInputs;
	private:
		bool m_Updating = false;

		std::shared_ptr<VertexArray> va;
		std::shared_ptr<VertexBuffer> vb;
		std::shared_ptr<IndexBuffer> ib;

		AngMSD* m_Model;

		OrthographicCamera* camera;

		Renderer renderer;

		Shader* shader;
		Shader* gridShader;

		std::shared_ptr<FrameBuffer> fb;

		BufferLayout layout =
		{
			{ ShaderDataType::Float3, "a_Position"}
		};

		FrameBufferSpecification fbSpec;

		glm::vec4 color = glm::vec4(0.8f, 0.5f, 0.2f, 1.00f);
		glm::vec3 m_CameraPosition = glm::vec3(0.0f,0.0f,0.0f);

		float m_CameraSpeed = 1000.0f;

		Cube* testCube = nullptr;
	};

}