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

}
