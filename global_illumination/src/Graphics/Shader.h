#pragma once
#include "Global/Types.h"
#include <string>
#include <unordered_map>

namespace GL
{
	/*
	* Represents a Shader program
	*/
	class ShaderProgram
	{
	public:
		ShaderProgram(const char* vertSrc, const char* fragSrc);
		ShaderProgram(const char* vertSrc, const char* geoSrc, const char* fragSrc);

		ShaderProgram(const ShaderProgram& other) = delete;
		ShaderProgram& operator=(const ShaderProgram& other) = delete;

		ShaderProgram(ShaderProgram&& other);
		ShaderProgram& operator=(ShaderProgram&& other);

		~ShaderProgram();
	public:
		bool IsValid();

		/// <summary>
		/// tries to recreate the shader, if it fails returns true without making them shader invalid.
		/// </summary>
		bool Reload(const char* vertexSrc, const char* geoSrc, const char* fragSrc);

		/// <summary>
		/// tries to recreate the shader, if it fails returns true without making them shader invalid.
		/// </summary>
		bool Reload(const char* vertexSrc, const char* fragSrc);

		void Bind() const;
		void UnBind() const;

		bool SetUniform(const std::string& name, float num) const;
		bool SetUniform(const std::string& name, glm::vec2 a) const;
		bool SetUniform(const std::string& name, glm::vec3 a) const;
		bool SetUniform(const std::string& name, glm::vec4 a) const;
		bool SetUniform(const std::string& name, int i) const;
		bool SetUniform(const std::string& name, glm::ivec3 a) const;
		bool SetUniform(const std::string& name, glm::uvec3 a) const;
		bool SetUniform(const std::string& name, unsigned int i) const;
		bool SetUniform(const std::string& name, const glm::mat4& mat) const;
	private:
		i32 GetLocation(const std::string& name) const;
	private:
		u32 m_program_id; // the shader program id OpenGl gives
		mutable std::unordered_map<std::string, i32> m_uniforms; // storing uniforms locations
	};
}