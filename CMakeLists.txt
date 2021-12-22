cmake_minimum_required(VERSION 3.12)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include("cmake/HunterGate.cmake")

HunterGate(
        URL "https://github.com/cpp-pm/hunter/archive/v0.23.251.tar.gz"
        SHA1 "5659b15dc0884d4b03dbd95710e6a1fa0fc3258d"
)

option(BUILD_TESTS "Build tests" ON)

project(client_app)
#string(APPEND CMAKE_CXX_FLAGS " -pedantic -Werror -Wall -Wextra")
#string(APPEND CMAKE_CXX_FLAGS " -Wno-unused-command-line-argument")
#string(APPEND CMAKE_CXX_FLAGS " -Wshadow -Wnon-virtual-dtor")

add_library(${PROJECT_NAME} STATIC
        include/NetCommon.hpp
        include/NetThreadSafeQueue.hpp
        include/NetConnection.hpp
        include/NetClient.hpp
        sources/source.cpp
        third-party/xtea3_lib/xtea3.h
        third-party/xtea3_lib/xtea3.cpp
        third-party/PicoSHA2/picosha2.h
)

hunter_add_package(SFML)
#hunter_add_package(GTest)
hunter_add_package(Boost COMPONENTS)
hunter_add_package(Boost COMPONENTS system)

find_package(Boost CONFIG REQUIRED)
find_package(Boost CONFIG REQUIRED system)
find_package(SFML COMPONENTS graphics CONFIG REQUIRED)
find_package(OpenGL REQUIRED)
find_package(Threads)
find_package(GTest CONFIG REQUIRED)

include_directories( ${CURSES_INCLUDE_DIRS} )

add_executable(client main/main.cpp)

target_include_directories(${PROJECT_NAME} PUBLIC
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
        "$<INSTALL_INTERFACE:include>"
        "${SFML_ROOT}/include"
        "${OPENGL_INCLUDE_DIRS}"
        "${BOOST_ROOT}/include"
)

target_link_libraries(client PUBLIC ${PROJECT_NAME} Threads::Threads ${OPENGL_LIBRARIES} sfml-graphics Boost::boost Boost::system)

if(BUILD_TESTS)
    add_executable(tests
            ${CMAKE_CURRENT_SOURCE_DIR}/tests/test.cpp
            )
    target_link_libraries(tests ${PROJECT_NAME} GTest::gtest_main GTest::gmock_main)
    enable_testing()
    add_test(NAME unit_tests COMMAND tests)
endif()
