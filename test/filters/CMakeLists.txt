find_program (QUICKTYPE quicktype)
if(NOT QUICKTYPE)
    message(FATAL_ERROR "Quicktype not found. Please install it: `npm install -g quicktype`")
endif()

add_custom_command(
  OUTPUT node_qt.hpp
  COMMAND ${QUICKTYPE} -s schema -t node -l C++ --namespace wot_qt --no-boost
    --include-location global-include -S ${CMAKE_CURRENT_SOURCE_DIR}/../../src/link.json
    -o node_qt.hpp ${CMAKE_CURRENT_SOURCE_DIR}/../../src/node.json
  DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/../../src/link.json
    ${CMAKE_CURRENT_SOURCE_DIR}/../../src/node.json
  VERBATIM
)

file(GLOB filters RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} CONFIGURE_DEPENDS *.cpp)

foreach(testfile IN LISTS filters)
  add_executable(${testfile})
  target_sources(${testfile} PUBLIC ../main.cpp ${testfile} node_qt.hpp)
  target_compile_definitions(${testfile} PRIVATE "BOOST_TEST_DYN_LINK=1")
  target_include_directories(${testfile} PUBLIC ../../src ${CMAKE_CURRENT_BINARY_DIR} ${tomlplusplus_SOURCE_DIR} ${Boost_INCLUDE_DIR})
  target_link_libraries(${testfile} ${Boost_LIBRARIES} OpenSSL::SSL wotlib nlohmann_json::nlohmann_json)
  target_link_libraries(${testfile} ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})
  add_test(NAME ${testfile} COMMAND ${testfile})
endforeach()
