cmake_minimum_required (VERSION 2.8)
project(mlog)

add_definitions("-Wall")
add_definitions("-O3")
#add_definitions("-g")


if ("${CMAKE_CXX_COMPILER} ${CMAKE_CXX_COMPILER_ARG1}" MATCHES ".*clang")
        add_definitions("-std=c++11")
        add_definitions("-stdlib=libc++")
	add_definitions("-fexceptions")
        target_link_libraries(test c++)
else()

endif()






#includes
include_directories(.)

#defines
add_definitions(-DMLOGDEBUG)
add_definitions(-DMLOGTRACE)
#add_definitions(-DMLOG_NO_LIB)

#source directorys
aux_source_directory(./libs mlog_src)

#librarys
add_library(mlog ${mlog_src})

#tests
add_executable(test test_src/test.cpp)
target_link_libraries(test mlog boost_system boost_filesystem boost_unit_test_framework)

