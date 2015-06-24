#include "ContactDependentForce.hpp"
#include "VertexElement.hpp"
#include "CellwiseDataGradient.hpp"
#include "VertexBasedCellPopulation.hpp"
#include "CellLabel.hpp"

template<unsigned DIM>
ContactDependentForce<DIM>::ContactDependentForce()
    : AbstractForce<DIM>(),
    mForce()
{
}

template<unsigned DIM>
ContactDependentForce<DIM>::~ContactDependentForce()
{
}

template<unsigned DIM>
void ContactDependentForce<DIM>::SetForce(c_vector<double,DIM> force)
{
    mForce = force;
}

template<unsigned DIM>
void ContactDependentForce<DIM>::AddForceContribution(AbstractCellPopulation<DIM,DIM>& rCellPopulation)
{
    if (dynamic_cast<VertexBasedCellPopulation<DIM>*>(&rCellPopulation))
    {
        VertexBasedCellPopulation<DIM>* pCellPopulation = static_cast<VertexBasedCellPopulation<DIM>*>(&(rCellPopulation));
        for (typename VertexBasedCellPopulation<DIM>::Iterator cell_iter = pCellPopulation->Begin();
                cell_iter != pCellPopulation->End();
                ++cell_iter)
        {
            cell_iter->GetCellData()->SetItem("ext_force_x", 0.0);
            cell_iter->GetCellData()->SetItem("ext_force_y", 0.0);

            cell_iter->template RemoveCellProperty<CellLabel>();

            //unsigned cell_index = pCellPopulation->GetLocationIndexUsingCell(*cell_iter);
            double mediumContact = cell_iter->GetCellData()->GetItem("boundary") - cell_iter->GetCellData()->GetItem("contact_boundary");
            if (10*DBL_EPSILON < mediumContact)
            {
                boost::shared_ptr<AbstractCellProperty> force_label(new CellLabel(4));
                cell_iter->AddCellProperty(force_label);

                VertexElement<DIM,DIM>* pElement = pCellPopulation->GetElementCorrespondingToCell(*cell_iter);

                cell_iter->GetCellData()->SetItem("ext_force_x", mForce[0]);
                cell_iter->GetCellData()->SetItem("ext_force_y", mForce[1]);

                for (unsigned local_index=0; local_index < pElement->GetNumNodes(); local_index++)
                {
                    unsigned global_index = pElement->GetNodeGlobalIndex(local_index);
                    pCellPopulation->GetNode(global_index)->AddAppliedForceContribution(mForce);
                }
            }
        }
    }
}

template<unsigned DIM>
void ContactDependentForce<DIM>::OutputForceParameters(out_stream& rParamsFile)
{
    // No parameters to include

    // Call method on direct parent class
    AbstractForce<DIM>::OutputForceParameters(rParamsFile);
}

/////////////////////////////////////////////////////////////////////////////
// Explicit instantiation
/////////////////////////////////////////////////////////////////////////////

template class ContactDependentForce<1>;
template class ContactDependentForce<2>;
template class ContactDependentForce<3>;

// Serialization for Boost >= 1.36
#include "SerializationExportWrapperForCpp.hpp"
EXPORT_TEMPLATE_CLASS_SAME_DIMS(ContactDependentForce)
