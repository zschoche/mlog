cmake_minimum_required (VERSION 2.8)
project(mlog)

#defines
add_definitions("-Wall")
add_definitions("-O3")
#add_definitions("-g")
add_definitions(-DMLOGDEBUG)
add_definitions(-DMLOGTRACE)

#add_definitions(-DMLOG_NO_LIB)

set(VERSION_MAJOR "0")
set(VERSION_MINOR "2")
set(VERSION_PATCH "0")

# boost
# you should define BOOST_ROOT first
find_package(Boost COMPONENTS system filesystem unit_test_framework regex log)
include_directories()
if(MSVC)
    # disable auto link on windows
    # to support use dynamic boost lib
    add_definitions(-DBOOST_ALL_NO_LIB)
endif()


#includes
include_directories(
    ${Boost_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}
)


#source directorys
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/libs mlog_src)

#librarys
FILE(GLOB mlog_public_incl ${CMAKE_CURRENT_SOURCE_DIR}/mlog/*.hpp)
FILE(GLOB mlog_private_incl ${CMAKE_CURRENT_SOURCE_DIR}/mlog/impl/*.hpp)

add_library(mlog ${mlog_src} ${mlog_incl})
set_target_properties(mlog PROPERTIES
    PUBLIC_HEADER  "${mlog_public_incl}"
    PRIVATE_HEADER "${mlog_private_incl}")

#programs
add_executable(test EXCLUDE_FROM_ALL test_src/test.cpp)
add_executable(benchmark EXCLUDE_FROM_ALL benchmark_src/benchmark.cpp)


if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    add_definitions("-std=c++11")
    message("using clang.")
    add_definitions("-fexceptions")
else()
    if(CMAKE_COMPILER_IS_GNUCXX)
        execute_process(COMMAND ${CMAKE_C_COMPILER} -dumpversion
                    OUTPUT_VARIABLE GCC_VERSION)
        if (GCC_VERSION VERSION_GREATER 4.7 OR GCC_VERSION VERSION_EQUAL 4.7)
                message("using gcc with -std=c++0x")
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x -pthread")
        else()
                message("using gcc with -std=gnu++0x")
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++0x")
        endif()
    elseif(MSVC)
        message("using msvc.")
    else()
        add_definitions("-std=c++11")
        message("using known compiler.")
    endif()

    set(TRAVIS "$ENV{TRAVIS}")
    if(TRAVIS)
        message("building on travis-ci")
       add_definitions(-DBOOST_TEST_DYN_LINK)
    else()
        message("using BOOST_TEST_DYN_LINK")
       add_definitions(-DBOOST_TEST_DYN_LINK)

    endif()
endif()

target_link_libraries(test mlog ${Boost_LIBRARIES})
target_link_libraries(benchmark mlog ${Boost_LIBRARIES} boost_log boost_thread)

install(
    TARGETS mlog
    RUNTIME DESTINATION bin COMPONENT runtime
    LIBRARY DESTINATION lib COMPONENT runtime
    ARCHIVE DESTINATION lib COMPONENT runtime
    PUBLIC_HEADER DESTINATION include/mlog COMPONENT development
    PRIVATE_HEADER DESTINATION include/mlog/impl COMPONENT development
)
#enable_testing()
#ADD_TEST(unittest test)

add_subdirectory(debian)
