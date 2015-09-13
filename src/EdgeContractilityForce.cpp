#include "EdgeContractilityForce.hpp"
#include "VertexElement.hpp"
#include "CellwiseDataGradient.hpp"
#include "VertexBasedCellPopulation.hpp"
#include "CellLabel.hpp"
#include "RandomNumberGenerator.hpp"

template<unsigned DIM>
EdgeContractilityForce<DIM>::EdgeContractilityForce()
    : AbstractForce<DIM>(),
    mProbability(0.01),
    mDuration(10),
    mMagnitude(50.0),
    mExponent(1.0)
{
}

template<unsigned DIM>
EdgeContractilityForce<DIM>::~EdgeContractilityForce()
{
}

template<unsigned DIM>
void EdgeContractilityForce<DIM>::SetProbability(double probability)
{
    mProbability = probability;
}

template<unsigned DIM>
void EdgeContractilityForce<DIM>::SetDuration(unsigned duration)
{
    mDuration = duration;
}

template<unsigned DIM>
void EdgeContractilityForce<DIM>::SetMagnitude(double magnitude)
{
    mMagnitude = magnitude;
}

template<unsigned DIM>
void EdgeContractilityForce<DIM>::SetExponent(double exponent)
{
    mExponent = exponent;
}

template<unsigned DIM>
void EdgeContractilityForce<DIM>::AddForceContribution(AbstractCellPopulation<DIM,DIM>& rCellPopulation)
{
    if (dynamic_cast<VertexBasedCellPopulation<DIM>*>(&rCellPopulation))
    {
        VertexBasedCellPopulation<DIM>* pCellPopulation = static_cast<VertexBasedCellPopulation<DIM>*>(&(rCellPopulation));

        for (typename VertexBasedCellPopulation<DIM>::Iterator cell_iter = pCellPopulation->Begin();
                cell_iter != pCellPopulation->End();
                ++cell_iter)
        {
            int edge_contracting_step;
            try
            {
                edge_contracting_step = (int) cell_iter->GetCellData()->GetItem("edge_contracting_step");
            }
            catch (Exception& e)
            {
                edge_contracting_step = -1;
            }

            VertexElement<DIM,DIM>* pElement = pCellPopulation->GetElementCorrespondingToCell(*cell_iter);
            unsigned num_nodes = pElement->GetNumNodes();

            if (edge_contracting_step >= 0)
            {
                unsigned node_index = (unsigned) cell_iter->GetCellData()->GetItem("contracting_node_index");

                if (node_index < num_nodes)
                {
                    unsigned global_index = pElement->GetNodeGlobalIndex(node_index);
                    unsigned global_index_next = pElement->GetNodeGlobalIndex(node_index + 1 < num_nodes ? node_index + 1 : 0);

                    Node<DIM>* pNode = pCellPopulation->GetNode(global_index);
                    Node<DIM>* pNextNode = pCellPopulation->GetNode(global_index_next);

                    c_vector<double, DIM> node_location = pNode->rGetLocation();
                    c_vector<double, DIM> next_node_location = pNextNode->rGetLocation();

                    c_vector<double, DIM> force(2);
                    force[0] = mMagnitude*exp(mExponent*edge_contracting_step)*(next_node_location[0] - node_location[0]);
                    force[1] = mMagnitude*exp(mExponent*edge_contracting_step)*(next_node_location[1] - node_location[1]);

                    c_vector<double, DIM> force_next(2);
                    force_next[0] = -force[0];
                    force_next[1] = -force[1];

                    pNode->AddAppliedForceContribution(force);
                    pNextNode->AddAppliedForceContribution(force_next);

                    if (++edge_contracting_step > mDuration)
                    {
                        cell_iter->GetCellData()->SetItem("edge_contracting_step", -1.0);
                    }
                    else if (pow(next_node_location[0] - node_location[0], 2) + pow(next_node_location[1] - node_location[1], 2) < 0.0001)
                    {
                        cell_iter->GetCellData()->SetItem("edge_contracting_step", -1.0);
                    }
                    else
                    {
                        cell_iter->GetCellData()->SetItem("edge_contracting_step", (double) edge_contracting_step);
                    }
                }
            }
            else
            {
                if (RandomNumberGenerator::Instance()->ranf() < mProbability)
                {
                    unsigned node_index = RandomNumberGenerator::Instance()->randMod(num_nodes);
                    unsigned global_index = pElement->GetNodeGlobalIndex(node_index);
                    unsigned global_index_next = pElement->GetNodeGlobalIndex(node_index + 1 < num_nodes ? node_index + 1 : 0);

                    Node<DIM>* pNode = pCellPopulation->GetNode(global_index);
                    Node<DIM>* pNextNode = pCellPopulation->GetNode(global_index_next);

                    if ((pNode->GetNumContainingElements() >= 2) && (pNextNode->GetNumContainingElements() >= 2) && (pNode->GetNumContainingElements() == pNextNode->GetNumContainingElements()))
                    {
                        c_vector<double, DIM> node_location = pNode->rGetLocation();
                        c_vector<double, DIM> next_node_location = pNextNode->rGetLocation();

                        c_vector<double, DIM> force(2);
                        force[0] = mMagnitude*(next_node_location[0] - node_location[0]);
                        force[1] = mMagnitude*(next_node_location[1] - node_location[1]);

                        c_vector<double, DIM> force_next(2);
                        force_next[0] = -force[0];
                        force_next[1] = -force[1];

                        pNode->AddAppliedForceContribution(force);
                        pNextNode->AddAppliedForceContribution(force_next);

                        cell_iter->GetCellData()->SetItem("edge_contracting_step", 0.0);
                        cell_iter->GetCellData()->SetItem("contracting_node_index", (double) node_index);
                    }
                    else
                    {
                     cell_iter->GetCellData()->SetItem("edge_contracting_step", -1.0);
                     cell_iter->GetCellData()->SetItem("contracting_node_index", -1.0);
                    }
                }
                else
                {
                    cell_iter->GetCellData()->SetItem("edge_contracting_step", -1.0);
                    cell_iter->GetCellData()->SetItem("contracting_node_index", -1.0);
                }
            }

        }
    }
}

template<unsigned DIM>
void EdgeContractilityForce<DIM>::OutputForceParameters(out_stream& rParamsFile)
{
    // No parameters to include

    // Call method on direct parent class
    AbstractForce<DIM>::OutputForceParameters(rParamsFile);
}

/////////////////////////////////////////////////////////////////////////////
// Explicit instantiation
/////////////////////////////////////////////////////////////////////////////

template class EdgeContractilityForce<1>;
template class EdgeContractilityForce<2>;
template class EdgeContractilityForce<3>;

// Serialization for Boost >= 1.36
#include "SerializationExportWrapperForCpp.hpp"
EXPORT_TEMPLATE_CLASS_SAME_DIMS(EdgeContractilityForce)
