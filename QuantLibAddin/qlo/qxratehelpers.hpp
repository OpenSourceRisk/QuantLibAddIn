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
#include <ql/termstructures/bootstraphelper.hpp>

namespace QuantLib {
		class Quote;
		class IborIndex;
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

}

#endif
