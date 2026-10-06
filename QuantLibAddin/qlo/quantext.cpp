/* -*- mode: c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#if defined(HAVE_CONFIG_H)
		#include <qlo/config.hpp>
#endif

#include <qlo/quantext.hpp>

#include <qle/indexes/fxindex.hpp>
#include <qle/cashflows/fxlinkedcashflow.hpp>

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

    FXLinkedCashFlowLeg::FXLinkedCashFlowLeg(
            const boost::shared_ptr<ObjectHandler::ValueObject>& properties,
            const std::vector<QuantLib::Date>& cashFlowDates,
            const std::vector<QuantLib::Date>& fixingDates,
            const std::vector<QuantLib::Real>& foreignAmounts,
            const boost::shared_ptr<QuantExt::FxIndex>& fxIndex,
            bool permanent)
    : Leg(properties, permanent)
    {
        QL_REQUIRE(cashFlowDates.size() == fixingDates.size() &&
                   cashFlowDates.size() == foreignAmounts.size(),
                   "CashFlowDates (" << cashFlowDates.size() <<
                   "), FixingDates (" << fixingDates.size() <<
                   ") and ForeignAmounts (" << foreignAmounts.size() <<
                   ") must have the same size");

        libraryObject_ = boost::shared_ptr<QuantLib::Leg>(new QuantLib::Leg());
        for (QuantLib::Size i = 0; i < cashFlowDates.size(); ++i)
            libraryObject_->push_back(boost::shared_ptr<QuantLib::CashFlow>(
                new QuantExt::FXLinkedCashFlow(
                    cashFlowDates[i], fixingDates[i], foreignAmounts[i], fxIndex)));
    }

}
