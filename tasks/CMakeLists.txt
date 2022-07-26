

file(GLOB TASK_SRC 
    *.cpp 
)
add_executable(eSpinn.sim ${TASK_SRC})
target_link_libraries(eSpinn.sim eSpinn ${Boost_LIBRARIES})
