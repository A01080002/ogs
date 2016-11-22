/**
 * \copyright
 * Copyright (c) 2012-2016, OpenGeoSys Community (http://www.opengeosys.org)
 *            Distributed under a Modified BSD License.
 *              See accompanying file LICENSE.txt or
 *              http://www.opengeosys.org/project/license
 *
 *  \file   ReletivePermeabilityCurve.h
 *
 * Created on November 2, 2016, 1:41 PM
 */

#ifndef OGS_RELETIVE_PERMEABILITY_CURVE_H
#define OGS_RELETIVE_PERMEABILITY_CURVE_H

#include <memory>
#include "RelativePermeability.h"

#include "MathLib/MathTools.h"
#include "MathLib/InterpolationAlgorithms/PiecewiseLinearInterpolation.h"

namespace MaterialLib
{
namespace PorousMedium
{
class ReletivePermeabilityCurve final : public RelativePermeability
{
public:
    ReletivePermeabilityCurve(
        std::unique_ptr<MathLib::PiecewiseLinearInterpolation>& curve_data)
        : _Sr(curve_data->getSupportMin()),
          _Smax(curve_data->getSupportMax()),
          _curve_data(std::move(curve_data))
    {
    }

    ReletivePermeabilityCurve(ReletivePermeabilityCurve& orig)
        : _Sr(orig._Sr),
          _Smax(orig._Smax),
          _curve_data(std::move(orig._curve_data))
    {
    }

    /// Get model name.
    std::string getName() const override
    {
        return "Relative permeability curve.";
    }

    /// Get relative permeability value.
    double getValue(const double saturation) const override
    {
        const double S = MathLib::limitValueInInterval(
            saturation, _Sr + _perturbation, _Smax - _perturbation);

        return _curve_data->getValue(S);
    }

private:
    const double _Sr;    ///< Residual saturation.
    const double _Smax;  ///< Maximum saturation.

    std::unique_ptr<MathLib::PiecewiseLinearInterpolation> _curve_data;

    const double _perturbation = std::numeric_limits<double>::epsilon();
};
}  // end namespace
}  // end namespace
#endif /* OGS_RELETIVE_PERMEABILITY_CURVE_H */
