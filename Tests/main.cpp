#if MANI_WINDOWS
#include "windows.h"
#include <strsafe.h>
#endif

#include <filesystem>
#include <iostream>

int main(int argc, char** argv)
{
#ifdef MANI_WINDOWS
    namespace fs = std::filesystem;

    // runTests signature
    typedef void(__stdcall* f_runTests) ();
    
    // base build path.
    const std::string prefix = "../../bin/";
    const std::string basePath = prefix + MANI_TESTS_BUILD_OUTPUTDIR;

    std::vector<fs::path> paths;
    if (fs::exists(basePath) && fs::is_directory(basePath))
    {
        for (const auto& entry : fs::directory_iterator(basePath))
        {
            if (fs::is_regular_file(entry) && entry.path().extension() == ".dll")
            {
                const std::string path = entry.path().string();
                if (path.find("Tests") != std::string::npos)
                {
                    paths.push_back(entry.path());
                }
            }
        }
    }

    for (const fs::path& path : paths)
    {
        const std::string sPath = path.string();
        std::cout << "Running " << sPath << "\n\n";

        std::wstring wpath(sPath.begin(), sPath.end());
   
        HINSTANCE dllHandle = LoadLibrary(wpath.c_str());
    
        if (dllHandle)
        {
            // resolve function address here
            f_runTests runTests = (f_runTests)GetProcAddress(dllHandle, "runTests");
            if (runTests)
            {
                runTests();
            }
            else
            {
                std::cout << "Could not locate the runTests function for " << sPath <<"\n";
            }
        }
        else
        {
            std::cout << "Could not load the shared library for " << sPath << " (" << GetLastError() << ")\n";
        }
            
        std::cout << "\n";
    }

    return EXIT_SUCCESS;
#else
    std::cout << "Platform not supported" << std::endl;
    return EXIT_FAILURE;
#endif
}