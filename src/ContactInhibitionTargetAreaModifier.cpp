#include <cmath>
#include "ContactInhibitionTargetAreaModifier.hpp"

template<unsigned DIM>
ContactInhibitionTargetAreaModifier<DIM>::ContactInhibitionTargetAreaModifier()
    : AbstractTargetAreaModifier<DIM>(),
    mScalingParameter(0.01),
    mExponent(1.0)
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
void ContactInhibitionTargetAreaModifier<DIM>::SetExponent(double exponent)
{
    mExponent = exponent;
}

template<unsigned DIM>
void ContactInhibitionTargetAreaModifier<DIM>::UpdateTargetAreaOfCell(CellPtr pCell)
{
    double cell_target_area;

    try
    {
        double cell_age = pCell->GetAge();
        double time_step = SimulationTime::Instance()->GetTimeStep();

        // TODO: make sure this is right
        if (cell_age > time_step + 10*DBL_EPSILON)
        {
            cell_target_area = pCell->GetCellData()->GetItem("target area");
        }
        else
        {
            // This is a new cell
            cell_target_area = pCell->GetCellData()->GetItem("starting area");
        }
    }
    catch (Exception& e)
    {
        // This is the beginning of the simulation; the target area should start at the cell's initial area
        double volume = pCell->GetCellData()->GetItem("volume");
        cell_target_area = volume;

        // Record the starting area for setting the target area of daughter cells
        pCell->GetCellData()->SetItem("starting area", volume);
    }

    double perimeter = pCell->GetCellData()->GetItem("boundary");
    double mediumContact = perimeter - pCell->GetCellData()->GetItem("contact_boundary");
    cell_target_area += mScalingParameter/(1.0 + std::exp(-mExponent*(mediumContact/perimeter)));

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
