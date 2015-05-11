#include "AncestorTrackingModifier.hpp"
#include "MeshBasedCellPopulation.hpp"

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
}

template<unsigned DIM>
void AncestorTrackingModifier<DIM>::UpdateCellData(AbstractCellPopulation<DIM,DIM>& rCellPopulation)
{
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

