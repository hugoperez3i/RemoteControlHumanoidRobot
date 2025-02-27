cmake_minimum_required(VERSION 3.10)

set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")

project(ControlClient VERSION 0.1)

find_package(glfw3 CONFIG REQUIRED)
find_package(Vulkan REQUIRED)

add_subdirectory(lib/robot_ctrl_iface)
add_subdirectory(lib/cli_sock)

find_package(Git QUIET)
    execute_process(COMMAND ${GIT_EXECUTABLE} 
        submodule update --init --recursive
    )
add_subdirectory(lib/GUI/imgui)

add_executable(${PROJECT_NAME} main.cpp)

target_link_libraries(${PROJECT_NAME} 
    PRIVATE DearImGUI
)

