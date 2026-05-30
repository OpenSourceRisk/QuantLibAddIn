# ObjectHandler — builds three static libraries consumed by the XLL:
# Ensure Boost imported targets are available (find_package is idempotent).
if(NOT MSVC_LINK_DYNAMIC_RUNTIME)
    set(Boost_USE_STATIC_RUNTIME ON)
endif()
set(Boost_USE_STATIC_LIBS ON)
find_package(Boost 1.58.0 REQUIRED COMPONENTS filesystem serialization)


#   xlsdk          : Excel SDK (framewrk.cpp + xlcall32.lib)
#   ohlib          : ObjectHandler core (oh/*.cpp)
#   ohxllib        : ObjectHandler Excel layer (ohxl/**/*.cpp)

# -----------------------------------------------------------------------
# xlsdk
# -----------------------------------------------------------------------
add_library(xlsdk STATIC
    xlsdk/framewrk.cpp
)

target_include_directories(xlsdk PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

# Link against the pre-built xlcall32.lib that ships with the SDK.
# x64 copy lives in xlsdk/x64/; Win32 copy is xlsdk/xlcall32.lib.
target_link_libraries(xlsdk PUBLIC
    $<IF:$<EQUAL:${CMAKE_SIZEOF_VOID_P},8>,
        ${CMAKE_CURRENT_SOURCE_DIR}/xlsdk/x64/XLCALL32.LIB,
        ${CMAKE_CURRENT_SOURCE_DIR}/xlsdk/xlcall32.lib>
)

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

target_link_libraries(ohlib PUBLIC Boost::boost Boost::filesystem Boost::serialization)

# -----------------------------------------------------------------------
# ohxllib  (ObjectHandler Excel layer — depends on ohlib + xlsdk)
# -----------------------------------------------------------------------
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
