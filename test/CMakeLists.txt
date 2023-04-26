add_custom_command(
  OUTPUT node_qt.hpp
  COMMAND ${QUICKTYPE} -s schema -t node -l C++ --namespace wot_qt --no-boost
    --include-location global-include -S ${CMAKE_CURRENT_SOURCE_DIR}/../src/link.json
    -o node_qt.hpp ${CMAKE_CURRENT_SOURCE_DIR}/../src/node.json
  DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/link.json
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/node.json
  VERBATIM
)

file(GLOB tests RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} CONFIGURE_DEPENDS *.cpp)
list(REMOVE_ITEM tests main.cpp)

# Copy node1s.toml to build dir
configure_file(node1s.toml node1s.toml COPYONLY)

foreach(testfile IN LISTS tests)
  add_executable(${testfile})
  target_sources(${testfile} PUBLIC main.cpp ${testfile} node_qt.hpp)
  target_compile_definitions(${testfile} PRIVATE "BOOST_TEST_DYN_LINK=1")
  target_include_directories(${testfile} PUBLIC ../src ${CMAKE_CURRENT_BINARY_DIR} ${tomlplusplus_SOURCE_DIR} ${Boost_INCLUDE_DIR})
  target_link_libraries(${testfile} ${Boost_LIBRARIES} OpenSSL::SSL wotlib nlohmann_json::nlohmann_json)
  target_link_libraries(${testfile} ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})
  add_test(NAME ${testfile} COMMAND ${testfile})
endforeach()

add_subdirectory(filters)
add_subdirectory(commands)
