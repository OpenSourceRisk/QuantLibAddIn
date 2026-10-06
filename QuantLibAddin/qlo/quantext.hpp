/* -*- mode: c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#ifndef qla_quantext_hpp
#define qla_quantext_hpp

#include <qlo/index.hpp>
#include <qlo/leg.hpp>
#include <ql/currency.hpp>
#include <ql/handle.hpp>
#include <ql/quote.hpp>
#include <ql/termstructures/yieldtermstructure.hpp>
#include <ql/time/calendar.hpp>
#include <string>
#include <vector>

namespace QuantExt {
    class FxIndex;
}

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

    class FXLinkedCashFlowLeg : public Leg {
      public:
        FXLinkedCashFlowLeg(
            const boost::shared_ptr<ObjectHandler::ValueObject>& properties,
            const std::vector<QuantLib::Date>& cashFlowDates,
            const std::vector<QuantLib::Date>& fixingDates,
            const std::vector<QuantLib::Real>& foreignAmounts,
            const boost::shared_ptr<QuantExt::FxIndex>& fxIndex,
            bool permanent);
    };

}

#endif
