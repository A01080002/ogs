/**
 * \copyright
 * Copyright (c) 2012-2016, OpenGeoSys Community (http://www.opengeosys.org)
 *            Distributed under a Modified BSD License.
 *              See accompanying file LICENSE.txt or
 *              http://www.opengeosys.org/project/license
 */

#include "MeshUtils.h"

#include "MeshLib/MeshSearch/NodeSearch.h"

namespace ProcessLib
{
namespace SmallDeformationWithLIE
{

namespace
{

// A class to check whether a node is located on a crack tip with
// the following conditions:
// - the number of connected fracture elements is one
// - the node is not located on a domain boundary
class IsCrackTip
{
public:
    explicit IsCrackTip(MeshLib::Mesh const& mesh)
        : _fracture_element_dim(mesh.getDimension()-1)
    {
        _is_internal_node.resize(mesh.getNumberOfNodes(), true);

        MeshLib::NodeSearch nodeSearch(mesh);
        nodeSearch.searchBoundaryNodes();
        for (auto i : nodeSearch.getSearchedNodeIDs())
            _is_internal_node[i] = false;
    }

    bool operator()(MeshLib::Node const& node) const
    {
        if (!_is_internal_node[node.getID()])
            return false;

        unsigned n_connected_fracture_elements = 0;
        for (MeshLib::Element const* e : node.getElements())
            if (e->getDimension() == _fracture_element_dim)
                n_connected_fracture_elements++;
        assert(n_connected_fracture_elements>0);

        return (n_connected_fracture_elements == 1);
    }

private:
    unsigned const _fracture_element_dim;
    std::vector<bool> _is_internal_node;
};


} // no named namespace


void getFractureMatrixDataInMesh(
        MeshLib::Mesh const& mesh,
        std::vector<MeshLib::Element*>& vec_matrix_elements,
        std::vector<int>& vec_fracture_mat_IDs,
        std::vector<std::vector<MeshLib::Element*>>& vec_fracture_elements,
        std::vector<std::vector<MeshLib::Element*>>& vec_fracture_matrix_elements,
        std::vector<std::vector<MeshLib::Node*>>& vec_fracture_nodes
        )
{
    IsCrackTip isCrackTip(mesh);

    // get vectors of matrix elements and fracture elements
    vec_matrix_elements.reserve(mesh.getNumberOfElements());
    std::vector<MeshLib::Element*> all_fracture_elements;
    for (MeshLib::Element* e : mesh.getElements())
    {
        if (e->getDimension() == mesh.getDimension())
            vec_matrix_elements.push_back(e);
        else
            all_fracture_elements.push_back(e);
    }
    DBUG("-> found total %d matrix elements and %d fracture elements",
         vec_matrix_elements.size(), all_fracture_elements.size());

    // get fracture material IDs
    auto opt_material_ids(mesh.getProperties().getPropertyVector<int>("MaterialIDs"));
    if (!opt_material_ids)
        OGS_FATAL("MaterialIDs propery vector not found in a mesh");
    for (MeshLib::Element* e : all_fracture_elements)
        vec_fracture_mat_IDs.push_back((*opt_material_ids)[e->getID()]);
    std::sort(vec_fracture_mat_IDs.begin(), vec_fracture_mat_IDs.end());
    vec_fracture_mat_IDs.erase(
        std::unique(vec_fracture_mat_IDs.begin(), vec_fracture_mat_IDs.end()),
        vec_fracture_mat_IDs.end());
    DBUG("-> found %d fracture material groups", vec_fracture_mat_IDs.size());

    // create a vector of fracture elements for each group
    vec_fracture_elements.resize(vec_fracture_mat_IDs.size());
    for (unsigned frac_id=0; frac_id<vec_fracture_mat_IDs.size(); frac_id++)
    {
        const auto frac_mat_id = vec_fracture_mat_IDs[frac_id];
        std::vector<MeshLib::Element*> &vec_elements = vec_fracture_elements[frac_id];
        for (MeshLib::Element* e : all_fracture_elements)
        {
            if ((*opt_material_ids)[e->getID()] == frac_mat_id)
                vec_elements.push_back(e);
        }
        DBUG("-> found %d elements on the fracture %d", vec_elements.size(), frac_id);
    }

    // get a vector of fracture nodes
    vec_fracture_nodes.resize(vec_fracture_mat_IDs.size());
    for (unsigned frac_id=0; frac_id<vec_fracture_mat_IDs.size(); frac_id++)
    {
        std::vector<MeshLib::Node*> &vec_nodes = vec_fracture_nodes[frac_id];
        for (MeshLib::Element* e : vec_fracture_elements[frac_id])
        {
            for (unsigned i=0; i<e->getNumberOfNodes(); i++)
            {
                if (isCrackTip(*e->getNode(i)))
                    continue;
                vec_nodes.push_back(const_cast<MeshLib::Node*>(e->getNode(i)));
            }
        }
        std::sort(vec_nodes.begin(), vec_nodes.end(),
            [](MeshLib::Node* node1, MeshLib::Node* node2) { return (node1->getID() < node2->getID()); }
            );
        vec_nodes.erase(std::unique(vec_nodes.begin(), vec_nodes.end()), vec_nodes.end());
        DBUG("-> found %d nodes on the fracture %d", vec_nodes.size(), frac_id);
    }

    // create a vector fracture elements and connected matrix elements,
    // which are passed to a DoF table
    for (auto fracture_elements : vec_fracture_elements)
    {
        std::vector<MeshLib::Element*> vec_ele;
        // first, collect matrix elements
        for (MeshLib::Element*e : fracture_elements)
        {
            for (unsigned i=0; i<e->getNumberOfBaseNodes(); i++)
            {
                MeshLib::Node const* node = e->getNode(i);
                if (isCrackTip(*node))
                    continue;
                for (unsigned j=0; j<node->getNumberOfElements(); j++)
                {
                    // only matrix elements
                    if (node->getElement(j)->getDimension() < mesh.getDimension())
                        continue;
                    vec_ele.push_back(const_cast<MeshLib::Element*>(node->getElement(j)));
                }
            }
        }
        std::sort(vec_ele.begin(), vec_ele.end(),
            [](MeshLib::Element* p1, MeshLib::Element* p2) { return (p1->getID() < p2->getID()); }
            );
        vec_ele.erase(std::unique(vec_ele.begin(), vec_ele.end()), vec_ele.end());

        // second, append fracture elements
        vec_ele.insert(vec_ele.end(), fracture_elements.begin(), fracture_elements.end());

        vec_fracture_matrix_elements.push_back(vec_ele);
    }
}

}  // namespace SmallDeformationWithLIE
}  // namespace ProcessLib
