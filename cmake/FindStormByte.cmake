# FindStormByte.cmake
include(FeatureSummary)

# Locate visibility.h to find the include directory
find_path(STORMBYTE_INCLUDE_DIR
          NAMES visibility.h
          PATH_SUFFIXES StormByte
          PATHS ${CMAKE_PREFIX_PATH} /usr/include /usr/local/include
)

# Locate the main library
find_library(STORMBYTE_LIBRARY
             NAMES StormByte
             PATH_SUFFIXES lib lib64
             PATHS ${CMAKE_PREFIX_PATH} /usr/lib /usr/lib64 /usr/local/lib /usr/local/lib64
)

# Mark internal variables as advanced
mark_as_advanced(STORMBYTE_LIBRARY)

# List of all known components
set(_available_components Buffer Config Crypto Database Logger Multimedia Network System)

# Transitive dependencies
# Define which components depend on other components
# Crypto depends on Buffer
# Network depends on Buffer
set(_component_dependencies_Buffer Logger)
set(_component_dependencies_Crypto Buffer)
set(_component_dependencies_Network Buffer)

# Use StormByte_FIND_COMPONENTS to determine requested components
if (DEFINED StormByte_FIND_COMPONENTS)
    set(_requested_components "${StormByte_FIND_COMPONENTS}")
else()
    set(_requested_components "")
endif()

# Check for invalid components
foreach(component IN LISTS _requested_components)
    if (NOT component IN_LIST _available_components)
        message(FATAL_ERROR "Requested component '${component}' is not a valid component. Available components: ${_available_components}.")
    endif()
endforeach()

# Variables to track found and missing components
unset(_found_components)
unset(_missing_components)

# Iterate through requested components only
foreach(component IN LISTS _requested_components)
    find_library(STORMBYTE_${component}_LIBRARY
                 NAMES StormByte-${component}
                 PATH_SUFFIXES lib lib64
                 PATHS ${CMAKE_PREFIX_PATH} /usr/lib /usr/lib64 /usr/local/lib /usr/local/lib64
    )
    
    if (STORMBYTE_${component}_LIBRARY)
        set(${component}_FOUND TRUE)
        set(STORMBYTE_${component}_LIBRARIES ${STORMBYTE_${component}_LIBRARY})
        add_library(StormByte-${component} UNKNOWN IMPORTED GLOBAL)
        add_library(StormByte::${component} ALIAS StormByte-${component})
        
        # Build the list of link libraries (always include StormByte)
        set(_link_libraries StormByte)
        
        # Add transitive dependencies if defined
        if (DEFINED _component_dependencies_${component})
            foreach(dep IN LISTS _component_dependencies_${component})
                if (TARGET StormByte-${dep})
                    list(APPEND _link_libraries StormByte-${dep})
                endif()
            endforeach()
        endif()
        
        set_target_properties(StormByte-${component} PROPERTIES
            IMPORTED_LOCATION ${STORMBYTE_${component}_LIBRARIES}
            INTERFACE_INCLUDE_DIRECTORIES ${STORMBYTE_INCLUDE_DIR}
            INTERFACE_LINK_LIBRARIES "${_link_libraries}"
        )
        list(APPEND _found_components ${component})
    else()
        set(${component}_FOUND FALSE)
        list(APPEND _missing_components ${component})
    endif()

    # Mark internal component variables as advanced
    mark_as_advanced(STORMBYTE_${component}_LIBRARY)
endforeach()

# Define the main library target
if (STORMBYTE_LIBRARY)
    add_library(StormByte UNKNOWN IMPORTED GLOBAL)
    set_target_properties(StormByte PROPERTIES
        IMPORTED_LOCATION ${STORMBYTE_LIBRARY}
        INTERFACE_INCLUDE_DIRECTORIES ${STORMBYTE_INCLUDE_DIR}
    )
	if (MSVC)
		target_compile_definitions(StormByte INTERFACE UNICODE NOMINMAX)
	endif()
else()
    set(STORMBYTE_FOUND FALSE)
endif()

# Generate error or status messages
set(_error_message "")
if (NOT STORMBYTE_LIBRARY)
    set(_error_message "StormByte library not found.")
endif()
if (_missing_components)
    string(JOIN ", " _missing_components_text ${_missing_components})
    set(_error_message "${_error_message}\nSome requested components were not found: ${_missing_components_text}.")
endif()

if (_error_message)
    message(FATAL_ERROR "${_error_message}")
else()
    if (_found_components)
        string(JOIN ", " _found_components_text ${_found_components})
        message(STATUS "StormByte library found. Enabled components: ${_found_components_text}.")
    else()
        message(STATUS "StormByte library found.")
    endif()
endif()

mark_as_advanced(STORMBYTE_INCLUDE_DIR STORMBYTE_LIBRARIES)
