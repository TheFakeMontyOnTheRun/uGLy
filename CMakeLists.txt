cmake_minimum_required(VERSION 3.11)
project(gles1cl)

set(CMAKE_C_STANDARD 99)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake-sdl2")

include_directories(
        ${SDL2_INCLUDE_DIR}
        ${SDL2_IMAGE_INCLUDE_DIRS}
        third_party/fpsqrt
)

include_directories(include)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D_FORTIFY_SOURCE=3 -Wall -Wshadow -Wextra -pedantic -g -DSDL_DISABLE_IMMINTRIN_H")

find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)

add_executable(tri
        src/es10cl.c
        src/es11.c
        src/matricesFP.c
        src/raster.c
        src/sdl2.c
        samples/tri.c
        third_party/fpsqrt/fpsqrt.c
)


include(FindPkgConfig)

target_include_directories(tri PUBLIC ${SDL2_INCLUDE_DIR} ${SDL2_IMAGE_INCLUDE_DIRS})
target_link_libraries(tri LINK_PUBLIC ${SDL2_LIBRARY} ${SDL2_IMAGE_LIBRARIES} m)

