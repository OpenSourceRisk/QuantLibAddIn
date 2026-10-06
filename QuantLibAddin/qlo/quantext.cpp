/* -*- mode: c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#if defined(HAVE_CONFIG_H)
		#include <qlo/config.hpp>
#endif

#include <qlo/quantext.hpp>

#include <qle/indexes/fxindex.hpp>

namespace QuantLibAddin {

    FxIndex::FxIndex(
            const boost::shared_ptr<ObjectHandler::ValueObject>& properties,
            const std::string& familyName,
            QuantLib::Natural fixingDays,
            const QuantLib::Currency& source,
            const QuantLib::Currency& target,
            const QuantLib::Calendar& fixingCalendar,
            const QuantLib::Handle<QuantLib::Quote>& fxSpot,
            const QuantLib::Handle<QuantLib::YieldTermStructure>& sourceYts,
            const QuantLib::Handle<QuantLib::YieldTermStructure>& targetYts,
            bool fixingTriangulation,
            bool permanent)
    : Index(properties, permanent)
    {
        libraryObject_ = boost::shared_ptr<QuantLib::Index>(
            new QuantExt::FxIndex(
                familyName,
                fixingDays,
                source,
                target,
                fixingCalendar,
                fxSpot,
                sourceYts,
                targetYts,
                fixingTriangulation));
    }

}
