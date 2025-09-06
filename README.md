# Manifold
Manifold is an ECS Game Engine focused on modularity. It promotes a System based architecture and procedural code.

# Requirements
Some of the dependencies are provided with the engine submodule. This includes a Premake5 and a lua interpreter

## Windows
* Visual Studio 2022
    * Visual Studio core Editor
    * Game development with C++
    * MSVC v143
    * Windows 11 SDK (10.0.22621.0)
    * Windows Universal CRT SDK

# Setup for a blank project
Manifold is provided as a static library. The recommended way to install it is through the git submodule feature

## Windows
### Project Template
For your convenience, please use this github template to kickstart a new project setup https://github.com/ManifoldEngine/ManifoldTemplate

### Download Manifold
1. Create your git repository as you see fit
2. `git submodule add https://github.com/ManifoldEngine/Manifold.git Engine`
    * Note that we have given a path to the submodule. However, it can be named as you please
3. `git submodule update --init --recursive` to pull all of Manifold's third party dependencies

### Build System
Manifold uses premake5 to generate its build files. The template project comes with a `GenerateProjectFiles.bat` script that runs the following command:
`Engine/ThirdParties/premake/premake5.exe vs2022`
This script generates the vs2022 solution. Each time that you add/move/remove a file to/from your project, you must re-generate project files in order to build the project.
Feel free to read and edit premake5.lua to link more libraries or change the build configuration.
More info at https://premake.github.io/
