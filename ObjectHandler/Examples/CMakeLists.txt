# ObjectHandler example programs.
#
# Mirrors the example projects in the hand-maintained ObjectHandler.sln:
#
#   ExampleObjects : static library of sample business objects (Account,
#                    Customer) used by the example program.
#   ExampleCpp     : console program that exercises the ObjectHandler
#                    repository using those sample objects.
#
# The Excel example add-ins (Examples/xl) are intentionally not built here —
# the XLL build is owned by QuantLibXL.

# -----------------------------------------------------------------------
# ExampleObjects  (static library of sample business objects)
# -----------------------------------------------------------------------
add_library(ExampleObjects STATIC
	ExampleObjects/Enumerations/accountenumerations.cpp
	ExampleObjects/Serialization/creators.cpp
	ExampleObjects/Serialization/serializationfactory.cpp
	ExampleObjects/ValueObjects/accountvalueobject.cpp
	ExampleObjects/ValueObjects/customervalueobject.cpp
)

# Include roots used by the sample headers (mirrors the VS project's
# AdditionalIncludeDirectories "..;..\.."):
#   ${CMAKE_CURRENT_SOURCE_DIR}     → <ExampleObjects/...>
#   ${CMAKE_CURRENT_SOURCE_DIR}/..  → <oh/...>, <Examples/...>
target_include_directories(ExampleObjects PUBLIC
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>
)

# ohlib brings in the oh/ headers, Boost (filesystem, serialization) and the
# OH_NO_AUTO_LINK definition that disables the #pragma comment(lib, ...)
# auto-link in oh/auto_link.hpp.
target_link_libraries(ExampleObjects PUBLIC ohlib)

# -----------------------------------------------------------------------
# ExampleCpp  (console program exercising the ObjectHandler repository)
#
# Output naming mirrors the other cmake client executables (e.g. QLADemo):
#   ExampleCpp-v<toolset>-x64-mt[-s|-gd|-sgd]-1_42_0.exe
# written to  <build>/examples/.
# -----------------------------------------------------------------------
if(MSVC)
	set(_ohex_toolset_tag "v${MSVC_TOOLSET_VERSION}")
else()
	set(_ohex_toolset_tag "")
endif()

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
	set(_ohex_platform_tag "-x64")
else()
	set(_ohex_platform_tag "")
endif()

if(MSVC_LINK_DYNAMIC_RUNTIME)
	set(_ohex_release_runtime_tag "-mt")
	set(_ohex_debug_runtime_tag   "-mt-gd")
else()
	set(_ohex_release_runtime_tag "-mt-s")
	set(_ohex_debug_runtime_tag   "-mt-sgd")
endif()

set(_ohex_version_tag "1_42_0")
set(_ohex_base_name "ExampleCpp-${_ohex_toolset_tag}${_ohex_platform_tag}")

add_executable(ExampleCpp "C++/example.cpp")

set_target_properties(ExampleCpp PROPERTIES
	OUTPUT_NAME
		"${_ohex_base_name}$<$<CONFIG:Debug>:${_ohex_debug_runtime_tag}>$<$<NOT:$<CONFIG:Debug>>:${_ohex_release_runtime_tag}>-${_ohex_version_tag}"
	RUNTIME_OUTPUT_DIRECTORY                "${CMAKE_BINARY_DIR}/examples"
	RUNTIME_OUTPUT_DIRECTORY_RELEASE        "${CMAKE_BINARY_DIR}/examples"
	RUNTIME_OUTPUT_DIRECTORY_DEBUG          "${CMAKE_BINARY_DIR}/examples"
	RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO "${CMAKE_BINARY_DIR}/examples"
	RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL     "${CMAKE_BINARY_DIR}/examples"
)

# ExampleObjects propagates the include roots and (transitively) ohlib, which
# provides the oh/ headers, Boost and OH_NO_AUTO_LINK.
target_link_libraries(ExampleCpp PRIVATE ExampleObjects)

if(WIN32)
	target_link_libraries(ExampleCpp PRIVATE
		odbc32
		odbccp32
		Ws2_32
		advapi32
	)
endif()
