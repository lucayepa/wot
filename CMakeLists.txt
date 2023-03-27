cmake_minimum_required(VERSION 3.13)

project(
  wot
  VERSION 0.1
  DESCRIPTION "A decentralized Web of Trust"
  LANGUAGES CXX)

# This part only if this is the main project and not included
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
  find_package(Doxygen)
  if(Doxygen_FOUND)
    add_subdirectory(doc)
  else()
    message(STATUS "Doxygen not found: no Doxygen docs")
  endif()
endif()

set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREAD ON)
find_package(Boost 1.54.0 REQUIRED COMPONENTS log log_setup program_options)

find_package(OpenSSL REQUIRED)

include(FetchContent)
FetchContent_Declare(
  json
  URL https://github.com/nlohmann/json/releases/download/v3.11.2/json.tar.xz
)

FetchContent_Declare(
  tomlplusplus
  GIT_REPOSITORY https://github.com/marzer/tomlplusplus.git
  GIT_TAG        v3.3.0
)

FetchContent_MakeAvailable(json)
FetchContent_MakeAvailable(tomlplusplus)

set(CMAKE_CXX_STANDARD 17)

add_executable(wot src/wot.cpp)

target_include_directories(wot PUBLIC src/generated)
target_include_directories(wot PUBLIC ${tomlplusplus_SOURCE_DIR})
target_include_directories(wot PUBLIC ${Boost_INCLUDE_DIR})

target_link_libraries(wot ${Boost_LIBRARIES})
target_link_libraries(wot OpenSSL::SSL)
target_link_libraries(wot nlohmann_json)
