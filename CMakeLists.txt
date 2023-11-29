cmake_minimum_required(VERSION 3.0)

project(Imgui_dx11)

include_directories(
    ./include
    ./include/imgui
    ./include/stb_image
)

file(GLOB_RECURSE src "./src/*.cpp")

add_executable(MyExecutable ${src})
