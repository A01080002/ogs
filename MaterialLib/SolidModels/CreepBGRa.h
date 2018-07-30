/**
 * \copyright
 * Copyright (c) 2012-2017, OpenGeoSys Community (http://www.opengeosys.org)
 *            Distributed under a Modified BSD License.
 *              See accompanying file LICENSE.txt or
 *              http://www.opengeosys.org/project/license
 *
 *  \file   CreepBGRa.h
 *  Created on July 6, 2018, 9:53 AM
 */

#pragma once

#include <memory>

#include "LinearElasticIsotropic.h"
#include "MathLib/KelvinVector.h"
#include "NumLib/NewtonRaphson.h"

namespace MaterialLib
{
namespace Solids
{
namespace Creep
{
/**
 * \Brief A class for computing the BGRa creep model, which reads
 * \f[
 * \dot \mathbf{\epsilon}^{cr}=\frac{3}{2}A \mathrm{e}^{-\frac{Q}{RT}}
 * \left(\dfrac{\sigma_{eff}}{sigma_0}\right)\dfrac{\mathbf{s}}{||\mathbf{s}||}
 * \f]
 * where $f\sigma_{eff}=\sqrt{\frac{3}{2}}||\mathbf{s}|$f, $fA, sigma_0, n, Q$f
 * are parameter, and $fR$f is the gas constant.
 *
 */
template <int DisplacementDim>
class CreepBGRa final : public LinearElasticIsotropic<DisplacementDim>
{
public:
    static int const KelvinVectorSize =
        MathLib::KelvinVector::KelvinVectorDimensions<DisplacementDim>::value;
    using ResidualVectorType = Eigen::Matrix<double, KelvinVectorSize, 1>;
    using JacobianMatrix = Eigen::Matrix<double, KelvinVectorSize,
                                         KelvinVectorSize, Eigen::RowMajor>;

    using KelvinVector =
        MathLib::KelvinVector::KelvinVectorType<DisplacementDim>;
    using KelvinMatrix =
        MathLib::KelvinVector::KelvinMatrixType<DisplacementDim>;

    std::unique_ptr<
        typename MechanicsBase<DisplacementDim>::MaterialStateVariables>
    createMaterialStateVariables() override
    {
        return LinearElasticIsotropic<
            DisplacementDim>::createMaterialStateVariables();
    }

    CreepBGRa(
        typename LinearElasticIsotropic<DisplacementDim>::MaterialProperties mp,
            NumLib::NewtonRaphsonSolverParameters nonlinear_solver_parameters,
        const double A, const double n, const double sigma0, const double Q)
        :  LinearElasticIsotropic<DisplacementDim>(std::move(mp)),
          _nonlinear_solver_parameters(std::move(nonlinear_solver_parameters)),
          _A(A),
          _n(n),
          _sigma0(sigma0),
          _Q(Q)
    {
    }

    boost::optional<
        std::tuple<KelvinVector, std::unique_ptr<typename MechanicsBase<
                                     DisplacementDim>::MaterialStateVariables>,
                   KelvinMatrix>>
    integrateStress(
        double const t, ProcessLib::SpatialPosition const& x, double const dt,
        KelvinVector const& eps_prev, KelvinVector const& eps,
        KelvinVector const& sigma_prev,
        typename MechanicsBase<DisplacementDim>::MaterialStateVariables const&
            material_state_variables, double const T, double const p) override;

private:
    NumLib::NewtonRaphsonSolverParameters const _nonlinear_solver_parameters;

    const double _A;
    const double _n;
    const double _sigma0;
    const double _Q;
};

extern template class CreepBGRa<2>;
extern template class CreepBGRa<3>;

}  // end of namespace Creep
}  // end of namespace Solids
}  // end of namespace CreepBGRa
