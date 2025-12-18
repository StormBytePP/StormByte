include(GNUInstallDirs)

# Sources
file(GLOB_RECURSE STORMBYTE_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_LIST_DIR}/*.cxx")

# Library
add_library(StormByte SHARED ${STORMBYTE_SOURCES})
set_target_properties(StormByte PROPERTIES
	LINKER_LANGUAGE CXX
	SOVERSION		${CMAKE_PROJECT_VERSION}
	VERSION 		${CMAKE_PROJECT_VERSION}
)
set_target_properties(StormByte PROPERTIES
  	POSITION_INDEPENDENT_CODE        ON
  	CXX_VISIBILITY_PRESET            hidden
  	VISIBILITY_INLINES_HIDDEN        ON
)

# Win dependencies
if(WIN32)
	target_link_libraries(StormByte PRIVATE Bcrypt.lib)
endif()

# Compile options
if(MSVC)
	target_compile_options(StormByte PRIVATE /EHsc)
	target_compile_options(StormByte PRIVATE $<$<CONFIG:Release>:/O2>)
	target_compile_options(StormByte PRIVATE $<$<CONFIG:Debug>:/Od>)
	target_compile_definitions(StormByte PUBLIC UNICODE NOMINMAX) # Create problems if disabled
	target_link_options(StormByte PRIVATE $<$<CONFIG:Release>:/LTCG /GL>)
else()
	set(CMAKE_CXX_FLAGS_DEBUG "-pipe -g -ggdb -Wall -Wextra -Wnon-virtual-dtor -pedantic -pedantic-errors -O0")
endif()

# Include directories
target_include_directories(StormByte
	SYSTEM BEFORE PUBLIC "${CMAKE_CURRENT_LIST_DIR}/public"
)

