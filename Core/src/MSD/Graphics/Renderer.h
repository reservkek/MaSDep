#pragma once

#include "Buffer.h"
#include "Shader.h"
#include "Objects.h"
#include "Cameras.h"
#include "FrameBuffer.h"
#include "VertexArray.h"

#define ASSERT(x) if (!(x)) __debugbreak();

namespace MSD {

	class Renderer
	{
	public:
		void Clear() const;
		void BeginScene();
		void EndScene();
		void Submit(const std::shared_ptr<VertexArray> va);
		void Draw(const std::shared_ptr<VertexArray> va);
		void Flush();

		//void CreateElement(const MSMagnetron& object, VertexArray& va, const VertexBufferLayout& layout);

	};

}
