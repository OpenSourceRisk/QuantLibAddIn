# QuantLibXL/Docs/CMakeLists.txt
#
# Builds the QuantLibXL HTML documentation with Doxygen, replicating the legacy
# automake "make docs" target for this project.
#
# This directory can be configured in TWO ways:
#
#   * Standalone — `cmake -S QuantLibXL/Docs -B <build>` — which requires only
#     Python 3, Doxygen and (optionally) Graphviz dot.  No C++ compiler or Boost
#     is needed, so the documentation can be built on a machine that cannot
#     compile the libraries.  That is why the standalone project is declared
#     with no languages (project(... NONE)).  (Note: the QuantLibXL *library* is
#     Windows-only, but its documentation builds on any platform, including the
#     Linux/WSL docs build.)
#
#   * As part of the top-level build — enabled with -DBUILD_DOCS=ON in the
#     root CMakeLists.txt — in which case the targets below are simply added to
#     the main project.
#
# The generated HTML is written under the build tree
# (<build>/QuantLibXL-docs/html/index.html), keeping the source tree clean.

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
	cmake_minimum_required(VERSION 3.15)
	project(QuantLibXLDocs NONE)
endif()

# Locate the shared documentation helper in <repo-root>/gensrc/cmake.
get_filename_component(_repo_root "${CMAKE_CURRENT_SOURCE_DIR}/../.." ABSOLUTE)
include("${_repo_root}/gensrc/cmake/DocsCommon.cmake")

# QuantLibXL's documentation reuses the QuantLibAddin Docs/auto.pages produced by
# `gensrc.py -d` (run in QuantLibAddin/gensrc with --oh_dir=../../ObjectHandler).
# In the integrated build QuantLibAddin/Docs is processed first and has already
# defined the QuantLibAddin-docs-gensrc target, so add_gensrc_docs reuses it
# (it is a no-op when the target exists).  In a standalone QuantLibXL/Docs build
# the target does not exist yet, so the same call defines it here.
add_gensrc_docs(QuantLibAddin-docs-gensrc
	"${CMAKE_CURRENT_SOURCE_DIR}/../../QuantLibAddin/gensrc" "../../ObjectHandler")

add_doxygen_docs(QuantLibXL-docs "${CMAKE_CURRENT_SOURCE_DIR}" quantlibxl.doxy)
add_dependencies(QuantLibXL-docs QuantLibAddin-docs-gensrc)

# When configured standalone, provide a convenience `docs` target so that
# `cmake --build <build> --target docs` works the same as in the other projects.
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
	add_custom_target(docs)
	add_dependencies(docs QuantLibXL-docs)
endif()
