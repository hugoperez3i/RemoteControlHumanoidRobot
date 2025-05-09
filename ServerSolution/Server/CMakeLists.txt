cmake_minimum_required(VERSION 3.10)
project(ControlServer VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)

set(SOURCES
    Server.cpp
)

add_subdirectory(controllers)
enable_testing()
add_subdirectory(test)

add_executable(ControlServer ${SOURCES})
set_target_properties(ControlServer PROPERTIES WIN32_EXECUTABLE false)  # keep false to include console output
target_link_libraries(ControlServer 
    ControllersLib
    ws2_32
    SQLite3
)
# add_library(sqlite3 STATIC lib/sqlite3.c)
# target_include_directories(sqlite3 PUBLIC lib/sqlite)


install(TARGETS ControlServer RUNTIME DESTINATION bin)

include(InstallRequiredSystemLibraries)
set(CPACK RESOURCE FILE LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
set(CPACK_PACKAGE_VERSION_MAJOR "1")
set(CPACK_PACKAGE_VERSION_MINOR "0")
include(CPack)