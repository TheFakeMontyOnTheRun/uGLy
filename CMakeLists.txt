cmake_minimum_required(VERSION 3.11)
project(gles1cl)



set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 11)
SET(CMAKE_CXX_STANDARD_REQUIRED ON)
SET(CMAKE_C_STANDARD_REQUIRED ON)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D_FORTIFY_SOURCE=3 -Wall -Wshadow -Wextra -pedantic -g -DSDL_DISABLE_IMMINTRIN_H -DBPP16")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_FORTIFY_SOURCE=3 -W -Wall -Wextra -Wno-unused-parameter -DGTEST_LANG_CXX11=0 -DBPP16")

if (APPLE)
    add_definitions(-D__GLIBCXX__)
endif (APPLE)

enable_testing()

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake-sdl2")

include_directories(
        ${SDL2_INCLUDE_DIR}
        ${SDL2_IMAGE_INCLUDE_DIRS}
        third_party/fpsqrt
)

include_directories(include)

find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)


include_directories(third_party/gtest)
include_directories(third_party/gtest/include)

include_directories(third_party/gmock/include)
include_directories(third_party/gmock)

set(TEST_SOURCE_FILES
        tests/TestFixP.cpp
        tests/common.cpp

        third_party/gtest/src/gtest-all.cc
        third_party/gmock/src/gmock-all.cc
        third_party/gmock/src/gmock_main.cc
)


set(SOURCE_FILES
        src/es10cl.c
        src/es11.c
        src/matricesFP.c
        src/raster.c
        third_party/fpsqrt/fpsqrt.c
)


include(FindPkgConfig)

add_executable(tri src/sdl2.c samples/tri.c ${SOURCE_FILES} )

target_include_directories(tri PUBLIC ${SDL2_INCLUDE_DIR} ${SDL2_IMAGE_INCLUDE_DIRS})
target_link_libraries(tri LINK_PUBLIC ${SDL2_LIBRARY} ${SDL2_IMAGE_LIBRARIES} m)

add_executable(ugly-test  ${TEST_SOURCE_FILES} ${SOURCE_FILES} )

target_link_libraries(ugly-test PRIVATE Threads::Threads)

add_test(NAME ugly-test COMMAND ugly-test)


