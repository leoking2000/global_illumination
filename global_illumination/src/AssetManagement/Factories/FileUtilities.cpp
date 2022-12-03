#include "FileUtilities.h"
#include <fstream>
#include <sstream>

namespace GL
{
    std::string readFile(const std::string& filepath)
    {
        std::ifstream input_file(filepath);

        if (!input_file.is_open())
        {
            return std::string();
        }

        return std::string((std::istreambuf_iterator<char>(input_file)), std::istreambuf_iterator<char>());
    }

    std::string DirNameOf(const std::string& filepath)
    {
        size_t pos = filepath.find_last_of("\\/");
        return (std::string::npos == pos) ? "" : filepath.substr(0, pos);
    }
}


