# RunDoxygen.cmake — build-time driver, invoked through `cmake -P`.
#
# Replicates the legacy automake "make docs" recipe for a single project:
#   1. Preprocess the .doxy file with ObjectHandler/Docs/preprocess_doxyfile.py
#      (sets STRIP_FROM_PATH and, on posix, HAVE_DOT=YES / GENERATE_HTMLHELP=NO).
#   2. Append a block of overrides that confine every output to the cmake build
#      tree (doxygen honours the *last* assignment of each tag).
#   3. Run doxygen with the working directory set to the project's Docs dir
#      (the .doxy files use INPUT / HTML_HEADER paths relative to that dir).
#   4. Copy the auxiliary stylesheets and images into the generated html, exactly
#      as the Makefile.am did (doxygen only copies HTML_STYLESHEET itself).
#
# Required -D parameters:
#   DOXY_DIR    absolute path to the project's Docs directory
#   DOXY_FILE   the doxygen config file name (e.g. gensrc.doxy)
#   PREPROCESS  absolute path to preprocess_doxyfile.py
#   PYTHON      python interpreter
#   DOXYGEN     doxygen executable
#   OUT_DIR     absolute output directory (html is written to ${OUT_DIR}/html)
# Optional -D parameters:
#   DOT_PATH    directory containing the dot (graphviz) executable

foreach(_req DOXY_DIR DOXY_FILE PREPROCESS PYTHON DOXYGEN OUT_DIR)
	if(NOT ${_req})
		message(FATAL_ERROR "RunDoxygen.cmake: missing required parameter ${_req}")
	endif()
endforeach()

file(MAKE_DIRECTORY "${OUT_DIR}")

set(_temp_doxy "${OUT_DIR}/processed.doxy")

# 1. Preprocess.  Must run with cwd = Docs dir so that the STRIP_FROM_PATH value
#    computed by the script (parent of the current directory) is correct, and so
#    the input file name resolves relative to the Docs dir.
execute_process(
	COMMAND "${PYTHON}" "${PREPROCESS}" "${DOXY_FILE}" "${_temp_doxy}"
	WORKING_DIRECTORY "${DOXY_DIR}"
	RESULT_VARIABLE _pp_rc)
if(NOT _pp_rc EQUAL 0)
	message(FATAL_ERROR "preprocess_doxyfile.py failed (${_pp_rc}) for ${DOXY_FILE}")
endif()

# 2. Append overrides that keep all generated output inside the build tree.
set(_overrides "
# ---- overrides appended by cmake/RunDoxygen.cmake ----
OUTPUT_DIRECTORY  = ${OUT_DIR}
HTML_OUTPUT       = html
GENERATE_HTML     = YES
GENERATE_HTMLHELP = NO
GENERATE_LATEX    = NO
HAVE_DOT          = YES
WARN_LOGFILE      = ${OUT_DIR}/doxywarnings.txt
")
if(DOT_PATH)
	string(APPEND _overrides "DOT_PATH          = ${DOT_PATH}\n")
endif()
file(APPEND "${_temp_doxy}" "${_overrides}")

# 3. Run doxygen.  cwd = Docs dir: the INPUT / HTML_HEADER / HTML_STYLESHEET
#    paths in the .doxy files are relative to that directory.
execute_process(
	COMMAND "${DOXYGEN}" "${_temp_doxy}"
	WORKING_DIRECTORY "${DOXY_DIR}"
	RESULT_VARIABLE _dox_rc)
if(NOT _dox_rc EQUAL 0)
	message(FATAL_ERROR "doxygen failed (${_dox_rc}) for ${DOXY_FILE}")
endif()

# 4. Copy the auxiliary stylesheets and images into the html output, mirroring
#    the legacy Makefile.am (doxygen itself only copies HTML_STYLESHEET).
set(_html "${OUT_DIR}/html")
foreach(_css tabs.css ql.css)
	if(EXISTS "${DOXY_DIR}/${_css}")
		file(COPY "${DOXY_DIR}/${_css}" DESTINATION "${_html}")
	endif()
endforeach()
file(GLOB _imgs
	 "${DOXY_DIR}/images/*.ico"
	 "${DOXY_DIR}/images/*.jpg"
	 "${DOXY_DIR}/images/*.png")
if(_imgs)
	file(COPY ${_imgs} DESTINATION "${_html}/images")
endif()

message(STATUS "Documentation generated: ${_html}/index.html")
