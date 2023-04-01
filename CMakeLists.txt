cmake_minimum_required(VERSION 3.13)

project(
  wot
  VERSION 0.1
  DESCRIPTION "A decentralized Web of Trust"
  LANGUAGES CXX
)

enable_testing()

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
find_package(Boost 1.54.0 REQUIRED COMPONENTS log log_setup program_options unit_test_framework)

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

find_program (QUICKTYPE quicktype)
if(NOT QUICKTYPE)
    message(FATAL_ERROR "Quicktype not found. Please install it: `npm install -g quicktype`")
endif()

set(CMAKE_CXX_STANDARD 17)

file(GLOB_RECURSE SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} CONFIGURE_DEPENDS src/*.cpp)
list(REMOVE_ITEM SOURCES src/wot.cpp)

add_executable(wot src/wot.cpp ${SOURCES} node_qt.hpp)

add_custom_command(
  OUTPUT node_qt.hpp
  COMMAND ${QUICKTYPE} -s schema -t node -l C++ --namespace wot_qt --no-boost --include-location global-include -S ${CMAKE_CURRENT_SOURCE_DIR}/src/link.json -o node_qt.hpp ${CMAKE_CURRENT_SOURCE_DIR}/src/node.json
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/link.json ${CMAKE_CURRENT_SOURCE_DIR}/src/node.json
  VERBATIM
)

function(includes_and_libs_for target)
  target_include_directories(${target} PUBLIC src)
  target_include_directories(${target} PUBLIC ${CMAKE_CURRENT_BINARY_DIR})
  target_include_directories(${target} PUBLIC ${tomlplusplus_SOURCE_DIR})
  target_include_directories(${target} PUBLIC ${Boost_INCLUDE_DIR})
  target_link_libraries(${target} ${Boost_LIBRARIES})
  target_link_libraries(${target} OpenSSL::SSL)
  target_link_libraries(${target} nlohmann_json)
endfunction()

includes_and_libs_for(wot)

file(GLOB_RECURSE tests RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}/test CONFIGURE_DEPENDS test/*.cpp)
list(REMOVE_ITEM tests main.cpp)

foreach(TESTFILE IN LISTS tests)
  add_executable(${TESTFILE} test/main.cpp ${SOURCES} test/${TESTFILE} node_qt.hpp)

  includes_and_libs_for(${TESTFILE})
  target_link_libraries(${TESTFILE} ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})
  target_compile_definitions(${TESTFILE} PRIVATE "BOOST_TEST_DYN_LINK=1")

  add_test(NAME ${TESTFILE} COMMAND ${TESTFILE})
endforeach()
