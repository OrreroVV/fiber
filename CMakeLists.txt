cmake_minimum_required(VERSION 2.8)
project(hzh)

include (cmake/utils.cmake)

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} -rdynamic -O3 -ggdb -std=c++11 -w")


include_directories(.)

# find_package(Boost REQUIRED)
# if(Boost_FOUND)
#     include_directories(${Boost_INCLUDE_DIRS})
# endif()

set(LIB_SRC
    hzh/utils.cpp
    hzh/thread.cpp
    hzh/fiber.cpp
    hzh/scheduler.cpp
    hzh/timer.cpp
    hzh/iomanager.cpp
    )



add_library(hzh SHARED ${LIB_SRC})
force_redefine_file_macro_for_sources(hzh) #__FILE__

find_library(PTHREAD pthread)

set(LIBS
        hzh
        dl
        pthread
        #yaml-cpp
        )

hzh_add_executable(test_thread "tests/test_thread.cpp" hzh "${LIBS}")

hzh_add_executable(test_fiber "tests/test_fiber.cpp" hzh "${LIBS}")
hzh_add_executable(test_scheduler "tests/test_scheduler.cpp" hzh "${LIBS}")
hzh_add_executable(test_iomanager "tests/test_iomanager.cpp" hzh "${LIBS}")

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)
