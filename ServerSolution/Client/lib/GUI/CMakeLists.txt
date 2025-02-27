cmake_minimum_required(VERSION 3.10)

project(DearImGUI)

add_library(${PROJECT_NAME}
    imgui/imconfig.h
    imgui/imgui_draw.cpp
    imgui/imgui_internal.h
    imgui/imgui_tables.cpp
    imgui/imgui_widgets.cpp
    imgui/imgui.cpp
    imgui/imgui.h
    imgui/imstb_rectpack.h
    imgui/imstb_textedit.h
    imgui/imstb_truetype.h
    imgui/backends/imgui_impl_vulkan.h 
    imgui/backends/imgui_impl_vulkan.cpp
    imgui/backends/imgui_impl_glfw.h   
    imgui/backends/imgui_impl_glfw.cpp

    imgui/imgui_demo.cpp
)

target_link_libraries(${PROJECT_NAME} PUBLIC
    glfw
    Vulkan::Vulkan
)

target_include_directories(${PROJECT_NAME} PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/imgui
    ${CMAKE_CURRENT_SOURCE_DIR}/imgui/backends
)
