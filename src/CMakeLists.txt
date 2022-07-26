
## build eSpinn as STATIC lib
# add sources
file(GLOB SRC_LIST 
    Models/*.cpp 
    Learning/*.cpp 
    Utilities/*.cpp 
    Plants/*.cpp
)
# build lib
add_library(eSpinn STATIC ${SRC_LIST})
target_link_libraries(eSpinn ${Boost_LIBRARIES})


# set message color
string(ASCII 27 ESC)
set(YELLOW "${ESC}[33m")
set(ColorReset "${ESC}[m")

# if package 'eSpinn' is built for use in MATLAB, 'PyeSpinn' is obmitted
if(USE_IN_MATLAB)
    message(STATUS "${YELLOW}Building package eSpinn "
        "in large mcmodel for use in MATLAB${ColorReset}")
else()
    ## build PyeSpinn as MODULE lib
    # set Pybind CXX option
    set(PYBIND11_CPP_STANDARD -std=c++11)
    # build lib
    pybind11_add_module(PyeSpinn PyModule/PybindWrapper.cpp )
    target_link_libraries(PyeSpinn PRIVATE eSpinn)
    message(STATUS "${YELLOW}Building package eSpinn & PyeSpinn${ColorReset}")
endif(USE_IN_MATLAB)

# clear the cache var
unset(YELLOW CACHE)
unset(ColorReset CACHE)
