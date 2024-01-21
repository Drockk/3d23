cmake_minimum_required(VERSION 3.15)
project(Engine CXX)

add_compile_definitions(PROJECT_NAME="${PROJECT_NAME}" PROJECT_DIR="${CMAKE_CURRENT_SOURCE_DIR}")

add_library(engine
        Mesh.h Mesh.cpp
        ColorMaterial.h ColorMaterial.cpp
        mesh_loader.h mesh_loader.cpp
        texture.h texture.cpp
        PhongMaterial.h PhongMaterial.cpp
 "Light.h")

target_link_libraries(engine PUBLIC objreader PRIVATE spdlog::spdlog)