cmake_minimum_required(VERSION 3.20)

project(doom LANGUAGES C)

set(sources)
set(source_files_list "")
foreach(source ${sources})
    list(APPEND source_files_list "${CMAKE_CURRENT_SOURCE_DIR}/src/${source}")
endforeach()
string(JOIN " " source_files_string "${source_files_list}")

add_executable("${PROJECT_NAME}" "${CMAKE_CURRENT_SOURCE_DIR}/src/main.c")

target_include_directories(
    "${PROJECT_NAME}"
    PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
)
target_sources("${PROJECT_NAME}" PUBLIC "${source_files_string}")

# configure libraries
# glad
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/libs/glad")
target_link_libraries("${PROJECT_NAME}" glad)

set_target_properties("${PROJECT_NAME}" PROPERTIES LINKER_LANGUAGE C)

# GLFW
set(GLFW_BUILD_DOCS OFF)
set(GLFW_BUILD_TESTS OFF)
set(GLFW_BUILD_EXAMPLES OFF)
set(GLFW_INSTALL OFF)

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/libs/glfw")
target_link_libraries("${PROJECT_NAME}" glfw)

# nuklear
if(MSVC)
    add_definitions("/wd4116")
endif()

# add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/libs/nuklear")
# target_link_libraries("${PROJECT_NAME}" nuklear)

# cimgui
if (WIN32)
    add_definitions("-DIMGUI_IMPL_API=extern \"C\" __declspec\(dllexport\)")
else(WIN32)
    add_definitions("-DIMGUI_IMPL_API=extern \"C\" ")
endif(WIN32)

if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/libs/cimgui/imgui/imgui_tables.cpp")
	set(TABLES_SOURCE "${CMAKE_CURRENT_SOURCE_DIR}/libs/cimgui/imgui/imgui_tables.cpp")
else()
	set(TABLES_SOURCE "")
endif()

set(IMGUI_SOURCES 
    "${CMAKE_CURRENT_SOURCE_DIR}/libs/cimgui/cimgui.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/libs/cimgui/imgui/imgui.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/libs/cimgui/imgui/imgui_draw.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/libs/cimgui/imgui/imgui_demo.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/libs/cimgui/imgui/imgui_widgets.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/libs/cimgui/imgui/backends/imgui_impl_glfw.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/libs/cimgui/imgui/backends/imgui_impl_opengl3.cpp"
    ${TABLES_SOURCE} 
)

add_library(cimgui "${IMGUI_SOURCES}")
target_include_directories(cimgui PUBLIC 
    "${CMAKE_CURRENT_SOURCE_DIR}/libs/cimgui/imgui/backends"
    "${CMAKE_CURRENT_SOURCE_DIR}/libs/cimgui/imgui"
    "${CMAKE_CURRENT_SOURCE_DIR}/libs/cimgui"
    "${CMAKE_CURRENT_SOURCE_DIR}/libs/cimgui/generator/output"
)
target_link_libraries(cimgui glfw)

# add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/libs/cimgui")
target_link_libraries("${PROJECT_NAME}" cimgui)