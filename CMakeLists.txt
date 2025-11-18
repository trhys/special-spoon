cmake_minimum_required(VERSION 3.15...4.0)

project(special-spoon
    VERSION 0.1
    LANGUAGE CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    add_definitions(-DSS_PLATFORM_WINDOWS)
endif()

add_subdirectory(special-spoon-core)

include(GNUInstallDirs)
install(
    FILES
        cmake/special-spoonConfig.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/special-spoon
)

if(BUILD_SANDBOX)
    add_subdirectory(Sandbox)
    target_link_libraries(Sandbox PRIVATE special-spoon-core)
endif()

if(DEBUG_MODE)
    add_definitions(-DSS_DEBUG_ENABLED -DSS_ASSERTS_ENABLED)
endif()