cmake_minimum_required(VERSION 3.13)

if (APPLE)
    include(toolchains/macToolchain.cmake)
elseif (WIN32)
    include(toolchains/windowsToolchain.cmake)
endif ()

project(seele)

set(CONAN_DISABLE_CHECK_COMPILER TRUE)

include(CTest)
include(GoogleTest)

find_package(GTest REQUIRED)

add_subdirectory(seele)
add_subdirectory(core)
