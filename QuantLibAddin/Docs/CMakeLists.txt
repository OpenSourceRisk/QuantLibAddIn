# QuantLibAddin/Docs/CMakeLists.txt
#
# Builds the QuantLibAddin HTML documentation with Doxygen, replicating the
# legacy automake "make docs" target for this project.
#
# This directory can be configured in TWO ways:
#
#   * Standalone — `cmake -S QuantLibAddin/Docs -B <build>` — which requires
#     only Python 3, Doxygen and (optionally) Graphviz dot.  No C++ compiler or
#     Boost is needed, so the documentation can be built on a machine that
#     cannot compile the libraries.  That is why the standalone project is
#     declared with no languages (project(... NONE)).
#
#   * As part of the top-level build — enabled with -DBUILD_DOCS=ON in the
#     root CMakeLists.txt — in which case the targets below are simply added to
#     the main project.
#
# The generated HTML is written under the build tree
# (<build>/QuantLibAddin-docs/html/index.html), keeping the source tree clean.

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
	cmake_minimum_required(VERSION 3.15)
	project(QuantLibAddinDocs NONE)
endif()

# Locate the shared documentation helper in <repo-root>/gensrc/cmake.
get_filename_component(_repo_root "${CMAKE_CURRENT_SOURCE_DIR}/../.." ABSOLUTE)
include("${_repo_root}/gensrc/cmake/DocsCommon.cmake")

# QuantLibAddin's documentation consumes the Docs/auto.pages (function reference
# and enumeration pages) produced by `gensrc.py -d`.  gensrc runs with its
# working directory set to QuantLibAddin/gensrc and --oh_dir=../../ObjectHandler.
# On a pristine checkout these pages do not exist yet, so the gensrc step is
# wired as a prerequisite below.  The target is named QuantLibAddin-docs-gensrc
# so that QuantLibXL's documentation (which reuses these auto.pages) can depend
# on the very same target in the integrated build.
add_gensrc_docs(QuantLibAddin-docs-gensrc
	"${CMAKE_CURRENT_SOURCE_DIR}/../gensrc" "../../ObjectHandler")

add_doxygen_docs(QuantLibAddin-docs "${CMAKE_CURRENT_SOURCE_DIR}" quantlibaddin.doxy)
add_dependencies(QuantLibAddin-docs QuantLibAddin-docs-gensrc)

# When configured standalone, provide a convenience `docs` target so that
# `cmake --build <build> --target docs` works the same as in the other projects.
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
	add_custom_target(docs)
	add_dependencies(docs QuantLibAddin-docs)
endif()
