#include "AppliedForce.hpp"
#include "VertexElement.hpp"
#include "CellwiseDataGradient.hpp"
#include "VertexBasedCellPopulation.hpp"

template<unsigned DIM>
AppliedForce<DIM>::AppliedForce()
    : AbstractForce<DIM>(),
    mForceMap()
{
}

template<unsigned DIM>
AppliedForce<DIM>::~AppliedForce()
{
}

template<unsigned DIM>
void AppliedForce<DIM>::SetForceMap(std::map <unsigned, c_vector<double,DIM> > forceMap)
{
    mForceMap = forceMap;
}

template<unsigned DIM>
void AppliedForce<DIM>::AddForceContribution(AbstractCellPopulation<DIM,DIM>& rCellPopulation)
{
    if (dynamic_cast<VertexBasedCellPopulation<DIM>*>(&rCellPopulation))
    {
        VertexBasedCellPopulation<DIM>* pCellPopulation = static_cast<VertexBasedCellPopulation<DIM>*>(&(rCellPopulation));
        for (typename VertexBasedCellPopulation<DIM>::Iterator cell_iter = pCellPopulation->Begin();
                cell_iter != pCellPopulation->End();
                ++cell_iter)
        {
            unsigned cell_index = pCellPopulation->GetLocationIndexUsingCell(*cell_iter);
            if (mForceMap.find(cell_index) != mForceMap.end())
            {
                VertexElement<DIM,DIM>* pElement = pCellPopulation->GetElementCorrespondingToCell(*cell_iter);

                c_vector<double,DIM> force = mForceMap[cell_index];

                for (unsigned local_index=0; local_index < pElement->GetNumNodes(); local_index++)
                {
                    unsigned global_index = pElement->GetNodeGlobalIndex(local_index);
                    pCellPopulation->GetNode(global_index)->AddAppliedForceContribution(force);
                }
            }
        }
    }
}

template<unsigned DIM>
void AppliedForce<DIM>::OutputForceParameters(out_stream& rParamsFile)
{
    // No parameters to include

    // Call method on direct parent class
    AbstractForce<DIM>::OutputForceParameters(rParamsFile);
}

/////////////////////////////////////////////////////////////////////////////
// Explicit instantiation
/////////////////////////////////////////////////////////////////////////////

template class AppliedForce<1>;
template class AppliedForce<2>;
template class AppliedForce<3>;

// Serialization for Boost >= 1.36
#include "SerializationExportWrapperForCpp.hpp"
EXPORT_TEMPLATE_CLASS_SAME_DIMS(AppliedForce)
