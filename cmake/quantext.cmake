# -----------------------------------------------------------------------
# QuantExt shim
# -----------------------------------------------------------------------
# ore/QuantExt/qle/CMakeLists.txt is written to be consumed from within
# ORE's own top-level build, which pulls in ore/cmake/commonSettings.cmake.
# We deliberately do NOT include commonSettings.cmake here because it is far
# too invasive for this integration: it forces set(CMAKE_CXX_STANDARD 20)
# globally, find_package(ZLIB REQUIRED), sanitizer/MSVC flag sets, and turns
# on all the ORE_BUILD_* subprojects (docs/tests/examples/app/swig).
#
# Instead this shim supplies only the handful of things the qle CMakeLists
# actually references, then pulls the QuantExt library in via add_subdirectory
# with an explicit binary directory (keeps its build tree out of ore/).
#
# After this file the target QuantExt exists, aliased as both Ore::QuantExt
# (provided by qle) and QuantExt::QuantExt (added below).

# The library target names and package namespace that qle expects.  These are
# normally set in ore/cmake/commonSettings.cmake.
set(ORE_PACKAGE_NAME "Ore")
set(QLE_LIB_NAME     "QuantExt")
set(QL_LIB_NAME      "QuantLib::QuantLib")

# qle builds an umbrella header (quantext.hpp) using the writeAll() helper.
include("${CMAKE_CURRENT_SOURCE_DIR}/ore/cmake/writeAll.cmake")

# Needed for CMAKE_INSTALL_INCLUDEDIR referenced by qle's target_include_directories
# and install() commands.
include(GNUInstallDirs)

# QuantExt needs a few more Boost components than QuantLibObjects (timer, thread)
# in addition to boost headers and serialization.
if(MSVC)
	if(NOT MSVC_LINK_DYNAMIC_RUNTIME)
		set(Boost_USE_STATIC_RUNTIME ON)
	endif()
	set(Boost_USE_STATIC_LIBS ON)
endif()
find_package(Boost 1.58.0 REQUIRED COMPONENTS serialization timer thread)

# The qle gitVersionGeneration custom target invokes
#   ${PROJECT_SOURCE_DIR}/cmake/generateGitVersion.cmake
# where PROJECT_SOURCE_DIR normally points at the ORE source root.  In this
# nested build PROJECT_SOURCE_DIR is the QuantLibAddin root (the last project()
# call), so we temporarily repoint it at the ore/ subtree while qle is
# processed, then restore it for the remaining subprojects.
set(_qle_saved_project_source_dir "${PROJECT_SOURCE_DIR}")
set(PROJECT_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/ore")

add_subdirectory(ore/QuantExt "${CMAKE_BINARY_DIR}/QuantExt")

set(PROJECT_SOURCE_DIR "${_qle_saved_project_source_dir}")

# Provide the QuantExt::QuantExt alias used by the QuantLibAddin wiring, to sit
# alongside the Ore::QuantExt alias that qle already defines.
if(NOT TARGET QuantExt::QuantExt)
	add_library(QuantExt::QuantExt ALIAS QuantExt)
endif()

# ORE mandates C++20 for its sources; the rest of this build uses the QuantLib
# default (C++17).  Confine the C++20 requirement to the QuantExt target so the
# QuantExt sources compile while the remainder of the build is unaffected.
set_target_properties(QuantExt PROPERTIES CXX_STANDARD 20 CXX_STANDARD_REQUIRED ON)

# QuantExt has several very large translation units that overflow the default
# object-file section limit under MSVC; ORE's commonSettings adds /bigobj
# globally, we scope it to this target.
if(MSVC)
	target_compile_options(QuantExt PRIVATE /bigobj)
endif()
