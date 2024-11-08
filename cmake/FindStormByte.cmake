# Look for the necessary header
find_path(StormByte_INCLUDE_DIR NAMES StormByte/StormByte.h)
mark_as_advanced(StormByte_INCLUDE_DIR)

# Look for the necessary library
find_library(StormByte_LIBRARY NAMES StormByte)
mark_as_advanced(StormByte_LIBRARY)

# Extract version information from the header file
if(StormByte_INCLUDE_DIR)
    file(STRINGS ${StormByte_INCLUDE_DIR}/StormByte/StormByte.h _ver_line
         REGEX "^#define STORMBYTE_VERSION *\"[0-9]+\\.[0-9]+\\.[0-9]+\""
         LIMIT_COUNT 1)
    string(REGEX MATCH "[0-9]+\\.[0-9]+\\.[0-9]+"
           StormByte_VERSION "${_ver_line}")
	unset(_ver_line)
	file(STRINGS ${StormByte_INCLUDE_DIR}/StormByte/Features.h _sqlite_feature
		   REGEX "^#define STORMBYTE_SQLITE *ON|OFF"
		   LIMIT_COUNT 1)
	string(REGEX MATCH "ON|OFF"  StormByte_SQLITE3_FEATURE "${_sqlite_feature}")
	unset(_sqlite_feature)
	set(StormByte_FOUND TRUE)
	set(StormByte_INCLUDE_DIRS ${StormByte_INCLUDE_DIR})
	set(StormByte_LIBRARIES ${StormByte_LIBRARY})
	if (StormByte_SQLITE3_FEATURE)
		# In order not to force changing rpath for linking we say only "sqlite3"
		# instead of the full library path
		list(APPEND StormByte_LIBRARIES sqlite3)
	endif()
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(StormByte
    REQUIRED_VARS StormByte_INCLUDE_DIR StormByte_LIBRARY StormByte_LIBRARIES
    VERSION_VAR StormByte_VERSION
    HANDLE_COMPONENTS
)

# Create the imported target
if(StormByte_FOUND)
    if(NOT TARGET StormByte)
        add_library(StormByte UNKNOWN IMPORTED GLOBAL)
        set_target_properties(StormByte PROPERTIES
			IMPORTED_LOCATION 				"${StormByte_LIBRARY}"
			INTERFACE_INCLUDE_DIRECTORIES 	"${StormByte_INCLUDE_DIRS}"
			INTERFACE_LINK_LIBRARIES      	"${StormByte_LIBRARIES}"
		)
		if (StormByte_SQLITE3_FEATURE)
			target_compile_definitions(StormByte INTERFACE STORMBYTE_ENABLE_SQLITE)
		endif()
    endif()
endif()
