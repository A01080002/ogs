/**
 * \copyright
 * Copyright (c) 2012-2016, OpenGeoSys Community (http://www.opengeosys.org)
 *            Distributed under a Modified BSD License.
 *              See accompanying file LICENSE.txt or
 *              http://www.opengeosys.org/project/license
 *
 */

#pragma once

#include "SmallDeformationLocalAssemblerMatrixNearFracture.h"

namespace ProcessLib
{
namespace SmallDeformationWithLIE
{

template <typename ShapeFunction, typename IntegrationMethod,
          unsigned GlobalDim, int DisplacementDim>
class LocalAssemblerDataMatrixNearFracture final
    : public SmallDeformationLocalAssemblerMatrixNearFracture<ShapeFunction, IntegrationMethod,
                                            DisplacementDim>
{
public:
    LocalAssemblerDataMatrixNearFracture(LocalAssemblerDataMatrixNearFracture const&) = delete;
    LocalAssemblerDataMatrixNearFracture(LocalAssemblerDataMatrixNearFracture&&) = delete;

    LocalAssemblerDataMatrixNearFracture(
        MeshLib::Element const& e,
        std::size_t const n_variables,
        std::size_t const local_matrix_size,
        std::vector<unsigned> const& dofIndex_to_localIndex,
        bool is_axially_symmetric,
        unsigned const integration_order,
        SmallDeformationProcessData<DisplacementDim>& process_data)
        : SmallDeformationLocalAssemblerMatrixNearFracture<ShapeFunction, IntegrationMethod,
                                         DisplacementDim>(
              e, n_variables, local_matrix_size, dofIndex_to_localIndex,
              is_axially_symmetric, integration_order, process_data)
    {
    }
};

}  // namespace SmallDeformationWithLIE
}  // namespace ProcessLib

