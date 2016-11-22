/**
 * \copyright
 * Copyright (c) 2012-2016, OpenGeoSys Community (http://www.opengeosys.org)
 *            Distributed under a Modified BSD License.
 *              See accompanying file LICENSE.txt or
 *              http://www.opengeosys.org/project/license
 *
 * \file   WettingPhaseBrookCoreyOilGas.cpp
 *
 * Created on November 1, 2016, 3:37 PM
 */

#include "WettingPhaseBrookCoreyOilGas.h"

#include <cmath>

#include "MathLib/MathTools.h"

namespace MaterialLib
{
namespace PorousMedium
{
double WettingPhaseBrookCoreyOilGas::getValue(const double saturation) const
{
    const double S = MathLib::limitValueInInterval(
        saturation, _Sr + _perturbation, _Smax - _perturbation);
    const double Se = (S - _Sr) / (_Smax - _Sr);
    const double Krel = std::pow(Se, 3.0 + 2.0 / _mm);
    return Krel < _Krel_min ? _Krel_min : Krel;
}

}  // end namespace
}  // end namespace
