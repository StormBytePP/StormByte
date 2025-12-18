include(FeatureSummary)

# FindStormByte.cmake - finder for installed StormByte core and optional modules
# Usage:
#   find_package(StormByte REQUIRED)
#   find_package(StormByte REQUIRED Buffer Logger)
#
# Behavior:
# - Locates the core StormByte library and include directory (via visibility.h).
# - For requested modules, verifies presence by checking for a module visibility header
#   under the discovered include tree and/or by locating a library named StormByte-<Module>.
# - Creates imported targets: `StormByte` (core) and `StormByte-<Module>` plus alias
#   `StormByte::<Module>` for convenience.
# - Sets variables: STORMBYTE_FOUND, STORMBYTE_INCLUDE_DIR, STORMBYTE_LIBRARY,
#   <Module>_FOUND and STORMBYTE_<Module>_LIBRARIES.
# - Honors a hardcoded (but editable) list of available components and a simple
#   transitive dependency map used to populate INTERFACE_LINK_LIBRARIES.

# -----------------------------
# User-editable lists
# -----------------------------
# List of components that this finder understands. Edit to add/remove modules.
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

# Simple map of transitive link dependencies: component -> space-separated list
# Example: Crypto depends on Buffer
set(_component_dependencies_Buffer "Logger")
set(_component_dependencies_Crypto "Buffer")
set(_component_dependencies_Network "Buffer")

# -----------------------------
# Helper: normalize requested components from find_package arguments or variable
# -----------------------------
if (DEFINED StormByte_FIND_COMPONENTS)
    set(_requested_components ${StormByte_FIND_COMPONENTS})
else()
    set(_requested_components)
    if (ARGN)
        foreach(_arg IN LISTS ARGN)
            string(TOUPPER "${_arg}" _ARG_UP)
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

# -----------------------------
# Find core include dir (visibility.h) and library
# -----------------------------
find_path(STORMBYTE_INCLUDE_DIR
    NAMES visibility.h
    PATH_SUFFIXES StormByte
    PATHS ${CMAKE_PREFIX_PATH} /usr/include /usr/local/include
)

find_library(STORMBYTE_LIBRARY
    NAMES StormByte
    PATH_SUFFIXES lib lib64
    PATHS ${CMAKE_PREFIX_PATH} /usr/lib /usr/lib64 /usr/local/lib /usr/local/lib64
)

# If the library was found but include dir not, attempt reasonable nearby include
if (STORMBYTE_LIBRARY AND NOT STORMBYTE_INCLUDE_DIR)
    get_filename_component(_storm_lib_dir "${STORMBYTE_LIBRARY}" PATH)
    set(_candidate_dirs
        "${_storm_lib_dir}/../include"
        "${_storm_lib_dir}/../../include"
        "${_storm_lib_dir}/include"
    )
    foreach(_cand IN LISTS _candidate_dirs)
        if (EXISTS "${_cand}/StormByte/visibility.h" OR EXISTS "${_cand}/visibility.h")
            set(STORMBYTE_INCLUDE_DIR "${_cand}")
            break()
        endif()
    endforeach()
endif()

mark_as_advanced(STORMBYTE_LIBRARY STORMBYTE_INCLUDE_DIR)

# -----------------------------
# Prepare output containers
# -----------------------------
unset(_found_components)
unset(_missing_components)

# -----------------------------
# Create core imported target if we found the library
# -----------------------------
if (STORMBYTE_LIBRARY)
    add_library(StormByte UNKNOWN IMPORTED GLOBAL)
    set_target_properties(StormByte PROPERTIES
        IMPORTED_LOCATION "${STORMBYTE_LIBRARY}"
    )

    if (STORMBYTE_INCLUDE_DIR AND EXISTS "${STORMBYTE_INCLUDE_DIR}")
        set_target_properties(StormByte PROPERTIES
            INTERFACE_INCLUDE_DIRECTORIES "${STORMBYTE_INCLUDE_DIR}"
        )
    endif()

    if (MSVC)
        target_compile_definitions(StormByte INTERFACE UNICODE NOMINMAX)
    endif()

    set(STORMBYTE_FOUND TRUE)
else()
    set(STORMBYTE_FOUND FALSE)
endif()

# -----------------------------
# For each requested component: detect header under the include tree or library
# -----------------------------
foreach(component IN LISTS _requested_components)
    # normalized names
    string(TOUPPER ${component} _UPCOMP)

    # default not found
    set(${component}_FOUND FALSE)
    unset(STORMBYTE_${component}_LIBRARIES)

    # 1) Check for a component header under the discovered include dir (preferred)
    if (STORMBYTE_INCLUDE_DIR AND EXISTS "${STORMBYTE_INCLUDE_DIR}")
        # Module include directories are lowercase under the StormByte include tree
        string(TOLOWER "${component}" _comp_lower)
        # Common layouts: <include>/StormByte/<component_lower>/visibility.h
        # or <include>/<component_lower>/visibility.h
        set(_candidate1 "${STORMBYTE_INCLUDE_DIR}/StormByte/${_comp_lower}/visibility.h")
        set(_candidate2 "${STORMBYTE_INCLUDE_DIR}/${_comp_lower}/visibility.h")
        if (EXISTS "${_candidate1}")
            set(${component}_FOUND TRUE)
        elseif (EXISTS "${_candidate2}")
            set(${component}_FOUND TRUE)
        endif()
    endif()

    # 2) Regardless, try to find a component library (StormByte-<Component>)
    find_library(STORMBYTE_${component}_LIBRARY
        NAMES StormByte-${component} StormByte_${component}
        PATH_SUFFIXES lib lib64
        PATHS ${CMAKE_PREFIX_PATH} /usr/lib /usr/lib64 /usr/local/lib /usr/local/lib64
    )
    if (STORMBYTE_${component}_LIBRARY)
        set(STORMBYTE_${component}_LIBRARIES "${STORMBYTE_${component}_LIBRARY}")
        set(${component}_FOUND TRUE)
    endif()

    # If found by header or library, create imported target and alias
    if (${component}_FOUND)
        add_library(StormByte-${component} UNKNOWN IMPORTED GLOBAL)
        add_library(StormByte::${component} ALIAS StormByte-${component})

        if (STORMBYTE_${component}_LIBRARIES)
            set_target_properties(StormByte-${component} PROPERTIES
                IMPORTED_LOCATION "${STORMBYTE_${component}_LIBRARIES}"
            )
        endif()

        # attach include dir from core if present
        if (STORMBYTE_INCLUDE_DIR AND EXISTS "${STORMBYTE_INCLUDE_DIR}")
            set_target_properties(StormByte-${component} PROPERTIES
                INTERFACE_INCLUDE_DIRECTORIES "${STORMBYTE_INCLUDE_DIR}"
            )
        endif()

        list(APPEND _found_components ${component})
    else()
        list(APPEND _missing_components ${component})
    endif()

    mark_as_advanced(STORMBYTE_${component}_LIBRARY)
endforeach()

# -----------------------------
# Populate transitive INTERFACE_LINK_LIBRARIES for component targets
# -----------------------------
foreach(component IN LISTS _found_components)
    set(_link_libraries StormByte)
    if (DEFINED _component_dependencies_${component})
        foreach(dep IN LISTS _component_dependencies_${component})
            if (dep IN_LIST _found_components)
                list(APPEND _link_libraries StormByte-${dep})
            endif()
        endforeach()
    endif()
    set_target_properties(StormByte-${component} PROPERTIES
        INTERFACE_LINK_LIBRARIES "${_link_libraries}"
    )
endforeach()

# -----------------------------
# Final status reporting
# -----------------------------
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
