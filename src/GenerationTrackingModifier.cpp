#include "GenerationTrackingModifier.hpp"
#include "Exception.hpp"

template<unsigned DIM>
GenerationTrackingModifier<DIM>::GenerationTrackingModifier(int samplingTimestepMultiple=1)
    : AbstractCellBasedSimulationModifier<DIM, DIM>(),
    mSamplingTimestepMultiple(samplingTimestepMultiple) {
}

template<unsigned DIM>
GenerationTrackingModifier<DIM>::~GenerationTrackingModifier() {
}

template<unsigned DIM>
void GenerationTrackingModifier<DIM>::UpdateAtEndOfTimeStep(AbstractCellPopulation<DIM, DIM>& rCellPopulation) {
    // Have to update at the step before the output time step, so it's ready
    // in time for output
    if ((SimulationTime::Instance()->GetTimeStepsElapsed() + 1) % mSamplingTimestepMultiple == 0)
    {
        UpdateCellData(rCellPopulation);
    }
}

template<unsigned DIM>
void GenerationTrackingModifier<DIM>::UpdateAtEndOfOutputTimeStep(AbstractCellPopulation<DIM, DIM>& rCellPopulation) {
}

template<unsigned DIM>
void GenerationTrackingModifier<DIM>::SetupSolve(AbstractCellPopulation<DIM, DIM>& rCellPopulation, std::string outputDirectory) {
    /*
     * We must update CellData in SetupSolve(), otherwise it will not have been
     * fully initialised by the time we enter the main time loop.
     */
    UpdateCellData(rCellPopulation);
}

template<unsigned DIM>
void GenerationTrackingModifier<DIM>::UpdateCellData(AbstractCellPopulation<DIM, DIM>& rCellPopulation) {
    rCellPopulation.Update();

    for (typename AbstractCellPopulation<DIM>::Iterator cell_iter = rCellPopulation.Begin();
            cell_iter != rCellPopulation.End();
            ++cell_iter) {
        try
        {
            int generation = cell_iter->GetCellData()->GetItem("generation");
            // Need this to account for floating-point error
            double eps = SimulationTime::Instance()->GetTime()*DBL_EPSILON;
            // Has the cell been created since the last output time?
            if ((cell_iter->GetBirthTime() + eps) >= (SimulationTime::Instance()->GetTime() - mSamplingTimestepMultiple*SimulationTime::Instance()->GetTimeStep()))
            {
                cell_iter->GetCellData()->SetItem("generation", generation + 1);
            }
        }
        catch (Exception& e)
        {
            cell_iter->GetCellData()->SetItem("generation", 0);
        }
    }
}

template<unsigned DIM>
void GenerationTrackingModifier<DIM>::OutputSimulationModifierParameters(out_stream& rParamsFile) {
    // No parameters to output, so just call method on direct parent class
    AbstractCellBasedSimulationModifier<DIM>::OutputSimulationModifierParameters(rParamsFile);
}

// Explicit instantiation
template class GenerationTrackingModifier<1>;
template class GenerationTrackingModifier<2>;
template class GenerationTrackingModifier<3>;

// Serialization for Boost >= 1.36
#include "SerializationExportWrapperForCpp.hpp"
EXPORT_TEMPLATE_CLASS_SAME_DIMS(GenerationTrackingModifier)
