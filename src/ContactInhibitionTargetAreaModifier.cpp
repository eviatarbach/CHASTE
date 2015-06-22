#include <cmath>
#include "ContactInhibitionTargetAreaModifier.hpp"

template<unsigned DIM>
ContactInhibitionTargetAreaModifier<DIM>::ContactInhibitionTargetAreaModifier()
    : AbstractTargetAreaModifier<DIM>(),
    mScalingParameter(0.01)
{
}

template<unsigned DIM>
ContactInhibitionTargetAreaModifier<DIM>::~ContactInhibitionTargetAreaModifier()
{
}

template<unsigned DIM>
void ContactInhibitionTargetAreaModifier<DIM>::SetScalingParameter(double scalingParameter)
{
    mScalingParameter = scalingParameter;
}

template<unsigned DIM>
void ContactInhibitionTargetAreaModifier<DIM>::UpdateTargetAreaOfCell(CellPtr pCell)
{
    double cell_target_area;

    try
    {
        cell_target_area = pCell->GetCellData()->GetItem("target area");
    }
    catch (Exception& e)
    {
        // This is the beginning of the simulation; the target area should start at the cell's initial area
        double volume = pCell->GetCellData()->GetItem("volume");
        cell_target_area = volume;

        // Record the starting area for setting the target area of daughter cells
        pCell->GetCellData()->SetItem("starting area", volume);
    }

    double g1_duration = pCell->GetCellCycleModel()->GetG1Duration();

    // If the cell is differentiated then its G1 duration is infinite
    if (g1_duration == DBL_MAX) // don't use magic number, compare to DBL_MAX
    {
        // This is just for fixed cell-cycle models, need to work out how to find the g1 duration
        g1_duration = pCell->GetCellCycleModel()->GetTransitCellG1Duration();
    }

    double cell_age = pCell->GetAge();

    if (cell_age < g1_duration)
    {
        double perimeter = pCell->GetCellData()->GetItem("boundary");
        double mediumContact = perimeter - pCell->GetCellData()->GetItem("contact_boundary");
        cell_target_area += mScalingParameter/(1.0 + std::exp(-2.0*(mediumContact/perimeter)));
    }
    else
    {
        /**
         * At division, daughter cells inherit the cell data array from the mother cell.
         * Here, we assign the target area that we want daughter cells to have to cells
         * that we know to divide in this time step.
         *
         * \todo This is a little hack that we might want to clean up in the future.
         */
        if (pCell->ReadyToDivide())
        {
            cell_target_area = pCell->GetCellData()->GetItem("starting area");
        }
    }

    // Set cell data
    pCell->GetCellData()->SetItem("target area", cell_target_area);
}

template<unsigned DIM>
void ContactInhibitionTargetAreaModifier<DIM>::OutputSimulationModifierParameters(out_stream& rParamsFile)
{
    // No parameters to output, so just call method on direct parent class
    AbstractTargetAreaModifier<DIM>::OutputSimulationModifierParameters(rParamsFile);
}

// Explicit instantiation
template class ContactInhibitionTargetAreaModifier<1>;
template class ContactInhibitionTargetAreaModifier<2>;
template class ContactInhibitionTargetAreaModifier<3>;

// Serialization for Boost >= 1.36
#include "SerializationExportWrapperForCpp.hpp"
EXPORT_TEMPLATE_CLASS_SAME_DIMS(ContactInhibitionTargetAreaModifier)
