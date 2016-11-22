/**
 * \copyright
 * Copyright (c) 2012-2016, OpenGeoSys Community (http://www.opengeosys.org)
 *            Distributed under a Modified BSD License.
 *              See accompanying file LICENSE.txt or
 *              http://www.opengeosys.org/project/license
 *
 * \file   NonWettingPhaseVanGenuchten.cpp
 *
 * Created on November 2, 2016, 11:24 AM
 */

#include "NonWettingPhaseVanGenuchten.h"

#include <cmath>

#include "MathLib/MathTools.h"

namespace MaterialLib
{
namespace PorousMedium
{
double NonWettingPhaseVanGenuchten::getValue(const double saturation_w) const
{
    const double S = MathLib::limitValueInInterval(
        saturation_w, _Sr + _minor_offset, _Smax - _minor_offset);
    const double Se = (S - _Sr) / (_Smax - _Sr);
    const double Krel = std::pow(1.0 - Se, 1.0 / 3.0) *
                        std::pow(1.0 - std::pow(Se, 1.0 / _mm), 2.0 * _mm);
    return Krel < _Krel_min ? _Krel_min : Krel;
}
}  // end namespace
}  // end namespace
