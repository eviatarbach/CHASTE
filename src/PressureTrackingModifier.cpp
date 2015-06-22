#include "PressureTrackingModifier.hpp"
#include "AbstractCellPopulation.hpp"

    template<unsigned DIM>
    PressureTrackingModifier<DIM>::PressureTrackingModifier()
: AbstractCellBasedSimulationModifier<DIM>(),
    mDeformationParameter(DOUBLE_UNSET)
{
}

    template<unsigned DIM>
PressureTrackingModifier<DIM>::~PressureTrackingModifier()
{
}

    template<unsigned DIM>
void PressureTrackingModifier<DIM>::SetDeformationParameter(double deformationParameter)
{
    mDeformationParameter = deformationParameter;
}

    template<unsigned DIM>
void PressureTrackingModifier<DIM>::UpdateAtEndOfTimeStep(AbstractCellPopulation<DIM,DIM>& rCellPopulation)
{
    UpdateCellData(rCellPopulation);
}

    template<unsigned DIM>
void PressureTrackingModifier<DIM>::SetupSolve(AbstractCellPopulation<DIM,DIM>& rCellPopulation, std::string outputDirectory)
{
    /*
     * We must update CellData in SetupSolve(), otherwise it will not have been 
     * fully initialised by the time we enter the main time loop.
     */
    UpdateCellData(rCellPopulation);
}

    template<unsigned DIM>
void PressureTrackingModifier<DIM>::UpdateCellData(AbstractCellPopulation<DIM,DIM>& rCellPopulation)
{
    if (mDeformationParameter == DOUBLE_UNSET)
    {
        EXCEPTION("Deformation parameter must be set");
    }
    for (typename AbstractCellPopulation<DIM, DIM>::Iterator cell_iter = rCellPopulation.Begin();
            cell_iter != rCellPopulation.End();
            ++cell_iter)
    {
        double contractility;
        try
        {
            contractility = cell_iter->GetCellData()->GetItem("contractility");
        }
        catch (Exception& e)
        {
            contractility = 0.0;
        }

        double volume = cell_iter->GetCellData()->GetItem("volume");
        double target_area = cell_iter->GetCellData()->GetItem("target area");
        double pressure = -2*contractility*volume + 2*(target_area - volume)*mDeformationParameter;

        cell_iter->GetCellData()->SetItem("pressure", pressure);
    }
}

    template<unsigned DIM>
void PressureTrackingModifier<DIM>::OutputSimulationModifierParameters(out_stream& rParamsFile)
{
    // No parameters to output, so just call method on direct parent class
    AbstractCellBasedSimulationModifier<DIM>::OutputSimulationModifierParameters(rParamsFile);
}

// Explicit instantiation
template class PressureTrackingModifier<1>;
template class PressureTrackingModifier<2>;
template class PressureTrackingModifier<3>;

// Serialization for Boost >= 1.36
#include "SerializationExportWrapperForCpp.hpp"
EXPORT_TEMPLATE_CLASS_SAME_DIMS(PressureTrackingModifier)
