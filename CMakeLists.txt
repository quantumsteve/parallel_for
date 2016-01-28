project(parallel_for)
cmake_minimum_required(VERSION 3.1)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

set(CMAKE_CXX_STANDARD 14)
set(CXX_STANDARD_REQUIRED 11)

IF(NOT CMAKE_BUILD_TYPE)
  SET(CMAKE_BUILD_TYPE Release CACHE STRING
      "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel."
      FORCE)
ENDIF(NOT CMAKE_BUILD_TYPE)

find_package(Boost)
find_package(TBB)
find_package ( OpenMP )

if(NOT DEFINED MANTID_PARALLELISM_BACKEND)
  if(OPENMP_FOUND)
    set(MANTID_PARALLELISM_BACKEND "OPENMP" CACHE STRING "Choose the parallelism backend.")
  elseif(TBB_FOUND)
    set(MANTID_PARALLELISM_BACKEND "TBB" CACHE STRING "Choose the parallelism backend.")
  else()
    set(MANTID_PARALLELISM_BACKEND "SERIAL" CACHE STRING "Choose the parallelism backend.")
  endif()
endif()

message(STATUS "Using ${MANTID_PARALLELISM_BACKEND} execution")

if(${MANTID_PARALLELISM_BACKEND} STREQUAL "OPENMP")
  add_definitions(-DUSE_OPENMP)
elseif(${MANTID_PARALLELISM_BACKEND} STREQUAL "TBB")
  add_definitions(-DUSE_TBB)
endif()

set_property(CACHE MANTID_PARALLELISM_BACKEND PROPERTY STRINGS "TBB;OPENMP;SERIAL")

add_executable(MatrixMultiply MatrixMultiply.cpp parallel.h)
target_include_directories(MatrixMultiply PUBLIC ${Boost_INCLUDE_DIR})
if(${MANTID_PARALLELISM_BACKEND} STREQUAL "TBB")
  target_include_directories(MatrixMultiply PUBLIC ${TBB_INCLUDE_DIRS})
  target_link_libraries(MatrixMultiply ${TBB_LIBRARIES})
endif()

add_executable(MutexedCout MutexedCout.cpp parallel.h)
target_include_directories(MutexedCout PUBLIC ${Boost_INCLUDE_DIR})
if(${MANTID_PARALLELISM_BACKEND} STREQUAL "TBB")
  target_include_directories(MutexedCout PUBLIC ${TBB_INCLUDE_DIRS})
  target_link_libraries(MutexedCout ${TBB_LIBRARIES})
endif()

add_executable(ThreadPool ThreadPool.cpp parallel.h)
target_include_directories(ThreadPool PUBLIC ${Boost_INCLUDE_DIR})
if(${MANTID_PARALLELISM_BACKEND} STREQUAL "TBB")
  target_include_directories(ThreadPool PUBLIC ${TBB_INCLUDE_DIRS})
  target_link_libraries(ThreadPool ${TBB_LIBRARIES})
endif()

add_executable(Macros Macros.cpp parallel.h)
target_include_directories(Macros PUBLIC ${Boost_INCLUDE_DIR})
if(${MANTID_PARALLELISM_BACKEND} STREQUAL "TBB")
  target_include_directories(Macros PUBLIC ${TBB_INCLUDE_DIRS})
  target_link_libraries(Macros ${TBB_LIBRARIES})
endif()

