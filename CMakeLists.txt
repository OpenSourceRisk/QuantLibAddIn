cmake_minimum_required(VERSION 3.15)

# Required for CMAKE_MSVC_RUNTIME_LIBRARY (CMP0091)
cmake_policy(SET CMP0091 NEW)

project(QuantLibAddin CXX)

include(cmake/commonSettings.cmake)

# -----------------------------------------------------------------------
# QuantLib (adds target ql_library, aliased as QuantLib::QuantLib)
# -----------------------------------------------------------------------
set(QL_BUILD_EXAMPLES   OFF CACHE BOOL "" FORCE)
set(QL_BUILD_TEST_SUITE OFF CACHE BOOL "" FORCE)
# QL_TAGGED_LAYOUT is ON by default when MSVC is used, which sets
# CMAKE_RELEASE_POSTFIX / CMAKE_DEBUG_POSTFIX globally.  Those global
# postfixes would affect every cmake target in the build, not just
# ql_library.  We therefore turn the flag OFF here so that only
# ql_library gets its postfix set (we do that explicitly below).
set(QL_TAGGED_LAYOUT OFF CACHE BOOL "" FORCE)
add_subdirectory(QuantLib)

# Apply tagged layout postfixes to ql_library only, mirroring what
# QL_TAGGED_LAYOUT would have done, so the library filename matches
# the filenames expected by the hand-maintained VS build.
if(MSVC)
    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
        set(_ql_arch_tag "-x64")
    else()
        set(_ql_arch_tag "")
    endif()
    if(MSVC_LINK_DYNAMIC_RUNTIME)
        set(_ql_debug_postfix   "${_ql_arch_tag}-mt-gd")
        set(_ql_release_postfix "${_ql_arch_tag}-mt")
    else()
        set(_ql_debug_postfix   "${_ql_arch_tag}-mt-sgd")
        set(_ql_release_postfix "${_ql_arch_tag}-mt-s")
    endif()
    set_target_properties(ql_library PROPERTIES
        DEBUG_POSTFIX          "${_ql_debug_postfix}"
        RELEASE_POSTFIX        "${_ql_release_postfix}"
        RELWITHDEBINFO_POSTFIX "${_ql_release_postfix}"
        MINSIZEREL_POSTFIX     "${_ql_release_postfix}"
    )
endif()

# -----------------------------------------------------------------------
# ObjectHandler static libs (ohlib + ohxllib) and xlsdk
# -----------------------------------------------------------------------
add_subdirectory(ObjectHandler)

# -----------------------------------------------------------------------
# QuantLibObjects static lib (QuantLibAddin qlo wrapper)
# -----------------------------------------------------------------------
add_subdirectory(QuantLibAddin)

# -----------------------------------------------------------------------
# QuantLibXL — the Excel XLL
# -----------------------------------------------------------------------
add_subdirectory(QuantLibXL)
