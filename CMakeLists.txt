cmake_minimum_required(VERSION 3.10)
project(hzh)

include (cmake/utils.cmake)

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_CXX_FLAGS " -g -O0 -ggdb -std=c++17 -w")


include_directories(.)
include_directories(/opt/glibc-2.31/include)
include_directories(3rd/openssl/include)
# add_definitions("-D__DEBUG__")

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
        /home/hzh/workspace/fiber/3rd/openssl/lib64/libssl.a
        /home/hzh/workspace/fiber/3rd/openssl/lib64/libcrypto.a
        dl
        pthread
        rt
        z
        m
        # /opt/glibc-2.31/lib/libpthread.so
        # /opt/glibc-2.31/lib/librt.so
        #yaml-cpp
        )

hzh_add_executable(test_thread "tests/test_thread.cpp" hzh "${LIBS}")

hzh_add_executable(test_fiber "tests/test_fiber.cpp" hzh "${LIBS}")
hzh_add_executable(test_scheduler "tests/test_scheduler.cpp" hzh "${LIBS}")
hzh_add_executable(test_iomanager "tests/test_iomanager.cpp" hzh "${LIBS}")

hzh_add_executable(main "tests/main.cpp" hzh "${LIBS}")

hzh_add_executable(test_server "tests/test_server.cpp" hzh "${LIBS}")
hzh_add_executable(test_client "tests/test_client.cpp" hzh "${LIBS}")

hzh_add_executable(test_ssl "tests/test_ssl.cpp" hzh "${LIBS}")

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)
