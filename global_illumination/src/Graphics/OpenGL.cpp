#include "OpenGL.h"
#include "GLFW/glfw3.h"
#include "Global/Logger.h"

#include <exception>
#include <stdexcept>

namespace GL
{
	void InitOpenGL()
	{
		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		{
			LOGERROR("Failed to initialize GLAD");
			throw std::runtime_error("Failed to initialize GLAD");
		}

		// print OpenGL version
		LOGINFO("Using OpenGL");
		LOGINFO((const char*)glGetString(GL_VERSION));
	}

	void glClearError()
	{
		while (glGetError());
	}

	void glCheckError(const char* file, int line)
	{
		while (GLenum error = glGetError())
		{
			switch (error)
			{
			case GL_INVALID_ENUM:
				LOGERROR("[%s %i] %u <Invalid Enum>", file, line, error);
				assert(false);
				break;
			case GL_INVALID_VALUE:
				LOGERROR("[%s %i] %u <Invalid Value>", file, line, error);
				assert(false);
				break;
			case GL_INVALID_OPERATION:
				LOGERROR("[%s %i] %u <Invalid Operation>", file, line, error);
				assert(false);
				break;
			default:
				LOGERROR("[%s %i] %u <ERROR>", file, line, error);
				assert(false);
				break;
			}
		}
	}
}


