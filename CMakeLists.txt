cmake_minimum_required(VERSION 3.5)

project(espinn)

# find Boost packages, min_version 1.58
find_package(Boost 1.58 REQUIRED COMPONENTS serialization)
# find Pybind11
find_package(pybind11 REQUIRED)

include_directories( ${Boost_INCLUDE_DIRS} )
include_directories(
    src
)

# set CXX flag
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
# add_compile_options(-std=c++11)

# set target Python version
set(PYTHON_EXECUTABLE="$HOME/anaconda3/bin/python")

set(eSpinn_COMPILE_FLAGS "-ffriend-injection")
# set(eSpinn_COMPILE_FLAGS "${eSpinn_COMPILE_FLAGS} -Wall")
if(ESPINN_VERBOSE)
    set(eSpinn_COMPILE_FLAGS "${eSpinn_COMPILE_FLAGS} -DESPINN_VERBOSE")
endif()
if(NDEBUG)
    set(eSpinn_COMPILE_FLAGS "${eSpinn_COMPILE_FLAGS} -DNDEBUG")
endif()

# to resolve relocation error when 'mex'ing matalb s-fn against lib 'eSpinn',
# compile this library with the 'mcmodel' flag
# however, the 'mcmodel=large' flag is in conflict with PIC flag and causes
# 'unresolvable R_X86_64_PLTOFF64 relocation` error while building 'PyeSpinn'
# therefore, if package 'eSpinn' is built for use in MATLAB
# 'PyeSpinn' is obmitted (see src/CMakeLists.txt)
if(USE_IN_MATLAB)
    set(eSpinn_COMPILE_FLAGS "${eSpinn_COMPILE_FLAGS} -DNDEBUG -mcmodel=large")
else()
    # compile with -fPIC to make a shared pybind module
    set(CMAKE_POSITION_INDEPENDENT_CODE ON)
endif()

set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} ${eSpinn_COMPILE_FLAGS}")

add_subdirectory(src)
# add_subdirectory(test)
# add_subdirectory(tasks)

# clear the cache var
unset(USE_IN_MATLAB CACHE)
unset(ESPINN_VERBOSE CACHE)
unset(NDEBUG CACHE)
