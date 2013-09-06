cmake_minimum_required (VERSION 2.8)
project(mlog)

#defines
add_definitions("-Wall")
add_definitions("-O3")
#add_definitions("-g")

add_definitions(-DMLOGDEBUG)
add_definitions(-DMLOGTRACE)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")	
#add_definitions(-DMLOG_NO_LIB)

# boost
# you should define BOOST_ROOT first
find_package(Boost COMPONENTS system filesystem unit_test_framework)
include_directories(Boost_INCLUDE_DIR ${Boost_INCLUDE_DIRS})


#includes
include_directories(.)


#source directorys
aux_source_directory(./libs mlog_src)

#librarys
add_library(mlog ${mlog_src})

#programs
add_executable(test EXCLUDE_FROM_ALL test_src/test.cpp)


if (CMAKE_CXX_COMPILER MATCHES ".*clang")
 	message("using clang.")
        add_definitions("-stdlib=libc++")
	add_definitions("-fexceptions")
        target_link_libraries(test c++)
	#add_definitions(-DBOOST_TEST_DYN_LINK) 
else()
 	message("using gcc.")
	add_definitions(-DBOOST_TEST_DYN_LINK)
endif()

target_link_libraries(test mlog ${Boost_LIBRARIES})

install(
    TARGETS mlog
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    )
install(DIRECTORY mlog DESTINATION include)
#enable_testing()
#ADD_TEST(unittest test)
