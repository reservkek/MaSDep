#pragma once

#include "Core.h"
#include "../glm/glm.hpp"

namespace MSD {

	struct ShaderProgramSource
	{
		std::string Comment;
		std::string VertexShader;
		std::string FragmentShader;
	};

	class Shader
	{
	private:
		std::string m_FilePath;
		unsigned int m_ShaderID;
		std::unordered_map<std::string, int> m_UniformLocationCache;
	public:
		Shader(const std::string& filepath);
		~Shader();

		void Bind() const;
		void Unbind() const;

		// Set uniforms
		void SetUniform1f(const std::string& name, float v);
		void SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3);
		void SetUniform4fv(const std::string& name, const glm::vec4& vec);
		void SetUniformMat4(const std::string& name, const glm::mat4& matrix);
		void SetUniform1i(const std::string& name, int i);
	private:
		ShaderProgramSource ParseShader(const std::string& filepath);
		unsigned int CompileShader(unsigned int type, const std::string& source);
		unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader);
		int GetUniformLocation(const std::string& name);
	};

}