include(GNUInstallDirs)

file(GLOB_RECURSE STORMBYTE_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_LIST_DIR}/*.cxx")

add_library(StormByte SHARED ${STORMBYTE_SOURCES})
set_target_properties(StormByte PROPERTIES
	LINKER_LANGUAGE CXX
	SOVERSION ${CMAKE_PROJECT_VERSION}
	VERSION ${CMAKE_PROJECT_VERSION}
	POSITION_INDEPENDENT_CODE ON
	CXX_VISIBILITY_PRESET hidden
	VISIBILITY_INLINES_HIDDEN ON
)

if(WIN32)
	target_link_libraries(StormByte PRIVATE Bcrypt.lib)
endif()

if(MSVC)
	# /EHsc: optional; often already the default for CXX with MSVC/clang-cl
	target_compile_options(StormByte PRIVATE
		/EHsc
		$<$<CONFIG:Release>:/O2>
		$<$<CONFIG:Debug>:/Od>
	)
	target_compile_definitions(StormByte PUBLIC UNICODE NOMINMAX)
else()
	# Prefer target_ over mutating global CMAKE_CXX_FLAGS_DEBUG
	target_compile_options(StormByte PRIVATE
		$<$<CONFIG:Debug>:-pipe -g -ggdb -Wall -Wextra -Wnon-virtual-dtor -pedantic -pedantic-errors -O0>
	)
endif()

target_include_directories(StormByte
	SYSTEM BEFORE PUBLIC "${CMAKE_CURRENT_LIST_DIR}/public"
)
