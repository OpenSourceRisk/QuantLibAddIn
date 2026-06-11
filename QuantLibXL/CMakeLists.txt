# QuantLibXL/CMakeLists.txt
# Builds the QuantLibXL Excel XLL (a DLL with .xll extension).
#
# Output naming mirrors Build A:
#   QuantLibXL-v<toolset>-x64-mt[-s|-gd|-sgd]-1_42_0.xll
#
# The toolset tag, platform tag and runtime tag are computed from cmake
# variables so that the name matches the hand-maintained VS project output.
#
# The XLL itself is Windows-only, so the whole library build is guarded by
# if(WIN32).  The QuantLibXL *documentation* (added at the bottom of this file)
# builds on any platform, so it is intentionally placed outside that guard.

if(WIN32)

# -----------------------------------------------------------------------
# Toolset tag  (e.g. v145 for VS 2026, v143 for VS 2022)
# -----------------------------------------------------------------------
if(MSVC)
    # MSVC_TOOLSET_VERSION is e.g. 145, 143, 142 ...
    set(QLXL_TOOLSET_TAG "v${MSVC_TOOLSET_VERSION}")
else()
    set(QLXL_TOOLSET_TAG "")
endif()

# -----------------------------------------------------------------------
# Platform tag
# -----------------------------------------------------------------------
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(QLXL_PLATFORM_TAG "-x64")
else()
    set(QLXL_PLATFORM_TAG "")
endif()

# -----------------------------------------------------------------------
# Runtime/build-type tag — one suffix per configuration.
# Dynamic runtime:  Release → -mt        Debug → -mt-gd
# Static runtime:   Release → -mt-s      Debug → -mt-sgd
#
# Whether we link dynamic or static runtime is controlled by the cmake
# cache variable MSVC_LINK_DYNAMIC_RUNTIME (set in commonSettings.cmake).
# -----------------------------------------------------------------------
if(MSVC_LINK_DYNAMIC_RUNTIME)
    set(QLXL_RELEASE_RUNTIME_TAG "-mt")
    set(QLXL_DEBUG_RUNTIME_TAG   "-mt-gd")
else()
    set(QLXL_RELEASE_RUNTIME_TAG "-mt-s")
    set(QLXL_DEBUG_RUNTIME_TAG   "-mt-sgd")
endif()

# -----------------------------------------------------------------------
# Version tag
# -----------------------------------------------------------------------
set(QLXL_VERSION_TAG "1_42_0")

# -----------------------------------------------------------------------
# Compose the base output name (without config suffix)
# The generator-expression below selects the right runtime tag at build time.
# -----------------------------------------------------------------------
set(QLXL_BASE_NAME
    "QuantLibXL-${QLXL_TOOLSET_TAG}${QLXL_PLATFORM_TAG}"
)

# -----------------------------------------------------------------------
# Source files (from QuantLibXLStatic2.vcxproj)
# -----------------------------------------------------------------------
set(QLXL_SRC
    qlxl/register/addin.cpp
    qlxl/register/register_abcd.cpp
    qlxl/register/register_accountingengines.cpp
    qlxl/register/register_all.cpp
    qlxl/register/register_alphaform.cpp
    qlxl/register/register_assetswap.cpp
    qlxl/register/register_basketlossmodels.cpp
    qlxl/register/register_bonds.cpp
    qlxl/register/register_browniangenerators.cpp
    qlxl/register/register_btp.cpp
    qlxl/register/register_calendar.cpp
    qlxl/register/register_calibrationhelpers.cpp
    qlxl/register/register_capfloor.cpp
    qlxl/register/register_capletvolstructure.cpp
    qlxl/register/register_cmsmarket.cpp
    qlxl/register/register_cmsmarketcalibration.cpp
    qlxl/register/register_correlation.cpp
    qlxl/register/register_couponvectors.cpp
    qlxl/register/register_credit.cpp
    qlxl/register/register_ctsmmcapletcalibration.cpp
    qlxl/register/register_curvestate.cpp
    qlxl/register/register_date.cpp
    qlxl/register/register_daycounter.cpp
    qlxl/register/register_defaultbasket.cpp
    qlxl/register/register_defaulttermstructures.cpp
    qlxl/register/register_driftcalculators.cpp
    qlxl/register/register_evolutiondescription.cpp
    qlxl/register/register_exercise.cpp
    qlxl/register/register_forwardrateagreement.cpp
    qlxl/register/register_fxforward.cpp
    qlxl/register/register_handles.cpp
    qlxl/register/register_index.cpp
    qlxl/register/register_instruments.cpp
    qlxl/register/register_interpolation.cpp
    qlxl/register/register_latentmodels.cpp
    qlxl/register/register_leg.cpp
    qlxl/register/register_marketmodelevolvers.cpp
    qlxl/register/register_marketmodels.cpp
    qlxl/register/register_mathf.cpp
    qlxl/register/register_optimization.cpp
    qlxl/register/register_options.cpp
    qlxl/register/register_overnightindexedswap.cpp
    qlxl/register/register_payoffs.cpp
    qlxl/register/register_piecewiseyieldcurve.cpp
    qlxl/register/register_prices.cpp
    qlxl/register/register_pricingengines.cpp
    qlxl/register/register_processes.cpp
    qlxl/register/register_products.cpp
    qlxl/register/register_quotes.cpp
    qlxl/register/register_randomsequencegenerator.cpp
    qlxl/register/register_rangeaccrual.cpp
    qlxl/register/register_ratehelpers.cpp
    qlxl/register/register_schedule.cpp
    qlxl/register/register_sequencestatistics.cpp
    qlxl/register/register_settings.cpp
    qlxl/register/register_shortratemodels.cpp
    qlxl/register/register_smilesection.cpp
    qlxl/register/register_statistics.cpp
    qlxl/register/register_swap.cpp
    qlxl/register/register_swaption.cpp
    qlxl/register/register_swaptionvolstructure.cpp
    qlxl/register/register_termstructures.cpp
    qlxl/register/register_timeseries.cpp
    qlxl/register/register_utilities.cpp
    qlxl/register/register_vanillaswap.cpp
    qlxl/register/register_volatilities.cpp
    qlxl/register/register_volatility.cpp
    qlxl/functions/abcd.cpp
    qlxl/functions/accountingengines.cpp
    qlxl/functions/alphaform.cpp
    qlxl/functions/assetswap.cpp
    qlxl/functions/basketlossmodels.cpp
    qlxl/functions/bonds.cpp
    qlxl/functions/browniangenerators.cpp
    qlxl/functions/btp.cpp
    qlxl/functions/calendar.cpp
    qlxl/functions/calibrationhelpers.cpp
    qlxl/functions/capfloor.cpp
    qlxl/functions/capletvolstructure.cpp
    qlxl/functions/cmsmarket.cpp
    qlxl/functions/cmsmarketcalibration.cpp
    qlxl/functions/correlation.cpp
    qlxl/functions/couponvectors.cpp
    qlxl/functions/credit.cpp
    qlxl/functions/ctsmmcapletcalibration.cpp
    qlxl/functions/curvestate.cpp
    qlxl/functions/date.cpp
    qlxl/functions/daycounter.cpp
    qlxl/functions/defaultbasket.cpp
    qlxl/functions/defaulttermstructures.cpp
    qlxl/functions/driftcalculators.cpp
    qlxl/functions/evolutiondescription.cpp
    qlxl/functions/exercise.cpp
    qlxl/functions/forwardrateagreement.cpp
    qlxl/functions/fxforward.cpp
    qlxl/functions/handles.cpp
    qlxl/functions/index.cpp
    qlxl/functions/instruments.cpp
    qlxl/functions/interpolation.cpp
    qlxl/functions/latentmodels.cpp
    qlxl/functions/leg.cpp
    qlxl/functions/manual.cpp
    qlxl/functions/marketmodelevolvers.cpp
    qlxl/functions/marketmodels.cpp
    qlxl/functions/mathf.cpp
    qlxl/functions/optimization.cpp
    qlxl/functions/options.cpp
    qlxl/functions/overnightindexedswap.cpp
    qlxl/functions/payoffs.cpp
    qlxl/functions/piecewiseyieldcurve.cpp
    qlxl/functions/prices.cpp
    qlxl/functions/pricingengines.cpp
    qlxl/functions/processes.cpp
    qlxl/functions/products.cpp
    qlxl/functions/quotes.cpp
    qlxl/functions/randomsequencegenerator.cpp
    qlxl/functions/rangeaccrual.cpp
    qlxl/functions/ratehelpers.cpp
    qlxl/functions/schedule.cpp
    qlxl/functions/sequencestatistics.cpp
    qlxl/functions/settings.cpp
    qlxl/functions/shortratemodels.cpp
    qlxl/functions/smilesection.cpp
    qlxl/functions/statistics.cpp
    qlxl/functions/swap.cpp
    qlxl/functions/swaption.cpp
    qlxl/functions/swaptionvolstructure.cpp
    qlxl/functions/termstructures.cpp
    qlxl/functions/timeseries.cpp
    qlxl/functions/utilities.cpp
    qlxl/functions/vanillaswap.cpp
    qlxl/functions/volatilities.cpp
    qlxl/functions/volatility.cpp
    qlxl/conversions/matrixtooper.cpp
    qlxl/conversions/opertomatrix.cpp
    qlxl/conversions/opertovector.cpp
    qlxl/conversions/scalartooper.cpp
    qlxl/session.cpp
)

# -----------------------------------------------------------------------
# The XLL target — a Windows DLL renamed to .xll
# -----------------------------------------------------------------------
add_library(QuantLibXL SHARED ${QLXL_SRC})

# Output filename: QuantLibXL-v145-x64-mt-s-1_42_0  (no lib prefix on Windows)
# We use generator expressions to select the runtime tag per config.
set_target_properties(QuantLibXL PROPERTIES
    PREFIX ""
    OUTPUT_NAME
        "${QLXL_BASE_NAME}$<$<CONFIG:Debug>:${QLXL_DEBUG_RUNTIME_TAG}>$<$<NOT:$<CONFIG:Debug>>:${QLXL_RELEASE_RUNTIME_TAG}>-${QLXL_VERSION_TAG}"
    SUFFIX ".xll"
    RUNTIME_OUTPUT_DIRECTORY             "${CMAKE_BINARY_DIR}/xll"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE     "${CMAKE_BINARY_DIR}/xll"
    RUNTIME_OUTPUT_DIRECTORY_DEBUG       "${CMAKE_BINARY_DIR}/xll"
    RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO "${CMAKE_BINARY_DIR}/xll"
    RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL  "${CMAKE_BINARY_DIR}/xll"
)

target_include_directories(QuantLibXL PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../ObjectHandler
    ${CMAKE_CURRENT_SOURCE_DIR}/../QuantLib
    ${CMAKE_CURRENT_SOURCE_DIR}/../QuantLibAddin
)

target_compile_definitions(QuantLibXL PRIVATE
    _USRDLL
    XLL_STATIC
)

# Explicitly disable Whole Program Optimization (/GL) on the XLL.
# If /GL were active, the LTCG-restart link pass would attempt to resolve
# project-reference libs by a mangled name derived from the XLL output
# name, causing LNK1104 for the dependency libs.
if(MSVC)
    target_compile_options(QuantLibXL PRIVATE /GL-)
endif()

target_link_libraries(QuantLibXL PRIVATE
    QuantLibObjects
    ohxllib
    odbc32
    odbccp32
    Ws2_32
    advapi32
)

# Suppress safe-exception-handler warning for x64 (no /SAFESEH on x64)
if(MSVC AND CMAKE_SIZEOF_VOID_P EQUAL 8)
    target_link_options(QuantLibXL PRIVATE /SAFESEH:NO)
endif()

endif() # WIN32 — end of the Windows-only XLL library build

# -----------------------------------------------------------------------
# Documentation — QuantLibXL owns its own Docs build (QuantLibXL-docs target).
# Unlike the XLL library above, the documentation builds on any platform, so it
# lives outside the if(WIN32) guard.  Gated on BUILD_DOCS so the integrated
# build only adds it when requested.  The option is (re)declared here so that
# this value is also honoured when QuantLibXL is configured on its own.
# -----------------------------------------------------------------------
option(BUILD_DOCS "Add targets to build the HTML documentation with Doxygen" OFF)
if(BUILD_DOCS)
    add_subdirectory(Docs)
endif()
