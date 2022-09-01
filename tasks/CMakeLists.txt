

file(GLOB TASK_SRC 
    pole_balancing.cpp
)
add_executable(eSpinn.sim ${TASK_SRC})
target_link_libraries(eSpinn.sim eSpinn ${Boost_LIBRARIES})
