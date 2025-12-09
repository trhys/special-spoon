cmake_minimum_required(VERSION 3.28)

project(special-spoon
    VERSION 0.1
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

if(WIN32)
    message(STATUS "Configuring for windows")
elseif(UNIX)
    message(STATUS "Configuring for unix")
    set(CMAKE_POSITION_INDEPENDENT_CODE ON)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
else()
    message(FATAL_ERROR "Unsupported platform")
endif()

include(GenerateExportHeader)

add_subdirectory(special-spoon-core)

include(GNUInstallDirs)
install(
    FILES
        cmake/special-spoonConfig.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/special-spoon
)

if(BUILD_SANDBOX)
    add_subdirectory(Sandbox)
endif()