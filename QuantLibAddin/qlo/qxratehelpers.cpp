/* -*- mode: c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*
 Copyright (C) 2025 Eric Ehlers

 This file is part of QuantLib, a free-software/open-source library
 for financial quantitative analysts and developers - https://www.quantlib.org/

 QuantLib is free software: you can redistribute it and/or modify it
 under the terms of the QuantLib license.  You should have received a
 copy of the license along with this program; if not, please email
 <quantlib-dev@lists.sf.net>. The license is also available online at
 <https://www.quantlib.org/license.shtml>.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the license for more details.
*/

#ifdef HAVE_CONFIG_H
#include <qlo/config.hpp>
#endif

#include <qlo/qxratehelpers.hpp>

#include <ql/indexes/iborindex.hpp>

#include <qle/termstructures/immfraratehelper.hpp>
#include <qle/termstructures/oisratehelper.hpp>
#include <qle/termstructures/averageoisratehelper.hpp>
#include <qle/termstructures/basistwoswaphelper.hpp>
#include <qle/termstructures/subperiodsswaphelper.hpp>
#include <qle/termstructures/tenorbasisswaphelper.hpp>

#include <oh/repository.hpp>

using ObjectHandler::ValueObject;
using ObjectHandler::convert2;
using boost::shared_ptr;

namespace QuantLibAddin {

		// Coerce the associated Rate object's ID into a string (empty on failure),
		// mirroring the helper of the same purpose in ratehelpers.cpp.
		namespace {
				std::string quoteId(const ObjectHandler::property_t& p) {
						try {
								return convert2<std::string>(p);
						} catch (...) {
								return std::string();
						}
				}
		}

		ImmFraRateHelper::ImmFraRateHelper(
						const shared_ptr<ValueObject>& properties,
						const QuantLib::Handle<QuantLib::Quote>& rate,
						QuantLib::Size imm1,
						QuantLib::Size imm2,
						const shared_ptr<QuantLib::IborIndex>& iborIndex,
						bool permanent)
		: RateHelper(properties, permanent) {
				libraryObject_ = shared_ptr<QuantLib::RateHelper>(new
						QuantExt::ImmFraRateHelper(rate, imm1, imm2, iborIndex));
				quoteName_ = quoteId(properties->getSystemProperty("Rate"));
		}

		QxOISRateHelper::QxOISRateHelper(
						const shared_ptr<ValueObject>& properties,
						QuantLib::Natural settlementDays,
						const QuantLib::Period& swapTenor,
						const QuantLib::Handle<QuantLib::Quote>& fixedRate,
						const shared_ptr<QuantLib::OvernightIndex>& overnightIndex,
						const QuantLib::DayCounter& fixedDayCounter,
						const QuantLib::Calendar& fixedCalendar,
						QuantLib::Natural paymentLag,
						QuantLib::Frequency paymentFrequency,
						QuantLib::BusinessDayConvention fixedConvention,
						QuantLib::BusinessDayConvention paymentAdjustment,
						QuantLib::DateGeneration::Rule rule,
						const QuantLib::Handle<QuantLib::YieldTermStructure>& discountingCurve,
						bool telescopicValueDates,
						bool permanent)
		: RateHelper(properties, permanent) {
				libraryObject_ = shared_ptr<QuantLib::RateHelper>(new
						QuantExt::OISRateHelper(settlementDays, swapTenor, fixedRate,
								overnightIndex, overnightIndex != nullptr,
								fixedDayCounter, fixedCalendar, paymentLag,
								false, paymentFrequency, fixedConvention,
								paymentAdjustment, rule, discountingCurve,
								!discountingCurve.empty(), telescopicValueDates));
				quoteName_ = quoteId(properties->getSystemProperty("FixedRate"));
		}

		QxDatedOISRateHelper::QxDatedOISRateHelper(
						const shared_ptr<ValueObject>& properties,
						const QuantLib::Date& startDate,
						const QuantLib::Date& endDate,
						const QuantLib::Handle<QuantLib::Quote>& fixedRate,
						const shared_ptr<QuantLib::OvernightIndex>& overnightIndex,
						const QuantLib::DayCounter& fixedDayCounter,
						const QuantLib::Calendar& fixedCalendar,
						QuantLib::Natural paymentLag,
						QuantLib::Frequency paymentFrequency,
						QuantLib::BusinessDayConvention fixedConvention,
						QuantLib::BusinessDayConvention paymentAdjustment,
						QuantLib::DateGeneration::Rule rule,
						const QuantLib::Handle<QuantLib::YieldTermStructure>& discountingCurve,
						bool telescopicValueDates,
						bool permanent)
		: RateHelper(properties, permanent) {
				libraryObject_ = shared_ptr<QuantLib::RateHelper>(new
						QuantExt::DatedOISRateHelper(startDate, endDate, fixedRate,
								overnightIndex, overnightIndex != nullptr,
								fixedDayCounter, fixedCalendar, paymentLag,
								paymentFrequency, fixedConvention, paymentAdjustment,
								rule, discountingCurve, !discountingCurve.empty(),
								telescopicValueDates));
				quoteName_ = quoteId(properties->getSystemProperty("FixedRate"));
		}

		AverageOISRateHelper::AverageOISRateHelper(
						const shared_ptr<ValueObject>& properties,
						const QuantLib::Handle<QuantLib::Quote>& fixedRate,
						const QuantLib::Period& spotLagTenor,
						const QuantLib::Period& swapTenor,
						const QuantLib::Period& fixedTenor,
						const QuantLib::DayCounter& fixedDayCounter,
						const QuantLib::Calendar& fixedCalendar,
						QuantLib::BusinessDayConvention fixedConvention,
						QuantLib::BusinessDayConvention fixedPaymentAdjustment,
						const shared_ptr<QuantLib::OvernightIndex>& overnightIndex,
						const QuantLib::Period& onTenor,
						const QuantLib::Handle<QuantLib::Quote>& onSpread,
						QuantLib::Natural rateCutoff,
						const QuantLib::Handle<QuantLib::YieldTermStructure>& discountCurve,
						bool permanent)
		: RateHelper(properties, permanent) {
				libraryObject_ = shared_ptr<QuantLib::RateHelper>(new
						QuantExt::AverageOISRateHelper(fixedRate, spotLagTenor, swapTenor,
								fixedTenor, fixedDayCounter, fixedCalendar, fixedConvention,
								fixedPaymentAdjustment, overnightIndex,
								overnightIndex != nullptr, onTenor, onSpread, rateCutoff,
								discountCurve, !discountCurve.empty()));
				quoteName_ = quoteId(properties->getSystemProperty("FixedRate"));
		}

		BasisTwoSwapHelper::BasisTwoSwapHelper(
						const shared_ptr<ValueObject>& properties,
						const QuantLib::Handle<QuantLib::Quote>& spread,
						const QuantLib::Period& swapTenor,
						const QuantLib::Calendar& calendar,
						QuantLib::Frequency longFixedFrequency,
						QuantLib::BusinessDayConvention longFixedConvention,
						const QuantLib::DayCounter& longFixedDayCount,
						const shared_ptr<QuantLib::IborIndex>& longIndex,
						QuantLib::Frequency shortFixedFrequency,
						QuantLib::BusinessDayConvention shortFixedConvention,
						const QuantLib::DayCounter& shortFixedDayCount,
						const shared_ptr<QuantLib::IborIndex>& shortIndex,
						bool longMinusShort,
						const QuantLib::Handle<QuantLib::YieldTermStructure>& discountingCurve,
						bool permanent)
		: RateHelper(properties, permanent) {
				libraryObject_ = shared_ptr<QuantLib::RateHelper>(new
						QuantExt::BasisTwoSwapHelper(spread, swapTenor, calendar,
								longFixedFrequency, longFixedConvention, longFixedDayCount,
								longIndex, longIndex != nullptr,
								shortFixedFrequency, shortFixedConvention, shortFixedDayCount,
								shortIndex, longMinusShort, shortIndex != nullptr,
								discountingCurve, !discountingCurve.empty()));
				quoteName_ = quoteId(properties->getSystemProperty("Spread"));
		}

		SubPeriodsSwapHelper::SubPeriodsSwapHelper(
						const shared_ptr<ValueObject>& properties,
						const QuantLib::Handle<QuantLib::Quote>& spread,
						const QuantLib::Period& swapTenor,
						const QuantLib::Period& fixedTenor,
						const QuantLib::Calendar& fixedCalendar,
						const QuantLib::DayCounter& fixedDayCount,
						QuantLib::BusinessDayConvention fixedConvention,
						const QuantLib::Period& floatPayTenor,
						const shared_ptr<QuantLib::IborIndex>& iborIndex,
						const QuantLib::DayCounter& floatDayCount,
						const QuantLib::Handle<QuantLib::YieldTermStructure>& discountingCurve,
						QuantExt::SubPeriodsCoupon1::Type type,
						bool permanent)
		: RateHelper(properties, permanent) {
				libraryObject_ = shared_ptr<QuantLib::RateHelper>(new
						QuantExt::SubPeriodsSwapHelper(spread, swapTenor, fixedTenor,
								fixedCalendar, fixedDayCount, fixedConvention, floatPayTenor,
								iborIndex, floatDayCount, discountingCurve, type));
				quoteName_ = quoteId(properties->getSystemProperty("Spread"));
		}

		TenorBasisSwapHelper::TenorBasisSwapHelper(
						const shared_ptr<ValueObject>& properties,
						const QuantLib::Handle<QuantLib::Quote>& spread,
						const QuantLib::Period& swapTenor,
						const shared_ptr<QuantLib::IborIndex>& payIndex,
						const shared_ptr<QuantLib::IborIndex>& receiveIndex,
						const QuantLib::Handle<QuantLib::YieldTermStructure>& discountingCurve,
						bool spreadOnRec,
						bool includeSpread,
						const QuantLib::Period& payFrequency,
						const QuantLib::Period& recFrequency,
						bool telescopicValueDates,
						QuantExt::SubPeriodsCoupon1::Type type,
						bool permanent)
		: RateHelper(properties, permanent) {
				libraryObject_ = shared_ptr<QuantLib::RateHelper>(new
						QuantExt::TenorBasisSwapHelper(spread, swapTenor, payIndex,
								receiveIndex, discountingCurve, payIndex != nullptr,
								receiveIndex != nullptr, !discountingCurve.empty(),
								spreadOnRec, includeSpread, payFrequency, recFrequency,
								telescopicValueDates, type));
				quoteName_ = quoteId(properties->getSystemProperty("Spread"));
		}

}
