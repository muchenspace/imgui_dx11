cmake_minimum_required(VERSION 3.25)

 project(Imgui_dx11)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


if (CMAKE_VERSION VERSION_LESS 3.28)
    set(CMAKE_EXPERIMENTAL_CXX_MODULE_CMAKE_API "2182bf5c-ef0d-489a-91da-49dbc3090d2a")
    set(CMAKE_EXPERIMENTAL_CXX_MODULE_DYNDEP ON)
endif()


include_directories(
    ./include
    ./include/imgui
    ./include/stb_image
    ./include/widget
    ./include/tools
)

file(GLOB_RECURSE src "./src/*.cpp")
file(GLOB_RECURSE module "./module/*.ixx")


add_executable(Imgui_dx11 ${src})


target_sources(Imgui_dx11 PRIVATE ${src} ${module})