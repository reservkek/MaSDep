#pragma once

#include "Core.h"

namespace MSD {

	enum class ShaderDataType
	{
		None = 0, Float, Float2, Float3, Float4, Mat2, Mat3, Mat4, Int, Int2, Int3, Int4, Bool
	};

	static unsigned int DataTypeSize(ShaderDataType type)
	{
		switch (type)
		{
		case ShaderDataType::Float:     return 4;
		case ShaderDataType::Float2:    return 8;
		case ShaderDataType::Float3:    return 12;
		case ShaderDataType::Float4:    return 16;
		case ShaderDataType::Mat2:		return 16;
		case ShaderDataType::Mat3:		return 36;
		case ShaderDataType::Mat4:		return 64;
		case ShaderDataType::Int:		return 4;
		case ShaderDataType::Int2:      return 8;
		case ShaderDataType::Int3:      return 12;
		case ShaderDataType::Int4:      return 16;
		case ShaderDataType::Bool:      return 1;
		}
		return 0;
	}

	struct BufferElement
	{
		std::string name;
		uint16_t offset;
		uint16_t size;
		uint16_t count;
		ShaderDataType type;
		bool normalized;

		BufferElement(ShaderDataType Type, const std::string& Name, bool Normalized = false)
			: name(Name), type(Type), size(DataTypeSize(Type)), count((uint32_t)ceil(size / 4)), offset(0), normalized(Normalized)
		{
		}
	};

	class BufferLayout
	{
	public:
		BufferLayout()
		{

		}

		BufferLayout(const std::initializer_list<BufferElement>& elements)
			: m_Elements(elements)
		{
			CalculateOffsetAndStride();
		}

		inline uint32_t GetStride() const { return m_Stride; }
		inline const std::vector<BufferElement>& GetElements() const { return m_Elements; }

		std::vector<BufferElement>::iterator begin() { return m_Elements.begin(); }
		std::vector<BufferElement>::iterator end() { return m_Elements.end(); }
	private:
		void CalculateOffsetAndStride()
		{
			for (auto& element : m_Elements)
			{
				element.offset = m_Stride;
				m_Stride += element.size;
			}
		}
		std::vector<BufferElement> m_Elements;
		uint16_t m_Stride = 0;
	};



	class VertexBuffer
	{
	public:
		VertexBuffer(float* data, uint32_t size);
		VertexBuffer(std::vector<glm::vec3>& data);
		~VertexBuffer();

		void Bind() const;
		void Unbind() const;

		const BufferLayout& GetLayout() const { return m_Layout; };
		void SetLayout(const BufferLayout& layout) { m_Layout = layout; };
	private:
		BufferLayout m_Layout;
		unsigned int m_vbID;
	};



	class IndexBuffer
	{
	private:
		unsigned int m_RendererID;
		unsigned int m_Count;
	public:
		IndexBuffer(const void* data, unsigned int count);
		IndexBuffer(std::vector<glm::uvec4>& data);
		~IndexBuffer();

		void Bind() const;
		void Unbind() const;

		inline unsigned int GetCount() const { return m_Count; }
	};

}
