# ObjectHandler — builds the static libraries consumed by the XLL.
#
# This directory can be configured in TWO ways:
#
#   * As part of the top-level build (add_subdirectory from the parent).
#
#   * Standalone — `cmake -S ObjectHandler -B <build>` — to build just the
#     ObjectHandler libraries (and, with -DBUILD_DOCS=ON, its documentation)
#     with no reference to any downstream project.  The only external piece it
#     relies on is the generic build settings in
#     gensrc/cmake/commonSettings.cmake (gensrc is ObjectHandler's upstream
#     dependency, so referring to it does not violate the project hierarchy).
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    cmake_minimum_required(VERSION 3.15)
    # Required for CMAKE_MSVC_RUNTIME_LIBRARY (CMP0091) — must precede project().
    cmake_policy(SET CMP0091 NEW)
    project(ObjectHandler CXX)
    include("${CMAKE_CURRENT_SOURCE_DIR}/../gensrc/cmake/commonSettings.cmake")
endif()

# Ensure Boost imported targets are available (find_package is idempotent).
# Static Boost (and static runtime) is only forced for the MSVC/Windows build;
# on Linux we use the platform's default Boost libraries.
if(MSVC)
    if(NOT MSVC_LINK_DYNAMIC_RUNTIME)
        set(Boost_USE_STATIC_RUNTIME ON)
    endif()
    set(Boost_USE_STATIC_LIBS ON)
endif()
find_package(Boost 1.58.0 REQUIRED COMPONENTS filesystem serialization)


#   xlsdk          : Excel SDK (framewrk.cpp + xlcall32.lib)   [Windows only]
#   ohlib          : ObjectHandler core (oh/*.cpp)
#   ohxllib        : ObjectHandler Excel layer (ohxl/**/*.cpp) [Windows only]

# -----------------------------------------------------------------------
# xlsdk  (Windows/Excel only)
# -----------------------------------------------------------------------
if(WIN32)
add_library(xlsdk STATIC
    xlsdk/framewrk.cpp
)

target_include_directories(xlsdk PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

# Suppress the xlsdk/auto_link.hpp #pragma comment(lib, ...) directive; cmake
# manages linking.  PUBLIC so every consumer of xlsdk inherits it too.
target_compile_definitions(xlsdk PUBLIC XLSDK_NO_AUTO_LINK)

# Link against the pre-built xlcall32.lib that ships with the SDK.
# x64 copy lives in xlsdk/x64/; Win32 copy is xlsdk/xlcall32.lib.
target_link_libraries(xlsdk PUBLIC
    $<IF:$<EQUAL:${CMAKE_SIZEOF_VOID_P},8>,
        ${CMAKE_CURRENT_SOURCE_DIR}/xlsdk/x64/XLCALL32.LIB,
        ${CMAKE_CURRENT_SOURCE_DIR}/xlsdk/xlcall32.lib>
)
endif()

# -----------------------------------------------------------------------
# ohlib  (ObjectHandler core — no Excel dependency)
# -----------------------------------------------------------------------
add_library(ohlib STATIC
    oh/enumerations/enumregistry.cpp
    oh/logger.cpp
    oh/processor.cpp
    oh/repository.cpp
    oh/serializationfactory.cpp
    oh/utilities.cpp
)

target_include_directories(ohlib PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

# Suppress the oh/auto_link.hpp #pragma comment(lib, ...) directive; cmake
# manages linking.  PUBLIC so every consumer of ohlib inherits it too.
target_compile_definitions(ohlib PUBLIC OH_NO_AUTO_LINK)

target_link_libraries(ohlib PUBLIC Boost::boost Boost::filesystem Boost::serialization)

# -----------------------------------------------------------------------
# ohxllib  (ObjectHandler Excel layer — depends on ohlib + xlsdk) [Windows only]
# -----------------------------------------------------------------------
if(WIN32)
add_library(ohxllib STATIC
    ohxl/callingrange.cpp
    ohxl/configuration.cpp
    ohxl/convert_oper.cpp
    ohxl/conversions/scalartooper.cpp
    ohxl/conversions/validations.cpp
    ohxl/functioncall.cpp
    ohxl/functions/enumerations.cpp
    ohxl/functions/garbagecollection.cpp
    ohxl/functions/group.cpp
    ohxl/functions/logging.cpp
    ohxl/functions/manual.cpp
    ohxl/functions/objects.cpp
    ohxl/functions/ohutils.cpp
    ohxl/functions/range.cpp
    ohxl/functions/serialization.cpp
    ohxl/functions/valueobjects.cpp
    ohxl/objectwrapperxl.cpp
    ohxl/rangereference.cpp
    ohxl/register/addin.cpp
    ohxl/register/register_all.cpp
    ohxl/register/register_enumerations.cpp
    ohxl/register/register_garbagecollection.cpp
    ohxl/register/register_group.cpp
    ohxl/register/register_logging.cpp
    ohxl/register/register_objects.cpp
    ohxl/register/register_ohutils.cpp
    ohxl/register/register_range.cpp
    ohxl/register/register_serialization.cpp
    ohxl/register/register_valueobjects.cpp
    ohxl/repositoryxl.cpp
    ohxl/utilities/xlutilities.cpp
)

target_include_directories(ohxllib PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

target_link_libraries(ohxllib PUBLIC ohlib xlsdk)
endif()

# -----------------------------------------------------------------------
# Examples — the sample objects library and the C++ example program that
# ship with ObjectHandler (Examples/ExampleObjects + Examples/C++).  Gated on
# BUILD_EXAMPLES so the integrated build only adds them when requested.  The
# option is (re)declared here so that this value is also honoured when
# ObjectHandler is configured on its own.
# -----------------------------------------------------------------------
option(BUILD_EXAMPLES "Add targets to build the ObjectHandler example programs" OFF)
if(BUILD_EXAMPLES)
    add_subdirectory(Examples)
endif()

# -----------------------------------------------------------------------
# Documentation — ObjectHandler owns its own Docs build (ObjectHandler-docs
# target).  Gated on BUILD_DOCS so the integrated build only adds it when
# requested.  The option is (re)declared here so that this value is also
# honoured when ObjectHandler is configured on its own.
# -----------------------------------------------------------------------
option(BUILD_DOCS "Add targets to build the HTML documentation with Doxygen" OFF)
if(BUILD_DOCS)
    add_subdirectory(Docs)
endif()
