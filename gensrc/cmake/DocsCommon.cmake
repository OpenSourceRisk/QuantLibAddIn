# DocsCommon.cmake — shared helper for the per-project documentation builds.
# This module is a generic piece of code-generation tooling and lives alongside
# gensrc (<root>/gensrc/cmake/DocsCommon.cmake).  Each project's
# Docs/CMakeLists.txt include()s it to discover the documentation tools
# (Python 3, Doxygen, optional Graphviz dot) and to gain two helper functions:
#
#   add_doxygen_docs(<target> <docs_dir> <doxy_file>)
#       Define <target>, which runs RunDoxygen.cmake to build the HTML
#       documentation for one project into the build tree
#       (<binary_dir>/<target>/html/index.html).
#
#   add_gensrc_docs(<target> <gensrc_dir> <oh_dir>)
#       Define <target> (unless it already exists), which runs `gensrc.py -d`
#       with the working directory set to <gensrc_dir> to (re)generate the
#       Docs/auto.pages Doxygen input.  Guarded by `if(NOT TARGET)` so that a
#       project which consumes another project's auto.pages can request the same
#       target by name without redefining it.
#
# Building the documentation needs only Python 3 and Doxygen, never a C++
# compiler or Boost, so a project's Docs directory can be configured standalone
# with project(<Name>Docs NONE).

# Run the tool discovery below only once per configuration, even though all four
# sibling Docs directories include this module.  The functions defined further
# down are global once defined, so a single inclusion makes them available to
# every directory.
include_guard(GLOBAL)

# The repository root is two levels above the directory holding this module
# (<root>/gensrc/cmake/DocsCommon.cmake).
get_filename_component(_docs_root "${CMAKE_CURRENT_LIST_DIR}/../.." ABSOLUTE)

# Paths are stored as cache variables so the helper functions see them no matter
# which directory scope calls them.  RunDoxygen.cmake sits next to this module.
set(DOCS_RUN_DOXYGEN "${CMAKE_CURRENT_LIST_DIR}/RunDoxygen.cmake"
	CACHE INTERNAL "RunDoxygen.cmake driver")
set(DOCS_PREPROCESS  "${_docs_root}/ObjectHandler/Docs/preprocess_doxyfile.py"
	CACHE INTERNAL "Shared doxyfile preprocessor")
set(DOCS_GENSRC_PY   "${_docs_root}/gensrc/gensrc.py"
	CACHE INTERNAL "gensrc.py code generator")

# -----------------------------------------------------------------------
# Tools.  Python 3 runs gensrc and the doxyfile preprocessor; Doxygen builds the
# HTML; Graphviz dot is optional and enables the inheritance / collaboration
# graphs (HAVE_DOT).
# -----------------------------------------------------------------------
find_package(Python3 REQUIRED COMPONENTS Interpreter)
set(DOCS_PYTHON "${Python3_EXECUTABLE}" CACHE INTERNAL "Python 3 interpreter")

# find_program's REQUIRED keyword is only available from CMake 3.18, but the
# project's minimum is 3.15, so check the result by hand for a clear message.
find_program(DOCS_DOXYGEN doxygen)
if(NOT DOCS_DOXYGEN)
	message(FATAL_ERROR
		"doxygen was not found on PATH; cannot build the documentation. "
		"Install Doxygen (and Graphviz for diagrams) and re-run cmake.")
endif()
message(STATUS "Docs: using doxygen at ${DOCS_DOXYGEN}")

find_program(DOCS_DOT dot)
if(DOCS_DOT)
	get_filename_component(_docs_dot_dir "${DOCS_DOT}" DIRECTORY)
	set(DOCS_DOT_DIR "${_docs_dot_dir}" CACHE INTERNAL "Directory containing dot")
	message(STATUS "Docs: using dot at ${DOCS_DOT} (diagrams enabled)")
else()
	set(DOCS_DOT_DIR "" CACHE INTERNAL "Directory containing dot")
	message(STATUS "Docs: dot (Graphviz) not found; diagrams will be skipped")
endif()

# -----------------------------------------------------------------------
# add_doxygen_docs(<target> <docs_dir> <doxy_file>)
#
# Define a per-project documentation target that drives RunDoxygen.cmake.
# The generated HTML is written to <binary_dir>/<target>/html, keeping the
# source tree clean.
# -----------------------------------------------------------------------
function(add_doxygen_docs _target _docs_dir _doxy_file)
	set(_out "${CMAKE_CURRENT_BINARY_DIR}/${_target}")
	set(_args
		-DDOXY_DIR=${_docs_dir}
		-DDOXY_FILE=${_doxy_file}
		-DPREPROCESS=${DOCS_PREPROCESS}
		-DPYTHON=${DOCS_PYTHON}
		-DDOXYGEN=${DOCS_DOXYGEN}
		-DOUT_DIR=${_out})
	if(DOCS_DOT_DIR)
		list(APPEND _args -DDOT_PATH=${DOCS_DOT_DIR})
	endif()
	add_custom_target(${_target}
		COMMAND ${CMAKE_COMMAND} ${_args} -P "${DOCS_RUN_DOXYGEN}"
		COMMENT "Building ${_target} HTML documentation with Doxygen"
		VERBATIM)
endfunction()

# -----------------------------------------------------------------------
# add_gensrc_docs(<target> <gensrc_dir> <oh_dir>)
#
# gensrc.py -d regenerates the Docs/auto.pages Doxygen input (the function
# reference and enumeration pages).  <gensrc_dir> is the working directory
# (a project's gensrc dir) and <oh_dir> is the value passed to --oh_dir.
# Idempotent: a project that consumes another project's auto.pages can ask for
# the same target name and reuse it instead of running gensrc twice.
# -----------------------------------------------------------------------
function(add_gensrc_docs _target _gensrc_dir _oh_dir)
	if(NOT TARGET ${_target})
		add_custom_target(${_target}
			COMMAND ${CMAKE_COMMAND} -E chdir "${_gensrc_dir}"
					"${DOCS_PYTHON}" "${DOCS_GENSRC_PY}" -d --oh_dir=${_oh_dir}
			COMMENT "Running gensrc -d in ${_gensrc_dir} to generate Doxygen auto.pages"
			VERBATIM)
	endif()
endfunction()
