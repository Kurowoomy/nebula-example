# Nebula Example

## Requirements
1. OS: Windows or Linux
2. Compiler with support for C++17.
3. GPU supporting Vulkan
4. [CMake 3.15+](https://cmake.org/download/)
5. [Vulkan SDK 1.1.130+](https://vulkan.lunarg.com/sdk/home)
6. [Python 3.5+](https://www.python.org/downloads)
    * Python modules required:
        1. numpy
        2. jedi
        
        As root/admin: `python -m pip install numpy jedi`
    * Python requirements (Windows):
        1. Correct architecture (64-bit if you're building for 64-bit systems)
        2. Installed for all users
        3. Added to PATH
        4. Installed with debugging symbols and binaries

## Setup

#### Setup config and toolkit

1. `./fips set config vulkan-win64-vstudio-debug` in your project directory

#### How to build project

In your project directory:

  1. `fips physx build`
  2. `fips anyfx setup`
  3. `fips build`
  4. `fips physx deploy`

*Note that step 3 can be done from within Visual Studio (`fips open`)*

#### Set environment variables

Remember to run `fips nebula` verb to set work and toolkit directory registry variables:

  * `fips nebula set work {PATH}`
  * `fips nebula set toolkit {PATH}`
