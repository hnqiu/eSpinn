

file(GLOB TEST_SRC 
    *.cpp 
)
add_executable(eSpinn.test ${TEST_SRC})
target_link_libraries(eSpinn.test eSpinn ${Boost_LIBRARIES})
