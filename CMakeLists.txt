cmake_minimum_required(VERSION 3.10)
project(granular_render LANGUAGES CXX)

# Estándar de C++
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Buscar Cairo
find_package(PkgConfig REQUIRED)
pkg_check_modules(CAIRO REQUIRED cairo)

# Incluir directorios
include_directories(
    ${CMAKE_SOURCE_DIR}/include
    ${CAIRO_INCLUDE_DIRS}
)

# Archivos fuente
set(SOURCES
    src/main.cpp
    src/renderer.cpp
    src/thread_pool.cpp
)

# Ejecutable
add_executable(granular_render ${SOURCES})

# Vincular librerías
target_link_libraries(granular_render
    ${CAIRO_LIBRARIES}
)

# Flags extra para Cairo (si hacen falta)
target_compile_options(granular_render PRIVATE ${CAIRO_CFLAGS_OTHER})

