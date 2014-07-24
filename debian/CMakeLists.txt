cmake_minimum_required (VERSION 2.8)

set (CPACK_DEB_COMPONENT_INSTALL 1)

set (CPACK_CMAKE_GENERATOR "Unix Makefiles")
set (CPACK_PACKAGE_NAME "mlog")
set (CPACK_SET_DESTDIR "on")
set (CPACK_PACKAGING_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX})
set (CPACK_GENERATOR "DEB")

set (CPACK_DEBIAN_PACKAGE_NAME mlog)

set (CPACK_PACKAGE_DESCRIPTION "Comfortable lightweight C++ logging library -- cross-platform, C++11")
set (CPACK_PACKAGE_VENDOR "Philipp Zschoche")
set (CPACK_PACKAGE_CONTACT "github@zschoche.org")
set (CPACK_PACKAGE_VERSION_MAJOR ${VERSION_MAJOR})
set (CPACK_PACKAGE_VERSION_MINOR ${VERSION_MINOR})
set (CPACK_PACKAGE_VERSION_PATCH ${VERSION_PATCH})

configure_file(MlogConfig.cmake.in  "${CMAKE_CURRENT_BINARY_DIR}/MlogConfig.cmake"  @ONLY)
configure_file(Runtime.cpack.in     "${CMAKE_CURRENT_BINARY_DIR}/Runtime.cpack"     @ONLY)
configure_file(Development.cpack.in "${CMAKE_CURRENT_BINARY_DIR}/Development.cpack" @ONLY)

install(FILES "${CMAKE_CURRENT_BINARY_DIR}/MlogConfig.cmake"
  DESTINATION lib/cmake COMPONENT development)

include (CPack)
