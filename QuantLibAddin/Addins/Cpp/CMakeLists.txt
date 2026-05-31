# QuantLibAddin/Addins/Cpp/CMakeLists.txt
# Builds the AddinCpp static library (the C++ addin layer).

add_library(AddinCpp STATIC
    abcd.cpp
    accountingengines.cpp
    alphaform.cpp
    assetswap.cpp
    bonds.cpp
    browniangenerators.cpp
    btp.cpp
    capfloor.cpp
    capletvolstructure.cpp
    cmsmarket.cpp
    correlation.cpp
    couponvectors.cpp
    ctsmmcapletcalibration.cpp
    curvestate.cpp
    evolutiondescription.cpp
    exercise.cpp
    forwardrateagreement.cpp
    garbagecollection.cpp
    handles.cpp
    index.cpp
    init.cpp
    instruments.cpp
    interpolation.cpp
    leg.cpp
    logging.cpp
    marketmodelevolvers.cpp
    marketmodels.cpp
    ohutils.cpp
    optimization.cpp
    options.cpp
    payoffs.cpp
    piecewiseyieldcurve.cpp
    pricingengines.cpp
    processes.cpp
    products.cpp
    quotes.cpp
    randomsequencegenerator.cpp
    rangeaccrual.cpp
    ratehelpers.cpp
    schedule.cpp
    sequencestatistics.cpp
    serialization.cpp
    settings.cpp
    shortratemodels.cpp
    smilesection.cpp
    statistics.cpp
    swap.cpp
    swaption.cpp
    swaptionvolstructure.cpp
    termstructures.cpp
    timeseries.cpp
    utilities.cpp
    valueobjects.cpp
    vanillaswap.cpp
    volatilities.cpp
    volatility.cpp
)

target_include_directories(AddinCpp PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../..>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../../ObjectHandler>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../../QuantLib>
)

target_compile_definitions(AddinCpp PRIVATE
    _LIB
)

target_link_libraries(AddinCpp PUBLIC
    QuantLibObjects
)
