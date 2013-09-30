cmake_minimum_required (VERSION 2.8)
project(mlog)

#defines
add_definitions("-Wall")
#add_definitions("-O3")
add_definitions("-g")

add_definitions(-DMLOGDEBUG)
add_definitions(-DMLOGTRACE)

#add_definitions(-DMLOG_NO_LIB)

# boost
# you should define BOOST_ROOT first
find_package(Boost COMPONENTS system filesystem unit_test_framework random)
include_directories(Boost_INCLUDE_DIR ${Boost_INCLUDE_DIRS})
if(MSVC)
    # disable auto link on windows
    # to support use dynamic boost lib
    add_definitions(-DBOOST_ALL_NO_LIB)
endif()


#includes
include_directories(.)


#source directorys
aux_source_directory(./libs mlog_src)

#librarys
add_library(mlog ${mlog_src})

#programs
add_executable(test EXCLUDE_FROM_ALL test_src/test.cpp)


if (CMAKE_CXX_COMPILER MATCHES ".*clang")
    	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")	
 	message("using clang.")
        add_definitions("-stdlib=libc++")
	add_definitions("-fexceptions")
        target_link_libraries(test c++)
	#add_definitions(-DBOOST_TEST_DYN_LINK) 
else()
    if(CMAKE_COMPILER_IS_GNUCXX)
	execute_process(COMMAND ${CMAKE_C_COMPILER} -dumpversion
                OUTPUT_VARIABLE GCC_VERSION)
	if (GCC_VERSION VERSION_GREATER 4.7 OR GCC_VERSION VERSION_EQUAL 4.7)
        	message("using gcc with -std=c++11")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")	
	else()
        	message("using gcc with -std=gnu++0x")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++0x")	
	endif()
    elseif(MSVC)
        message("using msvc.")
    else()
        message("using known compiler.")
    endif()
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
enable_testing()
ADD_TEST(unittest test)
