#include "OpenGL.h"
#include "Shader.h"
#include "Global/Logger.h"

u32 CompileShader(const char* source, u32 type);
u32 CreateShaderProgramFromSource(const char* vertSrc, const char* geoSrc, const char* fragSrc);

namespace GL
{
	ShaderProgram::ShaderProgram(const char* vertSrc, const char* fragSrc)
		:
		ShaderProgram(vertSrc, nullptr, fragSrc)
	{
	}

	ShaderProgram::ShaderProgram(const char* vertSrc, const char* geoSrc, const char* fragSrc)
		:
		m_program_id(0)
	{
		Reload(vertSrc, geoSrc, fragSrc);
	}

	ShaderProgram::ShaderProgram(ShaderProgram&& other)
		:
		m_program_id(other.m_program_id),
		m_uniforms(std::move(other.m_uniforms))
	{
		other.m_program_id = 0;
	}

	ShaderProgram& ShaderProgram::operator=(ShaderProgram&& other)
	{
		glCall(glDeleteProgram(m_program_id));

		m_program_id = other.m_program_id;
		m_uniforms = std::move(other.m_uniforms);

		other.m_program_id = 0;

		return *this;
	}

	ShaderProgram::~ShaderProgram()
	{
		glCall(glDeleteProgram(m_program_id));
	}

	bool ShaderProgram::IsValid()
	{	
		return m_program_id != 0;
	}

	bool ShaderProgram::Reload(const char* vertexSrc, const char* geoSrc, const char* fragSrc)
	{
		u32 new_program = CreateShaderProgramFromSource(vertexSrc, geoSrc, fragSrc);

		if (new_program == 0) // if new program failed to Compiled
		{
			return false; // we do nothing and return false
		}

		// delete old 
		glCall(glDeleteProgram(m_program_id));
		m_uniforms.clear();

		// replace
		m_program_id = new_program;

		return true;
	}

	bool ShaderProgram::Reload(const char* vertexSrc, const char* fragSrc)
	{
		return Reload(vertexSrc, nullptr, fragSrc);
	}

	void ShaderProgram::Bind() const
	{
		glCall(glUseProgram(m_program_id));
	}

	void ShaderProgram::UnBind() const
	{
		glCall(glUseProgram(0));
	}

	bool ShaderProgram::SetUniform(const std::string& name, float num) const
	{
		Bind();
		i32 location = GetLocation(name);

		if (location != -1)
		{
			glCall(glUniform1f(location, num));
			UnBind();
			return true;
		}

		UnBind();
		return false;
	}

	bool ShaderProgram::SetUniform(const std::string& name, glm::vec2 a) const
	{
		Bind();
		i32 location = GetLocation(name);

		if (location != -1)
		{
			glCall(glUniform2f(location, a.x, a.y));
			UnBind();
			return true;
		}

		UnBind();
		return false;
	}

	bool ShaderProgram::SetUniform(const std::string& name, glm::vec3 a) const
	{
		Bind();
		i32 location = GetLocation(name);

		if (location != -1)
		{
			glCall(glUniform3f(location, a.x, a.y, a.z));
			UnBind();
			return true;
		}

		UnBind();
		return false;
	}

	bool ShaderProgram::SetUniform(const std::string& name, glm::vec4 a) const
	{
		Bind();
		i32 location = GetLocation(name);

		if (location != -1)
		{
			glCall(glUniform4f(location, a.x, a.y, a.z, a.w));
			UnBind();
			return true;
		}

		UnBind();
		return false;
	}

	bool ShaderProgram::SetUniform(const std::string& name, int i) const
	{
		Bind();
		i32 location = GetLocation(name);

		if (location != -1)
		{
			glCall(glUniform1i(location, i));
			UnBind();
			return true;
		}

		UnBind();
		return false;
	}

	bool ShaderProgram::SetUniform(const std::string& name, glm::ivec3 a) const
	{
		Bind();
		i32 location = GetLocation(name);

		if (location != -1)
		{
			glCall(glUniform3i(location, a.x, a.y, a.z));
			UnBind();
			return true;
		}

		UnBind();
		return false;
	}

	bool ShaderProgram::SetUniform(const std::string& name, glm::uvec3 a) const
	{
		Bind();
		i32 location = GetLocation(name);

		if (location != -1)
		{
			glCall(glUniform3ui(location, a.x, a.y, a.z));
			UnBind();
			return true;
		}

		UnBind();
		return false;
	}

	bool ShaderProgram::SetUniform(const std::string& name, unsigned int i) const
	{
		Bind();
		i32 location = GetLocation(name);

		if (location != -1)
		{
			glCall(glUniform1ui(location, i));
			UnBind();
			return true;
		}

		UnBind();
		return false;
	}

	bool ShaderProgram::SetUniform(const std::string& name, const glm::mat4& mat) const
	{
		Bind();
		i32 location = GetLocation(name);

		if (location != -1)
		{
			glCall(glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(mat)));
			UnBind();
			return true;
		}

		UnBind();
		return false;
	}

	i32 ShaderProgram::GetLocation(const std::string& name) const
	{
		if (m_uniforms.find(name) != m_uniforms.end())
		{
			return m_uniforms[name];
		}

		i32 loc;
		glCall(loc = glGetUniformLocation(m_program_id, name.c_str()));

		if (loc != -1)
		{
			m_uniforms[name] = loc;
		}

		return loc;
	}
}

u32 CompileShader(const char* source, u32 type)
{
	if (source == nullptr) return 0;

	glCall(u32 id = glCreateShader(type));

	glCall(glShaderSource(id, 1, &source, nullptr));
	glCall(glCompileShader(id));

	int resoult;
	glCall(glGetShaderiv(id, GL_COMPILE_STATUS, &resoult));
	if (resoult == GL_FALSE)
	{
		if (type == GL_VERTEX_SHADER)
		{
			LOGERROR("<VERTEX SHADER COMPILE ERROR>");
		}
		else if (type == GL_FRAGMENT_SHADER)
		{
			LOGERROR("<FRAGMENT SHADER COMPILE ERROR>");
		}
		else if (type == GL_GEOMETRY_SHADER)
		{
			LOGERROR("<GEOMETRY SHADER COMPILE ERROR>");
		}

		char msg[512];
		glCall(glGetShaderInfoLog(id, 512, NULL, msg));
		LOGERROR(msg);
		glCall(glDeleteShader(id));
		return 0;
	}

	return id;
}

u32 CreateShaderProgramFromSource(const char* vertSrc, const char* geoSrc, const char* fragSrc)
{
	u32 vs = CompileShader(vertSrc, GL_VERTEX_SHADER);
	u32 gs = CompileShader(geoSrc, GL_GEOMETRY_SHADER);
	u32 fs = CompileShader(fragSrc, GL_FRAGMENT_SHADER);

	glCall(u32 programid = glCreateProgram());

	if (vs == 0 || fs == 0) return 0;
	if (geoSrc != nullptr && gs == 0) return 0;


	glCall(glAttachShader(programid, vs));
	if (gs != 0)
	{
		glCall(glAttachShader(programid, gs));
	}		
	glCall(glAttachShader(programid, fs));

	glCall(glLinkProgram(programid));
	glCall(glValidateProgram(programid));

	glCall(glDeleteShader(vs));
	if (gs != 0)
		glCall(glDeleteShader(gs));
	glCall(glDeleteShader(fs));

	return programid;
}
