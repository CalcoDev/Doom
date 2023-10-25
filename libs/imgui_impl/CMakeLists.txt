## CUSTOM MADE BY CALCOPOD
## BAD LMAO
cmake_minimum_required(VERSION 3.20)
project(imgui_impl)

add_library(
    imgui_impl 
    "include/imgui_impl/imgui_impl_glfw.cpp"
    "include/imgui_impl/imgui_impl_opengl3.cpp"
)
target_include_directories(imgui_impl PUBLIC "include/")