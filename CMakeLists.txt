cmake_minimum_required (VERSION 2.8)
project(mlog)

#defines
add_definitions("-Wall")
add_definitions("-O3")
#add_definitions("-g")

add_definitions(-DMLOGDEBUG)
add_definitions(-DMLOGTRACE)
#add_definitions(-DMLOG_NO_LIB)


#includes
include_directories(.)


#source directorys
aux_source_directory(./libs mlog_src)


#librarys
add_library(mlog ${mlog_src})

#programs
add_executable(test test_src/test.cpp)

#if ("${CMAKE_CXX_COMPILER} ${CMAKE_CXX_COMPILER_ARG1}" MATCHES ".*clang")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11")	
       # add_definitions("-std=c++11")
        add_definitions("-stdlib=libc++")
	add_definitions("-fexceptions")
        target_link_libraries(test c++)
#else()

#endif()

target_link_libraries(test mlog boost_system boost_filesystem boost_unit_test_framework)

