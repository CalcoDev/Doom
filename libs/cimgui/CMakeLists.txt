## CUSTOM MADE BY CALCOPOD
## BAD LMAO
cmake_minimum_required(VERSION 3.20)
project(cimgui)

if (WIN32)
    add_definitions("-DIMGUI_IMPL_API=extern \"C\" __declspec\(dllexport\)")
else(WIN32)
    add_definitions("-DIMGUI_IMPL_API=extern \"C\" ")
endif(WIN32)

if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui_tables.cpp")
	set(TABLES_SOURCE "${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui_tables.cpp")
else()
	set(TABLES_SOURCE "")
endif()

set(IMGUI_SOURCES 
    "${CMAKE_CURRENT_SOURCE_DIR}/cimgui.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui_draw.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui_demo.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui_widgets.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/imgui/backends/imgui_impl_glfw.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/imgui/backends/imgui_impl_opengl3.cpp"
    ${TABLES_SOURCE} 
)

add_library(cimgui "${IMGUI_SOURCES}")
target_include_directories(cimgui PUBLIC 
    "${CMAKE_CURRENT_SOURCE_DIR}/imgui/backends"
    "${CMAKE_CURRENT_SOURCE_DIR}/imgui"
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/generator/output"
)