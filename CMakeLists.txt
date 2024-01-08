 
project(Imgui_dx11)

include_directories(
    ./include
    ./include/imgui
    ./include/stb_image
    ./include/widget
    ./include/tools
)

file(GLOB_RECURSE src "./src/*.cpp")

add_executable(Imgui_dx11 ${src})
