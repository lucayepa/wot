cmake_minimum_required(VERSION 3.13)

project(wot)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_executable(wot src/wot.cpp)

include_directories(src/generated /usr/include/nlohmann/ lib)

#set(Boost_DEBUG true)
set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREAD ON)

find_package(Boost 1.54.0 REQUIRED COMPONENTS log log_setup program_options)
INCLUDE_DIRECTORIES( ${Boost_INCLUDE_DIR} )

find_package(OpenSSL REQUIRED)

target_link_libraries(wot ${Boost_LIBRARIES})
target_link_libraries(wot OpenSSL::SSL)
