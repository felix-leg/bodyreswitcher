# Test configuration

#function for making tests
function(brs_test TEST_NAME EXTRA_FILES)
add_executable(${TEST_NAME}_test EXCLUDE_FROM_ALL test_${TEST_NAME}.cpp ${EXTRA_FILES})
target_include_directories(${TEST_NAME}_test PUBLIC "${CMAKE_SOURCE_DIR}/src" "${CMAKE_SOURCE_DIR}/vendor")
target_include_directories(${TEST_NAME}_test PRIVATE ${Boost_INCLUDE_DIRS})
target_link_libraries(${TEST_NAME}_test SDL2::Main SDL2::Mixer SDL2::Image SDL2::TTF SDL2::GFX)
target_link_libraries(${TEST_NAME}_test ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})
add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME}_test)

message("Added ${TEST_NAME} test")
endfunction()

#tests list
brs_test( animation "${CMAKE_SOURCE_DIR}/src/animation.cpp")
brs_test( rectangle "${CMAKE_SOURCE_DIR}/src/rect.cpp")
brs_test( ui "${CMAKE_SOURCE_DIR}/src/rect.cpp;${CMAKE_SOURCE_DIR}/src/ui.cpp")
brs_test( filesystem "${CMAKE_SOURCE_DIR}/src/filesystem.cpp")
brs_test( multimedia_base "${CMAKE_SOURCE_DIR}/src/multimedia/base.cpp")
brs_test( vector "${CMAKE_SOURCE_DIR}/src/game_math/vector.cpp")
brs_test( graph "" )
brs_test( person_graph "${CMAKE_SOURCE_DIR}/src/logic/person_graph.cpp;${CMAKE_SOURCE_DIR}/src/mls.cpp;${CMAKE_SOURCE_DIR}/src/filesystem.cpp")
