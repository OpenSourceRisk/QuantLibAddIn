# gensrc/Docs/CMakeLists.txt
#
# Builds the gensrc HTML documentation with Doxygen, replicating the legacy
# automake "make docs" target for this project.
#
# This directory can be configured in TWO ways:
#
#   * Standalone — `cmake -S gensrc/Docs -B <build>` — which requires only
#     Python 3, Doxygen and (optionally) Graphviz dot.  No C++ compiler or
#     Boost is needed, so the documentation can be built on a machine that
#     cannot compile the libraries.  That is why the standalone project is
#     declared with no languages (project(... NONE)).
#
#   * As part of the top-level build — enabled with -DBUILD_DOCS=ON in the
#     root CMakeLists.txt — in which case the gensrc-docs target is simply added
#     to the main project.
#
# The generated HTML is written under the build tree
# (<build>/gensrc-docs/html/index.html), keeping the source tree clean.

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
	cmake_minimum_required(VERSION 3.15)
	project(gensrcDocs NONE)
endif()

# Locate the shared documentation helper in <repo-root>/gensrc/cmake.
get_filename_component(_repo_root "${CMAKE_CURRENT_SOURCE_DIR}/../.." ABSOLUTE)
include("${_repo_root}/gensrc/cmake/DocsCommon.cmake")

# gensrc HTML docs.  Unlike ObjectHandler / QuantLibAddin / QuantLibXL, gensrc's
# own documentation does not consume the gensrc-generated Docs/auto.pages, so no
# gensrc prerequisite is wired here.
add_doxygen_docs(gensrc-docs "${CMAKE_CURRENT_SOURCE_DIR}" gensrc.doxy)

# When configured standalone, provide a convenience `docs` target so that
# `cmake --build <build> --target docs` works the same as in the other projects.
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
	add_custom_target(docs)
	add_dependencies(docs gensrc-docs)
endif()
