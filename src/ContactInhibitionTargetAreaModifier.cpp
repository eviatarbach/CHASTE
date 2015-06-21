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
        // Get target area A of a healthy cell in S, G2 or M phase
        cell_target_area = this->mReferenceTargetArea;
    }

    double g1_duration = pCell->GetCellCycleModel()->GetG1Duration();

    // If the cell is differentiated then its G1 duration is infinite
    if (g1_duration == DBL_MAX) // don't use magic number, compare to DBL_MAX
    {
        // This is just for fixed cell-cycle models, need to work out how to find the g1 duration
        g1_duration = pCell->GetCellCycleModel()->GetTransitCellG1Duration();
    }

    /*
    if (pCell->HasCellProperty<ApoptoticCellProperty>())
    {
        // Age of cell when apoptosis begins
        if (pCell->GetStartOfApoptosisTime() - pCell->GetBirthTime() < g1_duration)
        {
            cell_target_area *= 0.5*(1 + (pCell->GetStartOfApoptosisTime() - pCell->GetBirthTime())/g1_duration);
        }

        // The target area of an apoptotic cell decreases linearly to zero (and past it negative)
        cell_target_area = cell_target_area - 0.5*cell_target_area/(pCell->GetApoptosisTime())*(SimulationTime::Instance()->GetTime()-pCell->GetStartOfApoptosisTime());

        // Don't allow a negative target area
        if (cell_target_area < 0)
        {
            cell_target_area = 0;
        }
    }
    else
    {*/
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
            cell_target_area = 0.5*this->mReferenceTargetArea;
        }
    }
    //}

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
