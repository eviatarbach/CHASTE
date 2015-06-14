#include "NagaiHondaMultipleDifferentialAdhesionForce.hpp"
#include "CellType.hpp"

template<unsigned DIM>
NagaiHondaMultipleDifferentialAdhesionForce<DIM>::NagaiHondaMultipleDifferentialAdhesionForce()
    : NagaiHondaForce<DIM>(),
    mAdhesionMatrix(2, std::vector<double>(2, 1.0))
{
}

template<unsigned DIM>
NagaiHondaMultipleDifferentialAdhesionForce<DIM>::~NagaiHondaMultipleDifferentialAdhesionForce()
{
}

template<unsigned DIM>
double NagaiHondaMultipleDifferentialAdhesionForce<DIM>::GetAdhesionParameter(Node<DIM>* pNodeA,
                                                                      Node<DIM>* pNodeB,
                                                                      VertexBasedCellPopulation<DIM>& rVertexCellPopulation)
{
    // Find the indices of the elements owned by each node
    std::set<unsigned> elements_containing_nodeA = pNodeA->rGetContainingElementIndices();
    std::set<unsigned> elements_containing_nodeB = pNodeB->rGetContainingElementIndices();

    // Find common elements
    std::set<unsigned> shared_elements;
    std::set_intersection(elements_containing_nodeA.begin(),
                          elements_containing_nodeA.end(),
                          elements_containing_nodeB.begin(),
                          elements_containing_nodeB.end(),
                          std::inserter(shared_elements, shared_elements.begin()));

    // Check that the nodes have a common edge
    assert(!shared_elements.empty());

    // If the edge corresponds to a single element, then the cell is on the boundary
    if (shared_elements.size() == 1)
    {
        unsigned element_index = *(shared_elements.begin());

        // Get cell associated with this element
        CellPtr p_cell = rVertexCellPopulation.GetCellUsingLocationIndex(element_index);

        CellPropertyCollection cell_data_collection = p_cell->rGetCellPropertyCollection().GetPropertiesType<CellType>();

        unsigned type = boost::static_pointer_cast<CellType>(cell_data_collection.GetProperty())->GetType();

        return mAdhesionMatrix.at(0).at(type);
    }
    else
    {
        std::set<unsigned>::iterator iter = shared_elements.begin();

        CellPtr p_cellA = rVertexCellPopulation.GetCellUsingLocationIndex(*iter);
        CellPtr p_cellB = rVertexCellPopulation.GetCellUsingLocationIndex(*(++iter));

        CellPropertyCollection cell_data_collectionA = p_cellA->rGetCellPropertyCollection().GetPropertiesType<CellType>();
        unsigned typeA = boost::static_pointer_cast<CellType>(cell_data_collectionA.GetProperty())->GetType();

        CellPropertyCollection cell_data_collectionB = p_cellB->rGetCellPropertyCollection().GetPropertiesType<CellType>();
        unsigned typeB = boost::static_pointer_cast<CellType>(cell_data_collectionB.GetProperty())->GetType();

        return mAdhesionMatrix.at(typeA).at(typeB);
    }
}

template<unsigned DIM>
void NagaiHondaMultipleDifferentialAdhesionForce<DIM>::SetNagaiHondaAdhesionMatrix(std::vector<std::vector<double> > adhesionMatrix)
{
    mAdhesionMatrix = adhesionMatrix;
}

template<unsigned DIM>
void NagaiHondaMultipleDifferentialAdhesionForce<DIM>::OutputForceParameters(out_stream& rParamsFile)
{
}

/////////////////////////////////////////////////////////////////////////////
// Explicit instantiation
/////////////////////////////////////////////////////////////////////////////

template class NagaiHondaMultipleDifferentialAdhesionForce<1>;
template class NagaiHondaMultipleDifferentialAdhesionForce<2>;
template class NagaiHondaMultipleDifferentialAdhesionForce<3>;

// Serialization for Boost >= 1.36
#include "SerializationExportWrapperForCpp.hpp"
EXPORT_TEMPLATE_CLASS_SAME_DIMS(NagaiHondaMultipleDifferentialAdhesionForce)
