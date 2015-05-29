
#include "ContactBoundaryTrackingModifier.hpp"
#include "AbstractCellPopulation.hpp"
#include "MeshBasedCellPopulation.hpp"
#include "NodeBasedCellPopulation.hpp"
#include "PottsBasedCellPopulation.hpp"
#include "VertexBasedCellPopulation.hpp"

template<unsigned DIM>
ContactBoundaryTrackingModifier<DIM>::ContactBoundaryTrackingModifier()
: AbstractCellBasedSimulationModifier<DIM>() {
}

template<unsigned DIM>
ContactBoundaryTrackingModifier<DIM>::~ContactBoundaryTrackingModifier() {
}

template<unsigned DIM>
void ContactBoundaryTrackingModifier<DIM>::UpdateAtEndOfTimeStep(AbstractCellPopulation<DIM, DIM>& rCellPopulation) {
    UpdateCellData(rCellPopulation);
}

template<unsigned DIM>
void ContactBoundaryTrackingModifier<DIM>::SetupSolve(AbstractCellPopulation<DIM, DIM>& rCellPopulation, std::string outputDirectory) {
    /*
     * We must update CellData in SetupSolve(), otherwise it will not have been 
     * fully initialised by the time we enter the main time loop.
     */
    UpdateCellData(rCellPopulation);
}

template<unsigned DIM>
void ContactBoundaryTrackingModifier<DIM>::UpdateCellData(AbstractCellPopulation<DIM, DIM>& rCellPopulation) {
    if (dynamic_cast<MeshBasedCellPopulation<DIM>*> (&rCellPopulation)) {
        MeshBasedCellPopulation<DIM>* pCellPopulation = static_cast<MeshBasedCellPopulation<DIM>*> (&(rCellPopulation));
        // Make sure the Voronoi tessellation has been created
        ///\todo #2273 - check if this call is needed
        pCellPopulation->CreateVoronoiTessellation();

        for (typename MeshBasedCellPopulation<DIM, DIM>::Iterator cell_iter = pCellPopulation->Begin();
                cell_iter != pCellPopulation->End();
                ++cell_iter) {
            double neighbour_contact = 0.0;

            // Get the location index corresponding to this cell
            unsigned index = pCellPopulation->GetLocationIndexUsingCell(*cell_iter);
            double boundary = pCellPopulation->GetSurfaceAreaOfVoronoiElement(index);

            std::set<unsigned> neighbour_indices = pCellPopulation->GetNeighbouringNodeIndices(index);
            for (std::set<unsigned>::iterator neighbour_iter = neighbour_indices.begin();
                    neighbour_iter != neighbour_indices.end();
                    ++neighbour_iter) {
                // Get the length of the edge shared with this neighbour
                unsigned neighbour_index = *neighbour_iter;
                double edge_length = pCellPopulation->GetVoronoiEdgeLength(index, neighbour_index);

                // Ignore ghost nodes (in the case of a MeshBasedCellPopulationWithGhostNodes)
                ///\todo #2273 - check we have correctly dealt with ghost nodes
                if (!pCellPopulation->IsGhostNode(neighbour_index)) {
                    neighbour_contact += edge_length;
                }
            }

            cell_iter->GetCellData()->SetItem("contact_boundary", boundary - neighbour_contact);
        }
    } else if (dynamic_cast<PottsBasedCellPopulation<DIM>*> (&rCellPopulation)) {
        PottsBasedCellPopulation<DIM>* pCellPopulation = static_cast<PottsBasedCellPopulation<DIM>*> (&(rCellPopulation));

        for (typename PottsBasedCellPopulation<DIM>::Iterator cell_iter = pCellPopulation->Begin();
                cell_iter != pCellPopulation->End();
                ++cell_iter) {
            double neighbour_contact = 0.0;

            // Get the location index corresponding to this cell
            unsigned elem_index = pCellPopulation->GetLocationIndexUsingCell(*cell_iter);
            double boundary = pCellPopulation->rGetMesh().GetSurfaceAreaOfElement(elem_index);

            unsigned num_nodes_in_elem = pCellPopulation->rGetMesh().GetElement(elem_index)->GetNumNodes();

            // Iterate over nodes contained in the element corresponding to this cell
            for (unsigned local_index = 0; local_index < num_nodes_in_elem; local_index++) {
                // Get this node's von Neumann neighbours (not Moore neighbours, since they must share an edge)
                unsigned global_index = pCellPopulation->rGetMesh().GetElement(elem_index)->GetNodeGlobalIndex(local_index);
                std::set<unsigned> neighbour_node_indices = pCellPopulation->rGetMesh().GetVonNeumannNeighbouringNodeIndices(global_index);

                // Iterate over these neighbours
                for (std::set<unsigned>::iterator neighbour_iter = neighbour_node_indices.begin();
                        neighbour_iter != neighbour_node_indices.end();
                        ++neighbour_iter) {
                    // Get the elements containing this neighbour
                    std::set<unsigned> neighbour_elem_indices = pCellPopulation->GetNode(*neighbour_iter)->rGetContainingElementIndices();

                    if (neighbour_elem_indices.size() == 1) {
                        unsigned neigbour_elem_index = *(neighbour_elem_indices.begin());

                        if (neigbour_elem_index != elem_index) {
                            // Edge is between two different elements
                            neighbour_contact += 1.0;
                        }
                    }
                }
            }

            cell_iter->GetCellData()->SetItem("contact_boundary", boundary - neighbour_contact);
        }
    } else if (dynamic_cast<VertexBasedCellPopulation<DIM>*> (&rCellPopulation)) {
        VertexBasedCellPopulation<DIM>* pCellPopulation = static_cast<VertexBasedCellPopulation<DIM>*> (&(rCellPopulation));

        for (typename VertexBasedCellPopulation<DIM>::Iterator cell_iter = pCellPopulation->Begin();
                cell_iter != pCellPopulation->End();
                ++cell_iter) {
            double neighbour_contact = 0.0;

            // Get the location index corresponding to this cell
            unsigned elem_index = pCellPopulation->GetLocationIndexUsingCell(*cell_iter);
            double boundary = pCellPopulation->rGetMesh().GetSurfaceAreaOfElement(elem_index);

            // Get the set of neighbouring element indices
            std::set<unsigned> neighbour_elem_indices = pCellPopulation->rGetMesh().GetNeighbouringElementIndices(elem_index);

            // Iterate over these neighbours
            for (std::set<unsigned>::iterator neighbour_iter = neighbour_elem_indices.begin();
                    neighbour_iter != neighbour_elem_indices.end();
                    ++neighbour_iter) {
                // Get the length of the edge shared with this neighbour
                unsigned neighbour_index = *neighbour_iter;
                double edge_length = pCellPopulation->rGetMesh().GetEdgeLength(elem_index, neighbour_index);

                neighbour_contact += edge_length;
            }
            cell_iter->GetCellData()->SetItem("contact_boundary", boundary - neighbour_contact);
        }
    }
}

template<unsigned DIM>
void ContactBoundaryTrackingModifier<DIM>::OutputSimulationModifierParameters(out_stream& rParamsFile) {
    // No parameters to output, so just call method on direct parent class
    AbstractCellBasedSimulationModifier<DIM>::OutputSimulationModifierParameters(rParamsFile);
}

// Explicit instantiation
template class ContactBoundaryTrackingModifier<1>;
template class ContactBoundaryTrackingModifier<2>;
template class ContactBoundaryTrackingModifier<3>;

// Serialization for Boost >= 1.36
#include "SerializationExportWrapperForCpp.hpp"
EXPORT_TEMPLATE_CLASS_SAME_DIMS(ContactBoundaryTrackingModifier)