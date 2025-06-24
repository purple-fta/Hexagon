cmake_minimum_required(VERSION 3.24)
project(Hexagon)

set(CMAKE_CXX_STANDARD 20)

include(FetchContent)

FetchContent_Declare(
        SFML
        GIT_REPOSITORY https://github.com/SFML/SFML.git
        GIT_TAG 3.0.0
        GIT_SHALLOW ON
        EXCLUDE_FROM_ALL
        SYSTEM
)

FetchContent_MakeAvailable(SFML)

set(SRC_DIR "${CMAKE_SOURCE_DIR}/src")
set(ASSETS_DIR "${CMAKE_SOURCE_DIR}/assets")

file(GLOB_RECURSE SOURCES "${SRC_DIR}/*.cpp")


add_executable(Hexagon ${SOURCES})
target_link_libraries(Hexagon PRIVATE SFML::Graphics)

add_custom_command(TARGET Hexagon POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${ASSETS_DIR}" "$<TARGET_FILE_DIR:Hexagon>/assets"
)
