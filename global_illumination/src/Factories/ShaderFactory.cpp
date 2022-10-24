#include "ShaderFactory.h"
#include "Global/Logger.h"

#include "FileUtilities.h"
#include <assert.h>

namespace GL
{
	ShaderProgram GL::ShaderFactory::Create(const std::string& name)
	{
        LOGINFO("ShaderFactory: Creating shader %s", name.c_str());

        std::string vertex = readFile("Shaders\\" + name + ".vert");
        std::string fragment = readFile("Shaders\\" + name + ".frag");
        std::string geometry = readFile("Shaders\\" + name + ".geom");

        if (geometry.empty())
        {
            ShaderProgram shader(vertex.c_str(), fragment.c_str());

            assert(shader.IsValid());

            return shader;
        }
        else
        {
            ShaderProgram shader(vertex.c_str(), geometry.c_str(), fragment.c_str());

            assert(shader.IsValid());

            LOGDEBUG("With geometry shader");

            return shader;
        }

	}

    void ShaderFactory::Reload(ShaderProgram& shader, const std::string& name)
    {
        std::string vertex = readFile("Shaders\\" + name + ".vert");
        std::string fragment = readFile("Shaders\\" + name + ".frag");

        if (shader.Reload(vertex.c_str(), fragment.c_str()))
        {
            LOGINFO("ShaderFactory: Reload shader %s", name.c_str());
        }
        else
        {
            LOGERROR("ShaderFactory: Reload shader %s Failed", name.c_str());
        }
    }
}


