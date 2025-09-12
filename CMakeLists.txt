cmake_minimum_required(VERSION 3.12)
project(granular_cmap_render)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Cairo
find_package(PkgConfig REQUIRED)
pkg_check_modules(CAIRO REQUIRED cairo)

add_executable(granular_cmap_render
    main.cpp
    thread_pool.cpp
)

target_include_directories(granular_cmap_render PRIVATE
    ${CAIRO_INCLUDE_DIRS}
)

target_link_libraries(granular_cmap_render
    ${CAIRO_LIBRARIES}
)

# Para que el compilador vea thread_pool.hpp
target_include_directories(granular_cmap_render PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

