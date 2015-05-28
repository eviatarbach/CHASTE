#include "AncestorTrackingModifier.hpp"
#include "Exception.hpp"
#include "SimulationTime.hpp"

    template<unsigned DIM>
    AncestorTrackingModifier<DIM>::AncestorTrackingModifier()
: AbstractCellBasedSimulationModifier<DIM>()
{
}

    template<unsigned DIM>
AncestorTrackingModifier<DIM>::~AncestorTrackingModifier()
{
}

    template<unsigned DIM>
void AncestorTrackingModifier<DIM>::UpdateAtEndOfTimeStep(AbstractCellPopulation<DIM,DIM>& rCellPopulation)
{
    UpdateCellData(rCellPopulation);
}

    template<unsigned DIM>
void AncestorTrackingModifier<DIM>::UpdateAtEndOfOutputTimeStep(AbstractCellPopulation<DIM,DIM>& rCellPopulation)
{
    rCellPopulation.SetCellAncestorsToLocationIndices();
}

    template<unsigned DIM>
void AncestorTrackingModifier<DIM>::SetupSolve(AbstractCellPopulation<DIM,DIM>& rCellPopulation, std::string outputDirectory)
{
    /*
     * We must update CellData in SetupSolve(), otherwise it will not have been
     * fully initialised by the time we enter the main time loop.
     */
    rCellPopulation.SetCellAncestorsToLocationIndices();
    UpdateCellData(rCellPopulation);
}

    template<unsigned DIM>
void AncestorTrackingModifier<DIM>::UpdateCellData(AbstractCellPopulation<DIM,DIM>& rCellPopulation)
{
    rCellPopulation.Update();

    for (typename AbstractCellPopulation<DIM>::Iterator cell_iter = rCellPopulation.Begin();
            cell_iter != rCellPopulation.End();
            ++cell_iter)
    {
        if (cell_iter->GetAncestor() != cell_iter->GetCellId())
        {
            if (cell_iter->GetAncestor() != UNSIGNED_UNSET)
            {
                cell_iter->GetCellData()->SetItem("direct_ancestor", cell_iter->GetAncestor());
            }
        }
        try
        {
            cell_iter->GetCellData()->GetItem("direct_ancestor");
        }
        catch (Exception& e)
        {
            if (cell_iter->GetAncestor() != UNSIGNED_UNSET)
            {
                cell_iter->GetCellData()->SetItem("direct_ancestor", cell_iter->GetAncestor());
            }
        }
        }
}

    template<unsigned DIM>
void AncestorTrackingModifier<DIM>::OutputSimulationModifierParameters(out_stream& rParamsFile)
{
    // No parameters to output, so just call method on direct parent class
    AbstractCellBasedSimulationModifier<DIM>::OutputSimulationModifierParameters(rParamsFile);
}

// Explicit instantiation
template class AncestorTrackingModifier<1>;
template class AncestorTrackingModifier<2>;
template class AncestorTrackingModifier<3>;

// Serialization for Boost >= 1.36
#include "SerializationExportWrapperForCpp.hpp"
EXPORT_TEMPLATE_CLASS_SAME_DIMS(AncestorTrackingModifier)

