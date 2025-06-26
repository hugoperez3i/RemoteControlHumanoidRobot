cmake_minimum_required(VERSION 3.10)
project(ControlServer VERSION 1.0.0)

# Full CMake tutorial playlist: https://www.youtube.com/playlist?list=PLalVdRk2RC6o5GHu618ARWh0VO0bFlif4

set(CMAKE_CXX_STANDARD 17)

set(SOURCES
    Server.cpp
)

add_subdirectory(controllers)
enable_testing()
add_subdirectory(test)

set(EXECUTABLE_NAME "ControlServer")
add_executable(${EXECUTABLE_NAME} ${SOURCES})
set_target_properties(${EXECUTABLE_NAME} PROPERTIES WIN32_EXECUTABLE false)  # keep false to include console output
target_link_libraries(${EXECUTABLE_NAME} 
    ControllersLib
    ws2_32
    SQLite3
)


install(TARGETS ${EXECUTABLE_NAME} RUNTIME DESTINATION bin)

# https://cmake.org/cmake/help/book/mastering-cmake/chapter/Packaging%20With%20CPack.html

include(InstallRequiredSystemLibraries)
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
set(CPACK_NSIS_MUI_ICON "${CMAKE_CURRENT_SOURCE_DIR}/icons/imgIcon.ico")
set(CPACK_NSIS_MUI_UNIICON "${CMAKE_CURRENT_SOURCE_DIR}/icons/imgIcon.ico")
set(CPACK_CREATE_DESKTOP_LINKS ${EXECUTABLE_NAME})
set(CPACK_NSIS_INSTALLED_ICON_NAME "build//${EXECUTABLE_NAME}.exe")
set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")
include(CPack)