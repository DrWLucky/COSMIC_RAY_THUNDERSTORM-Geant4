#----------------------------------------------------------------------------
# Setup the project
cmake_minimum_required(VERSION 2.6 FATAL_ERROR)
project(MOS_test C CXX Fortran)
enable_language(Fortran)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/src/cmake/modules/) # path to custom .cmake files to find packages (e.g. matio)

execute_process(COMMAND "source ~/.bashrc")

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")

#-----------------MATLAB-----------------------------------------------------
#set(Matlab_ROOT_DIR "/scratch/Matlab/R2017b/")
#find_package(Matlab REQUIRED MAT_LIBRARY)
#file(GLOB matlab_headers ${Matlab_INCLUDE_DIRS}/*.h)
#${Matlab_LIBRARIES}
#----------------------------------------------------------------------------

#-----------------HDF5------------------------------------------------------
find_package(HDF5 REQUIRED C) # call to the .cmake script located at ${CMAKE_SOURCE_DIR}/src/cmake/modules/
file(GLOB hdf5_headers ${HDF5_INCLUDE_DIRS}/*.h)

#-----------------MATIO-----------------------------------------------------
# to be able to write matlab output file .mat
find_package(MATIO REQUIRED) # call to the .cmake script located at ${CMAKE_SOURCE_DIR}/src/cmake/modules/
file(GLOB matio_headers ${MATIO_INCLUDE_DIR}/*.h)

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
add_executable(mos_test ${main} ${hdf5_headers} ${matio_headers} ${sourcesf} ${sourcesf2} ${sourcesf90} ${sourcesc} ${sourcesc2} ${sourcesh} ${sources} ${headers})
target_link_libraries(mos_test ${Geant4_LIBRARIES} ${HDF5_LIBRARIES} ${MATIO_LIBRARY})
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

