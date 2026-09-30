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

/*! \file qxratehelpers.hpp
		\brief QuantLibAddin wrappers for QuantExt rate helpers.
*/

#ifndef qla_qxratehelpers_hpp
#define qla_qxratehelpers_hpp

#include <qlo/ratehelpers.hpp>

#include <ql/time/date.hpp>
#include <ql/time/period.hpp>
#include <ql/time/calendar.hpp>
#include <ql/time/daycounter.hpp>
#include <ql/time/businessdayconvention.hpp>
#include <ql/time/frequency.hpp>
#include <ql/time/dategenerationrule.hpp>
#include <ql/termstructures/bootstraphelper.hpp>

#include <qle/cashflows/subperiodscoupon.hpp>

namespace QuantLib {
		class Quote;
		class IborIndex;
		class OvernightIndex;
		class YieldTermStructure;
		template <class T>
		class Handle;
}

namespace QuantLibAddin {

		//! Wrapper for QuantExt::ImmFraRateHelper.
		/*! QuantExt::ImmFraRateHelper derives from QuantLib::RateHelper
				(BootstrapHelper<YieldTermStructure>), so it slots into the existing
				qlo::RateHelper object model unchanged.
		*/
		class ImmFraRateHelper : public RateHelper {
			public:
				ImmFraRateHelper(
						const boost::shared_ptr<ObjectHandler::ValueObject>& properties,
						const QuantLib::Handle<QuantLib::Quote>& rate,
						QuantLib::Size imm1,
						QuantLib::Size imm2,
						const boost::shared_ptr<QuantLib::IborIndex>& iborIndex,
						bool permanent);
		};

		//! Wrapper for QuantExt::OISRateHelper.
		class QxOISRateHelper : public RateHelper {
			public:
				QxOISRateHelper(
						const boost::shared_ptr<ObjectHandler::ValueObject>& properties,
						QuantLib::Natural settlementDays,
						const QuantLib::Period& swapTenor,
						const QuantLib::Handle<QuantLib::Quote>& fixedRate,
						const boost::shared_ptr<QuantLib::OvernightIndex>& overnightIndex,
						const QuantLib::DayCounter& fixedDayCounter,
						const QuantLib::Calendar& fixedCalendar,
						QuantLib::Natural paymentLag,
						QuantLib::Frequency paymentFrequency,
						QuantLib::BusinessDayConvention fixedConvention,
						QuantLib::BusinessDayConvention paymentAdjustment,
						QuantLib::DateGeneration::Rule rule,
						const QuantLib::Handle<QuantLib::YieldTermStructure>& discountingCurve,
						bool telescopicValueDates,
						bool permanent);
		};

		//! Wrapper for QuantExt::DatedOISRateHelper.
		class QxDatedOISRateHelper : public RateHelper {
			public:
				QxDatedOISRateHelper(
						const boost::shared_ptr<ObjectHandler::ValueObject>& properties,
						const QuantLib::Date& startDate,
						const QuantLib::Date& endDate,
						const QuantLib::Handle<QuantLib::Quote>& fixedRate,
						const boost::shared_ptr<QuantLib::OvernightIndex>& overnightIndex,
						const QuantLib::DayCounter& fixedDayCounter,
						const QuantLib::Calendar& fixedCalendar,
						QuantLib::Natural paymentLag,
						QuantLib::Frequency paymentFrequency,
						QuantLib::BusinessDayConvention fixedConvention,
						QuantLib::BusinessDayConvention paymentAdjustment,
						QuantLib::DateGeneration::Rule rule,
						const QuantLib::Handle<QuantLib::YieldTermStructure>& discountingCurve,
						bool telescopicValueDates,
						bool permanent);
		};

		//! Wrapper for QuantExt::AverageOISRateHelper.
		class AverageOISRateHelper : public RateHelper {
			public:
				AverageOISRateHelper(
						const boost::shared_ptr<ObjectHandler::ValueObject>& properties,
						const QuantLib::Handle<QuantLib::Quote>& fixedRate,
						const QuantLib::Period& spotLagTenor,
						const QuantLib::Period& swapTenor,
						const QuantLib::Period& fixedTenor,
						const QuantLib::DayCounter& fixedDayCounter,
						const QuantLib::Calendar& fixedCalendar,
						QuantLib::BusinessDayConvention fixedConvention,
						QuantLib::BusinessDayConvention fixedPaymentAdjustment,
						const boost::shared_ptr<QuantLib::OvernightIndex>& overnightIndex,
						const QuantLib::Period& onTenor,
						const QuantLib::Handle<QuantLib::Quote>& onSpread,
						QuantLib::Natural rateCutoff,
						const QuantLib::Handle<QuantLib::YieldTermStructure>& discountCurve,
						bool permanent);
		};

		//! Wrapper for QuantExt::BasisTwoSwapHelper.
		class BasisTwoSwapHelper : public RateHelper {
			public:
				BasisTwoSwapHelper(
						const boost::shared_ptr<ObjectHandler::ValueObject>& properties,
						const QuantLib::Handle<QuantLib::Quote>& spread,
						const QuantLib::Period& swapTenor,
						const QuantLib::Calendar& calendar,
						QuantLib::Frequency longFixedFrequency,
						QuantLib::BusinessDayConvention longFixedConvention,
						const QuantLib::DayCounter& longFixedDayCount,
						const boost::shared_ptr<QuantLib::IborIndex>& longIndex,
						QuantLib::Frequency shortFixedFrequency,
						QuantLib::BusinessDayConvention shortFixedConvention,
						const QuantLib::DayCounter& shortFixedDayCount,
						const boost::shared_ptr<QuantLib::IborIndex>& shortIndex,
						bool longMinusShort,
						const QuantLib::Handle<QuantLib::YieldTermStructure>& discountingCurve,
						bool permanent);
		};

		//! Wrapper for QuantExt::SubPeriodsSwapHelper.
		class SubPeriodsSwapHelper : public RateHelper {
			public:
				SubPeriodsSwapHelper(
						const boost::shared_ptr<ObjectHandler::ValueObject>& properties,
						const QuantLib::Handle<QuantLib::Quote>& spread,
						const QuantLib::Period& swapTenor,
						const QuantLib::Period& fixedTenor,
						const QuantLib::Calendar& fixedCalendar,
						const QuantLib::DayCounter& fixedDayCount,
						QuantLib::BusinessDayConvention fixedConvention,
						const QuantLib::Period& floatPayTenor,
						const boost::shared_ptr<QuantLib::IborIndex>& iborIndex,
						const QuantLib::DayCounter& floatDayCount,
						const QuantLib::Handle<QuantLib::YieldTermStructure>& discountingCurve,
						QuantExt::SubPeriodsCoupon1::Type type,
						bool permanent);
		};

		//! Wrapper for QuantExt::TenorBasisSwapHelper.
		class TenorBasisSwapHelper : public RateHelper {
			public:
				TenorBasisSwapHelper(
						const boost::shared_ptr<ObjectHandler::ValueObject>& properties,
						const QuantLib::Handle<QuantLib::Quote>& spread,
						const QuantLib::Period& swapTenor,
						const boost::shared_ptr<QuantLib::IborIndex>& payIndex,
						const boost::shared_ptr<QuantLib::IborIndex>& receiveIndex,
						const QuantLib::Handle<QuantLib::YieldTermStructure>& discountingCurve,
						bool spreadOnRec,
						bool includeSpread,
						const QuantLib::Period& payFrequency,
						const QuantLib::Period& recFrequency,
						bool telescopicValueDates,
						QuantExt::SubPeriodsCoupon1::Type type,
						bool permanent);
		};

}

#endif
