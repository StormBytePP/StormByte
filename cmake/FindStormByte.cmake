include(FeatureSummary)

# FindStormByte.cmake - robust finder for StormByte and optional modules
# Specs:
# - Find main library `StormByte` and the include dir (via visibility.h)
# - Optional components: Buffer, Config, Crypto, Database, Logger, Multimedia, Network, System
# - Create imported targets `StormByte` and `StormByte-<Component>` and aliases `StormByte::<Component>`
# - Set *_FOUND and STORMBYTE_<Component>_LIBRARIES variables
# - Handle a small hardcoded transitive dependency map

# Locate visibility.h to determine include directory
find_path(STORMBYTE_INCLUDE_DIR
          NAMES visibility.h
          PATH_SUFFIXES StormByte
          PATHS ${CMAKE_PREFIX_PATH} /usr/include /usr/local/include
)

# Locate the main StormByte library
find_library(STORMBYTE_LIBRARY
             NAMES StormByte
             PATH_SUFFIXES lib lib64
             PATHS ${CMAKE_PREFIX_PATH} /usr/lib /usr/lib64 /usr/local/lib /usr/local/lib64
)

mark_as_advanced(STORMBYTE_LIBRARY STORMBYTE_INCLUDE_DIR)

# Hardcoded list of available components (add to this list to extend)
set(_available_components
    Buffer
    Config
    Crypto
    Database
    Logger
    Multimedia
    Network
    System
)

# Hardcoded transitive dependency map (component -> list of components it depends on)
# Example: Crypto depends on Buffer, Network depends on Buffer
set(_component_dependencies_Buffer Logger)
set(_component_dependencies_Crypto Buffer)
set(_component_dependencies_Network Buffer)

# Parse requested components from the calling project.
# Support two usage styles:
# 1) The caller sets `StormByte_FIND_COMPONENTS` before calling `find_package`.
# 2) The caller invokes `find_package(StormByte REQUIRED Module1 Module2 ...)`.
if (DEFINED StormByte_FIND_COMPONENTS)
    set(_requested_components ${StormByte_FIND_COMPONENTS})
else()
    # Collect ARGN (arguments passed to this find module) and filter out common keywords
    set(_requested_components)
    if (ARGN)
        foreach(_arg IN LISTS ARGN)
            string(TOUPPER "${_arg}" _ARG_UP)
            # Skip standard find_package keywords
            if (_ARG_UP STREQUAL "REQUIRED" OR _ARG_UP STREQUAL "QUIET" OR _ARG_UP STREQUAL "COMPONENTS" OR _ARG_UP STREQUAL "EXACT" OR _ARG_UP STREQUAL "NO_MODULE")
                continue()
            endif()
            list(APPEND _requested_components ${_arg})
        endforeach()
    endif()
endif()

# Validate requested components
foreach(_c IN LISTS _requested_components)
    if (NOT _c IN_LIST _available_components)
        message(FATAL_ERROR "Requested component '$_c' is not valid. Available components: ${_available_components}.")
    endif()
endforeach()

# Prepare containers for results
unset(_found_components)
unset(_missing_components)

# First: find each requested component's library and create imported targets for found ones
foreach(component IN LISTS _requested_components)
    string(TOUPPER ${component} _UPCOMP)
    find_library(STORMBYTE_${component}_LIBRARY
                 NAMES StormByte-${component}
                 PATH_SUFFIXES lib lib64
                 PATHS ${CMAKE_PREFIX_PATH} /usr/lib /usr/lib64 /usr/local/lib /usr/local/lib64
    )

    if (STORMBYTE_${component}_LIBRARY)
        set(${component}_FOUND TRUE)
        set(STORMBYTE_${component}_LIBRARIES ${STORMBYTE_${component}_LIBRARY})

        # Create an imported target so callers can link to it
        add_library(StormByte-${component} UNKNOWN IMPORTED GLOBAL)
        add_library(StormByte::${component} ALIAS StormByte-${component})

        # Set basic properties (location and include dir)
        set_target_properties(StormByte-${component} PROPERTIES
            IMPORTED_LOCATION ${STORMBYTE_${component}_LIBRARIES}
            INTERFACE_INCLUDE_DIRECTORIES ${STORMBYTE_INCLUDE_DIR}
            INTERFACE_LINK_LIBRARIES "StormByte"
        )

        list(APPEND _found_components ${component})
    else()
        set(${component}_FOUND FALSE)
        list(APPEND _missing_components ${component})
    endif()

    mark_as_advanced(STORMBYTE_${component}_LIBRARY)
endforeach()

# Now that all requested components have been considered, update INTERFACE_LINK_LIBRARIES
# for each found component to include any transitive dependencies that were also found.
foreach(component IN LISTS _found_components)
    set(_link_libraries StormByte)
    if (DEFINED _component_dependencies_${component})
        foreach(dep IN LISTS _component_dependencies_${component})
            # Only add dependency link if the dependency was found/created
            if (dep IN_LIST _found_components)
                list(APPEND _link_libraries StormByte-${dep})
            endif()
        endforeach()
    endif()
    set_target_properties(StormByte-${component} PROPERTIES
        INTERFACE_LINK_LIBRARIES "${_link_libraries}"
    )
endforeach()

# Create main StormByte imported target if the library was located
if (STORMBYTE_LIBRARY)
    add_library(StormByte UNKNOWN IMPORTED GLOBAL)
    set_target_properties(StormByte PROPERTIES
        IMPORTED_LOCATION ${STORMBYTE_LIBRARY}
        INTERFACE_INCLUDE_DIRECTORIES ${STORMBYTE_INCLUDE_DIR}
    )
    if (MSVC)
        target_compile_definitions(StormByte INTERFACE UNICODE NOMINMAX)
    endif()
    set(STORMBYTE_FOUND TRUE)
else()
    set(STORMBYTE_FOUND FALSE)
endif()

# Report status or errors
set(_error_message "")
if (NOT STORMBYTE_FOUND)
    set(_error_message "StormByte library not found.")
endif()
if (_missing_components)
    string(JOIN ", " _missing_components_text ${_missing_components})
    if (_error_message)
        set(_error_message "${_error_message}\nSome requested components were not found: ${_missing_components_text}.")
    else()
        set(_error_message "Some requested components were not found: ${_missing_components_text}.")
    endif()
endif()

if (_error_message)
    message(FATAL_ERROR "${_error_message}")
else()
    if (_found_components)
        string(JOIN ", " _found_components_text ${_found_components})
        message(STATUS "StormByte library found. Enabled components: ${_found_components_text}.")
    elseif (STORMBYTE_FOUND)
        message(STATUS "StormByte library found.")
    endif()
endif()

mark_as_advanced(STORMBYTE_INCLUDE_DIR STORMBYTE_LIBRARY)
