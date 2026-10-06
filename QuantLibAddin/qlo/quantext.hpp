/* -*- mode: c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#ifndef qla_quantext_hpp
#define qla_quantext_hpp

#include <qlo/index.hpp>
#include <ql/currency.hpp>
#include <ql/handle.hpp>
#include <ql/quote.hpp>
#include <ql/termstructures/yieldtermstructure.hpp>
#include <ql/time/calendar.hpp>
#include <string>

namespace QuantLibAddin {

    class FxIndex : public Index {
      public:
        FxIndex(
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
            bool permanent);
      protected:
        OH_OBJ_CTOR(FxIndex, Index);
    };

}

#endif
