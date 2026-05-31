# QuantLibAddin/Clients/Cpp/CMakeLists.txt
# Builds the QLADemo console executable (C++ addin demo client).
#
# Output naming mirrors Build A:
#   QLADemo-v<toolset>-x64-mt[-s|-gd|-sgd]-1_23_0.exe
#
# The executable is written to  build/<preset>/cpp/
# which keeps it separate from the Build A output in
# QuantLibAddin/Clients/Cpp/bin/.

# -----------------------------------------------------------------------
# Toolset / platform / runtime tags  (same logic as QuantLibXL)
# -----------------------------------------------------------------------
if(MSVC)
    set(QLADEMO_TOOLSET_TAG "v${MSVC_TOOLSET_VERSION}")
else()
    set(QLADEMO_TOOLSET_TAG "")
endif()

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(QLADEMO_PLATFORM_TAG "-x64")
else()
    set(QLADEMO_PLATFORM_TAG "")
endif()

if(MSVC_LINK_DYNAMIC_RUNTIME)
    set(QLADEMO_RELEASE_RUNTIME_TAG "-mt")
    set(QLADEMO_DEBUG_RUNTIME_TAG   "-mt-gd")
else()
    set(QLADEMO_RELEASE_RUNTIME_TAG "-mt-s")
    set(QLADEMO_DEBUG_RUNTIME_TAG   "-mt-sgd")
endif()

set(QLADEMO_VERSION_TAG "1_23_0")

set(QLADEMO_BASE_NAME
    "QLADemo-${QLADEMO_TOOLSET_TAG}${QLADEMO_PLATFORM_TAG}"
)

# -----------------------------------------------------------------------
# Executable target
# -----------------------------------------------------------------------
add_executable(QLADemo qlademo.cpp)

set_target_properties(QLADemo PROPERTIES
    OUTPUT_NAME
        "${QLADEMO_BASE_NAME}$<$<CONFIG:Debug>:${QLADEMO_DEBUG_RUNTIME_TAG}>$<$<NOT:$<CONFIG:Debug>>:${QLADEMO_RELEASE_RUNTIME_TAG}>-${QLADEMO_VERSION_TAG}"
    RUNTIME_OUTPUT_DIRECTORY             "${CMAKE_BINARY_DIR}/cpp"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE     "${CMAKE_BINARY_DIR}/cpp"
    RUNTIME_OUTPUT_DIRECTORY_DEBUG       "${CMAKE_BINARY_DIR}/cpp"
    RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO "${CMAKE_BINARY_DIR}/cpp"
    RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL  "${CMAKE_BINARY_DIR}/cpp"
)

target_include_directories(QLADemo PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../..
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../ObjectHandler
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../QuantLib
)

target_link_libraries(QLADemo PRIVATE
    AddinCpp
    odbc32
    odbccp32
    Ws2_32
    advapi32
)
