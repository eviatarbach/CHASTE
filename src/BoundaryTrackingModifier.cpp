#include "BoundaryTrackingModifier.hpp"
#include "AbstractCellPopulation.hpp"
#include "MeshBasedCellPopulation.hpp"
#include "NodeBasedCellPopulation.hpp"
#include "PottsBasedCellPopulation.hpp"
#include "VertexBasedCellPopulation.hpp"

    template<unsigned DIM>
    BoundaryTrackingModifier<DIM>::BoundaryTrackingModifier()
: AbstractCellBasedSimulationModifier<DIM>()
{
}

    template<unsigned DIM>
BoundaryTrackingModifier<DIM>::~BoundaryTrackingModifier()
{
}

    template<unsigned DIM>
void BoundaryTrackingModifier<DIM>::UpdateAtEndOfTimeStep(AbstractCellPopulation<DIM,DIM>& rCellPopulation)
{
    UpdateCellData(rCellPopulation);
}

    template<unsigned DIM>
void BoundaryTrackingModifier<DIM>::SetupSolve(AbstractCellPopulation<DIM,DIM>& rCellPopulation, std::string outputDirectory)
{
    /*
     * We must update CellData in SetupSolve(), otherwise it will not have been 
     * fully initialised by the time we enter the main time loop.
     */
    UpdateCellData(rCellPopulation);
}

    template<unsigned DIM>
void BoundaryTrackingModifier<DIM>::UpdateCellData(AbstractCellPopulation<DIM,DIM>& rCellPopulation)
{
    if (dynamic_cast<MeshBasedCellPopulation<DIM>*>(&rCellPopulation))
    {
        MeshBasedCellPopulation<DIM>* pCellPopulation = static_cast<MeshBasedCellPopulation<DIM>*>(&(rCellPopulation));
        // Make sure the Voronoi tessellation has been created
        ///\todo #2273 - check if this call is needed
        pCellPopulation->CreateVoronoiTessellation();

        for (typename MeshBasedCellPopulation<DIM, DIM>::Iterator cell_iter = pCellPopulation->Begin();
                cell_iter != pCellPopulation->End();
                ++cell_iter)
        {
            // Get the location index corresponding to this cell
            unsigned index = pCellPopulation->GetLocationIndexUsingCell(*cell_iter);
            double boundary = pCellPopulation->GetSurfaceAreaOfVoronoiElement(index);
            cell_iter->GetCellData()->SetItem("boundary", boundary);
        }
    }
    else if (dynamic_cast<PottsBasedCellPopulation<DIM>*>(&rCellPopulation))
    {
        PottsBasedCellPopulation<DIM>* pCellPopulation = static_cast<PottsBasedCellPopulation<DIM>*>(&(rCellPopulation));

        for (typename PottsBasedCellPopulation<DIM>::Iterator cell_iter = pCellPopulation->Begin();
                cell_iter != pCellPopulation->End();
                ++cell_iter)
        {
            // Get the location index corresponding to this cell
            unsigned index = pCellPopulation->GetLocationIndexUsingCell(*cell_iter);
            double boundary = pCellPopulation->rGetMesh().GetSurfaceAreaOfElement(index);
            cell_iter->GetCellData()->SetItem("boundary", boundary);
        }
    }
    else if (dynamic_cast<VertexBasedCellPopulation<DIM>*>(&rCellPopulation))
    {
        VertexBasedCellPopulation<DIM>* pCellPopulation = static_cast<VertexBasedCellPopulation<DIM>*>(&(rCellPopulation));
        for (typename VertexBasedCellPopulation<DIM>::Iterator cell_iter = pCellPopulation->Begin();
                cell_iter != pCellPopulation->End();
                ++cell_iter)
        {
            // Get the location index corresponding to this cell
            unsigned index = pCellPopulation->GetLocationIndexUsingCell(*cell_iter);
            double boundary = pCellPopulation->rGetMesh().GetSurfaceAreaOfElement(index);
            cell_iter->GetCellData()->SetItem("boundary", boundary);
        }
    }
}

    template<unsigned DIM>
void BoundaryTrackingModifier<DIM>::OutputSimulationModifierParameters(out_stream& rParamsFile)
{
    // No parameters to output, so just call method on direct parent class
    AbstractCellBasedSimulationModifier<DIM>::OutputSimulationModifierParameters(rParamsFile);
}

// Explicit instantiation
template class BoundaryTrackingModifier<1>;
template class BoundaryTrackingModifier<2>;
template class BoundaryTrackingModifier<3>;

// Serialization for Boost >= 1.36
#include "SerializationExportWrapperForCpp.hpp"
EXPORT_TEMPLATE_CLASS_SAME_DIMS(BoundaryTrackingModifier)
