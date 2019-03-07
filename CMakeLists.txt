#----------------------------------------------------------------------------
# Setup the project
cmake_minimum_required(VERSION 2.6 FATAL_ERROR)
project(MOS_test C CXX Fortran)
enable_language(Fortran)

execute_process(COMMAND "source ~/.bashrc")

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")

#set(Matlab_ROOT_DIR "/scratch/Matlab/R2017b/")
#find_package(Matlab REQUIRED MAT_LIBRARY)

set(Matlab_MEX_LIBRARY "${PROJECT_SOURCE_DIR}/src/matlab/bin/glnxa64/libmex.so")
set(Matlab_MX_LIBRARY "${PROJECT_SOURCE_DIR}/src/matlab/bin/glnxa64/libmx.so")
set(Matlab_ENG_LIBRARY "${PROJECT_SOURCE_DIR}/src/matlab/bin/glnxa64/libeng.so")
set(Matlab_MAT_LIBRARY "${PROJECT_SOURCE_DIR}/src/matlab/bin/glnxa64/libmat.so")
set(Matlab_LIBRARIES
  ${Matlab_MEX_LIBRARY} ${Matlab_MX_LIBRARY}
  ${Matlab_ENG_LIBRARY} ${Matlab_MAT_LIBRARY})

set(Matlab_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/src/matlab/extern/include")

#set(HDF5_USE_STATIC_LIBRARIES ON)
#find_package(HDF5 REQUIRED C CXX)
#set(HDF5_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/src/hdf5/include")

#message("\n")
#message(${HDF5_INCLUDE_DIRS})
#message("\n")
#message(${HDF5_LIBRARIES})
#message("\n")
message("\n")
message(${Matlab_INCLUDE_DIRS})
message("\n")
message(${Matlab_LIBRARIES})
message("\n")
#----------------------------------------------------------------------------

#----------------------------------------------------------------------------
# Find Geant4 package, activating all available UI and Vis drivers by default
# You can set WITH_GEANT4_UIVIS to OFF via the command line or ccmake/cmake-gui
# to build a batch mode only executable
#
option(WITH_GEANT4_UIVIS "Build example with Geant4 UI and Vis drivers" ON)
if (WITH_GEANT4_UIVIS)
    find_package(Geant4 REQUIRED gdml ui_all vis_all)
else ()
    find_package(Geant4 REQUIRED gdml)
endif ()

#----------------------------------------------------------------------------
# Setup Geant4 include directories and compile definitions
#
include(${Geant4_USE_FILE})

#----------------------------------------------------------------------------
# Locate sources and headers for this project
#
include_directories(${PROJECT_SOURCE_DIR}/build/)
include_directories(${PROJECT_SOURCE_DIR}/src/include
        ${Geant4_INCLUDE_DIR})

#include_directories(${HDF5_INCLUDE_DIRS})
include_directories(${Matlab_INCLUDE_DIRS})

file(GLOB sources ${PROJECT_SOURCE_DIR}/src/src/*.cc)
file(GLOB headers ${PROJECT_SOURCE_DIR}/src/include/*.hh)
file(GLOB sourcesf ${PROJECT_SOURCE_DIR}/src/src/*.f)
file(GLOB sourcesf2 ${PROJECT_SOURCE_DIR}/src/src/msis/*.for)
file(GLOB sourcesf90 ${PROJECT_SOURCE_DIR}/src/src/parma/*.f90)
file(GLOB sourcesc ${PROJECT_SOURCE_DIR}/src/src/*.c)
file(GLOB sourcesc2 ${PROJECT_SOURCE_DIR}/src/src/C_functions/*.c)
file(GLOB sourcesh ${PROJECT_SOURCE_DIR}/src/include/*.h)
file(GLOB main ${PROJECT_SOURCE_DIR}/src/mos_test.cc)
#file(GLOB hdf_headers ${HDF5_INCLUDE_DIRS}/*.h)
file(GLOB matlab_headers ${Matlab_INCLUDE_DIRS}/*.h)

#${hdf}
#----------------------------------------------------------------------------
# Add the executable, and link it to the Geant4 libraries
#
add_executable(mos_test ${main} ${matlab_headers} ${sourcesf} ${sourcesf2} ${sourcesf90} ${sourcesc} ${sourcesc2} ${sourcesh} ${sources} ${headers})
target_link_libraries(mos_test ${Geant4_LIBRARIES} ${Matlab_LIBRARIES})
#target_link_libraries(fegs_bgo ${cadmesh_LIBRARIES} )

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    # using GCC
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -mfpmath=sse -msse2 -ffp-contract=off")
    set(CMAKE_Fortran_FLAGS "${CMAKE_Fortran_FLAGS} -ffree-line-length-512")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Intel")
    # using Intel C++
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -fp-model=extended")
endif ()

#----------------------------------------------------------------------------
# Install the executable to 'bin' directory under CMAKE_INSTALL_PREFIX
#
install(TARGETS mos_test DESTINATION bin)

