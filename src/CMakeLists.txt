find_program (QUICKTYPE quicktype)
if(NOT QUICKTYPE)
    message(FATAL_ERROR "Quicktype not found. Please install it: `npm install -g quicktype`")
endif()

add_custom_command(
  OUTPUT node_qt.hpp
  COMMAND ${QUICKTYPE} -s schema -t node -l C++ --namespace wot_qt --no-boost --include-location global-include
    -S ${CMAKE_CURRENT_SOURCE_DIR}/link.json -o node_qt.hpp ${CMAKE_CURRENT_SOURCE_DIR}/node.json
  DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/link.json
    ${CMAKE_CURRENT_SOURCE_DIR}/node.json
  VERBATIM
)

file(GLOB filters RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} CONFIGURE_DEPENDS filters/*.hpp)
file(GLOB sources RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} CONFIGURE_DEPENDS *.cpp)
list(REMOVE_ITEM sources wot.cpp)

add_library(wotlib OBJECT)
target_sources(wotlib PRIVATE ${sources} ${filters} node_qt.hpp)
target_include_directories(wotlib PUBLIC . ${tomlplusplus_SOURCE_DIR} ${Boost_INCLUDE_DIR} ${CMAKE_CURRENT_BINARY_DIR}) # node_qt.hpp is in binary
target_link_libraries(wotlib PRIVATE ${Boost_LIBRARIES} nlohmann_json::nlohmann_json)

add_executable(wot wot.cpp)
target_include_directories(wot PUBLIC . ${tomlplusplus_SOURCE_DIR} ${Boost_INCLUDE_DIR} ${CMAKE_CURRENT_BINARY_DIR}) # node_qt.hpp is in binary
target_link_libraries(wot ${Boost_LIBRARIES} OpenSSL::SSL nlohmann_json::nlohmann_json wotlib)
