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