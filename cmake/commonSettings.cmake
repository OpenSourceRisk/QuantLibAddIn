include_guard(GLOBAL)

# Boost lookup: honour environment variables used by the VS build.
# BOOST_INCLUDEDIR and BOOST_LIBRARYDIR can be set on the cmake command line
# or via the preset cacheVariables.  Alternatively the caller may set
# BOOST_ROOT / Boost_DIR for newer CMake FindBoost / BoostConfig logic.
if(DEFINED ENV{BOOST})
    if(POLICY CMP0167)
        cmake_policy(SET CMP0167 OLD)
        set(CMAKE_POLICY_DEFAULT_CMP0167 OLD)
    endif()
    set(BOOST_INCLUDEDIR "$ENV{BOOST}" CACHE STRING "Boost include dir (from env BOOST)")
endif()

# -----------------------------------------------------------------------
# Options
# -----------------------------------------------------------------------
option(MSVC_LINK_DYNAMIC_RUNTIME "Link against the dynamic CRT (/MD, /MDd)" ON)
option(MSVC_PARALLELBUILD         "Add /MP flag for parallel compilation"    ON)

# -----------------------------------------------------------------------
# C++ standard
# -----------------------------------------------------------------------
if(NOT DEFINED CMAKE_CXX_STANDARD)
    set(CMAKE_CXX_STANDARD 17)
endif()
set(CMAKE_CXX_STANDARD_REQUIRED ON)
if(NOT DEFINED CMAKE_CXX_EXTENSIONS)
    set(CMAKE_CXX_EXTENSIONS FALSE)
endif()

# -----------------------------------------------------------------------
# MSVC-specific settings
# -----------------------------------------------------------------------
if(MSVC)
    # Always build static libs (the XLL is the only DLL in this build).
    set(BUILD_SHARED_LIBS OFF)

    # CRT selection — must be set before any add_library / add_executable.
    # CMP0091 NEW is required (set in the root CMakeLists.txt).
    set(CMAKE_MSVC_RUNTIME_LIBRARY
        "MultiThreaded$<$<CONFIG:Debug>:Debug>$<$<BOOL:${MSVC_LINK_DYNAMIC_RUNTIME}>:DLL>"
    )

    add_compile_options(
        /W3
        /wd4819          # character outside source character set
        /bigobj
        "$<$<CONFIG:Release>:/O2;/GF;/Gy>"
        "$<$<CONFIG:RelWithDebInfo>:/O2;/GF;/Gy>"
        $<$<BOOL:${MSVC_PARALLELBUILD}>:/MP>
    )

    add_compile_definitions(
        WIN32
        _WINDOWS
        _SCL_SECURE_NO_DEPRECATE
        _CRT_SECURE_NO_DEPRECATE
        BOOST_ALL_NO_LIB         # disable Boost auto-link; cmake handles linking
        QLADDIN_NO_AUTO_LINK     # disable QuantLibAddin auto_link.hpp pragma comment
        OH_NO_AUTO_LINK          # disable ObjectHandler  auto_link.hpp pragma comment
        XLSDK_NO_AUTO_LINK       # disable xlsdk          auto_link.hpp pragma comment
    )

    add_link_options(
        /LARGEADDRESSAWARE
    )
endif()
