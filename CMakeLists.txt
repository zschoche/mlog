cmake_minimum_required (VERSION 2.6)
project(mlog)

ADD_DEFINITIONS("-std=c++0x")
ADD_DEFINITIONS("-stdlib=libc++")

set(CMAKE_CXX_FLAGS "-Wall -O3 -fexceptions")
#SET(CMAKE_C_LINK_FLAGS "-stdlib=libc++")
#set(CMAKE_CXX_FLAGS "-Wall -g -std=c++11 -fexceptions")

#includes
include_directories(.)
#include_directories(/Developer/usr/lib/c++/v1)
#defines
add_definitions(-DMLOGDEBUG)
add_definitions(-DMLOGTRACE)

#source directorys
aux_source_directory(./mlog mlog_src)
aux_source_directory(./test_src test_src)

#librarys
add_library(mlog ${mlog_src})

#tests
add_executable(test ${test_src})
target_link_libraries(test mlog)
target_link_libraries(test c++)
#target_link_libraries(test stdc++)
#target_link_libraries(test c++abi) 
target_link_libraries(test boost_thread)
target_link_libraries(test boost_system)
target_link_libraries(test boost_filesystem)
