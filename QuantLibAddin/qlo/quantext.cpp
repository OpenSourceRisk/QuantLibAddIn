/* -*- mode: c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#if defined(HAVE_CONFIG_H)
		#include <qlo/config.hpp>
#endif

#include <qlo/quantext.hpp>

#include <qle/indexes/fxindex.hpp>
#include <qle/cashflows/fxlinkedcashflow.hpp>
#include <qle/cashflows/floatingratefxlinkednotionalcoupon.hpp>
#include <qle/instruments/currencyswap.hpp>

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

    FloatingRateFXLinkedNotionalLeg::FloatingRateFXLinkedNotionalLeg(
            const boost::shared_ptr<ObjectHandler::ValueObject>& properties,
            const QuantLib::Leg& underlying,
            const std::vector<QuantLib::Date>& fxFixingDates,
            QuantLib::Real foreignAmount,
            const boost::shared_ptr<QuantExt::FxIndex>& fxIndex,
            bool permanent)
    : Leg(properties, permanent)
    {
        QL_REQUIRE(underlying.size() == fxFixingDates.size(),
                   "Underlying leg size (" << underlying.size() <<
                   ") and FxFixingDates size (" << fxFixingDates.size() <<
                   ") must be equal");

        libraryObject_ = boost::shared_ptr<QuantLib::Leg>(new QuantLib::Leg());
        for (QuantLib::Size i = 0; i < underlying.size(); ++i) {
            boost::shared_ptr<QuantLib::FloatingRateCoupon> coupon =
                boost::dynamic_pointer_cast<QuantLib::FloatingRateCoupon>(underlying[i]);
            QL_REQUIRE(coupon, "underlying leg cashflow #" << i <<
                       " is not a floating rate coupon");
            libraryObject_->push_back(boost::shared_ptr<QuantLib::CashFlow>(
                new QuantExt::FloatingRateFXLinkedNotionalCoupon(
                    fxFixingDates[i], foreignAmount, fxIndex, coupon)));
        }
    }

    CurrencySwap::CurrencySwap(
            const boost::shared_ptr<ObjectHandler::ValueObject>& properties,
            const std::vector<boost::shared_ptr<Leg> >& legPtrs,
            const std::vector<bool>& payer,
            const std::vector<QuantLib::Currency>& currencies,
            bool isPhysicallySettled,
            bool isResettable,
            bool permanent)
    : Instrument(properties, permanent)
    {
        QL_REQUIRE(legPtrs.size() == payer.size() &&
                   legPtrs.size() == currencies.size(),
                   "Legs (" << legPtrs.size() <<
                   "), Payer (" << payer.size() <<
                   ") and Currencies (" << currencies.size() <<
                   ") must have the same size");

        std::vector<QuantLib::Leg> legs(legPtrs.size());
        boost::shared_ptr<QuantLib::Leg> qlLeg;
        for (QuantLib::Size i = 0; i < legPtrs.size(); ++i) {
            legPtrs[i]->getLibraryObject(qlLeg);
            legs[i] = *qlLeg;
        }

        libraryObject_ = boost::shared_ptr<QuantLib::Instrument>(
            new QuantExt::CurrencySwap(
                legs, payer, currencies, isPhysicallySettled, isResettable));
    }

}
