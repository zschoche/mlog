cmake_minimum_required (VERSION 2.6)
project(mlog)

add_definitions("-std=c++0x")
add_definitions("-stdlib=libc++")
add_definitions("-Wall")
add_definitions("-O3")
add_definitions("-fexceptions")
#add_definitions("-g")

#includes
include_directories(.)

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
target_link_libraries(test boost_thread)
target_link_libraries(test boost_system)
target_link_libraries(test boost_filesystem)
