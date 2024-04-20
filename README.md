# Manifold
Manifold is an ECS Game Engine focused on modularity. It promotes a System based architecture and procedural code.

# Requirements
Some of the dependencies are provided with the engine submodule. This includes a Premake5 executable, a cmake executable, and a lua interpreter

## Windows
* Visual Studio 2022
    * Visual Studio core Editor
    * Game development with C++
    * MSVC v143
    * MSVC v141
    * Windows 11 SDK
    * Windows Universal CRT SDK
* Python 3

# Setup for a blank project
Manifold is provided as a static library. The recommended way to install it is through the git submodule feature

## Windows
### download Manifold
1. Create your git repository as you see fit
2. `git submodule add https://github.com/ManifoldEngine/Manifold.git Engine`
    * Note that we have given a path to the submodule. However, it can be named as you please
3. `git submodule update --init --recursive` to pull all of Manifold's third party dependencies

### initialize project files
1. Open a Windows command prompt at the root of your repo
2. `cd Engine` or the path you set the Manifold submodule
3. Run `InitProject.bat [YourProjectName]`
    * this will create an empty project structure at `../` from Manifold's location, and buils all of the third party libraries
    * note that this script can be run to re-generate the original template files. It has a few option flag that can be passed
        * `InitProject.bat -h` or `InitProject.bat --help` to print a help page
        * `InitProject.bat [YourProjectName] --no-template` to *not* overwrite the project's structure
        * `InitProject.bat [YourProjectName] --no-thirdparties` to *not* rebuild all of the third party projects

Your project is ready to be edited. In the folder above Manifold, you should find the folloing structure:
```
| Config
|-- [for any config file set outside of the c++ project]
| Engine/
|-- [Manifold's sources]
| [YourProjectName]
|-- Assets/
|---- [The default location for your assets]
|-- Sources/
|---- main.cpp
|---- [The default location for your c++ code]
```

### Build System
Manifold uses premake5 to generate its build files. The template project comes with a `GenerateProjectFiles.bat` script that runs the following command:
`Engine/ThirdParties/premake/premake5.exe vs2022`
This script generates the vs2022 solution. Each time that you add/move/remove a file to/from your project, you must re-generate project files in order to build the project.
Feel free to read and edit premake5.lua to link more libraries or change the build configuration.
More info at https://premake.github.io/
