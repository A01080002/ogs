/**
 * \copyright
 * Copyright (c) 2012-2017, OpenGeoSys Community (http://www.opengeosys.org)
 *            Distributed under a Modified BSD License.
 *              See accompanying file LICENSE.txt or
 *              http://www.opengeosys.org/project/license
 *
 * \file   StaggeredCouplingTerm.cpp
 *
 * Created on November 7, 2016, 12:14 PM
 */

#include "StaggeredCouplingTerm.h"
#include "Process.h"

namespace ProcessLib
{
const StaggeredCouplingTerm createVoidStaggeredCouplingTerm()
{
    std::unordered_map<std::type_index, Process const&> coupled_processes;
    std::unordered_map<std::type_index, GlobalVector const&> coupled_xs;
    const bool empty = true;
    return StaggeredCouplingTerm(coupled_processes, coupled_xs, 0.0, empty);
}

std::unordered_map<std::type_index, const std::vector<double>>
getCurrentLocalSolutionsOfCoupledProcesses(
    const std::unordered_map<std::type_index, GlobalVector const&>&
        global_coupled_xs,
    const std::vector<GlobalIndexType>& indices)
{
    std::unordered_map<std::type_index, const std::vector<double>>
        local_coupled_xs;

    // Get local nodal solutions of the coupled equations.
    for (auto const& global_coupled_x_pair : global_coupled_xs)
    {
        auto const& coupled_x = global_coupled_x_pair.second;
        auto const local_coupled_x = coupled_x.get(indices);
        BaseLib::insertIfTypeIndexKeyUniqueElseError(
            local_coupled_xs, global_coupled_x_pair.first, local_coupled_x,
            "local_coupled_x");
    }
    return local_coupled_xs;
}

}  // end of ProcessLib
