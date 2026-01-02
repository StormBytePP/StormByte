cmake_minimum_required(VERSION 3.28)
set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

project("StormByte C++ Library"
	VERSION 0.0.9999
	DESCRIPTION "StormByte is a comprehensive, cross-platform C++ library aimed at easing system programming, configuration management, logging, and database handling tasks. This library provides a unified API that abstracts away the complexities and inconsistencies of different platforms (Windows, Linux)."
	HOMEPAGE_URL "https://dev.stormbyte.org/StormByte"
)

include(cmake/optimizations.cmake)
include(cmake/asan.cmake)

add_subdirectory(doc)
add_subdirectory(thirdparty)
add_subdirectory(lib)
add_subdirectory(test)

include(cmake/outputflags.cmake)
include(cmake/install.cmake)