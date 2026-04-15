#pragma once

#include <Core/Log.h>
#include <filesystem>
#include <string>

namespace Mani
{
    constexpr LogChannel LogShaderImporter("ShaderImporter");

    struct Shader;

    class ShaderImporter
    {
    public:
        static bool importFromPath(const std::filesystem::path& path, Shader& outShader);
        static bool exportToPath(const std::filesystem::path& path, const Shader& shader);
    
    private:

        /*
         * Extracts a shader sources from a shader source file.
         * The expected format is as follow:
         *
         * #type vertex\n
         * [vertex shader sources]
         * #type fragment\n
         * [fragment shader sources]
         *
         * returns true if the parse was successful.
         */
        static bool parseShaderSourceFileFromPath(const std::filesystem::path& path, std::string& outFileName, std::string& outVertexSource, std::string& outFragmentSource);
    };
}
