#include "BoundaryTrackingModifier.hpp"
#include "AbstractCellPopulation.hpp"
#include "MeshBasedCellPopulation.hpp"
#include "NodeBasedCellPopulation.hpp"
#include "PottsBasedCellPopulation.hpp"
#include "VertexBasedCellPopulation.hpp"

    template<unsigned DIM>
    BoundaryTrackingModifier<DIM>::BoundaryTrackingModifier()
: AbstractCellBasedSimulationModifier<DIM>()
{
}

    template<unsigned DIM>
BoundaryTrackingModifier<DIM>::~BoundaryTrackingModifier()
{
}

    template<unsigned DIM>
void BoundaryTrackingModifier<DIM>::UpdateAtEndOfTimeStep(AbstractCellPopulation<DIM,DIM>& rCellPopulation)
{
    UpdateCellData(rCellPopulation);
}

    template<unsigned DIM>
void BoundaryTrackingModifier<DIM>::SetupSolve(AbstractCellPopulation<DIM,DIM>& rCellPopulation, std::string outputDirectory)
{
    /*
     * We must update CellData in SetupSolve(), otherwise it will not have been 
     * fully initialised by the time we enter the main time loop.
     */
    UpdateCellData(rCellPopulation);
}

    template<unsigned DIM>
void BoundaryTrackingModifier<DIM>::UpdateCellData(AbstractCellPopulation<DIM,DIM>& rCellPopulation)
{
    if (dynamic_cast<MeshBasedCellPopulation<DIM>*>(&rCellPopulation))
    {
        MeshBasedCellPopulation<DIM>* pCellPopulation = static_cast<MeshBasedCellPopulation<DIM>*>(&(rCellPopulation));
        // Make sure the Voronoi tessellation has been created
        ///\todo #2273 - check if this call is needed
        pCellPopulation->CreateVoronoiTessellation();

        for (typename MeshBasedCellPopulation<DIM, DIM>::Iterator cell_iter = pCellPopulation->Begin();
                cell_iter != pCellPopulation->End();
                ++cell_iter)
        {
            // Get the location index corresponding to this cell
            unsigned index = pCellPopulation->GetLocationIndexUsingCell(*cell_iter);
            double boundary = pCellPopulation->GetSurfaceAreaOfVoronoiElement(index);
            cell_iter->GetCellData()->SetItem("boundary", boundary);
        }
    }
    else if (dynamic_cast<PottsBasedCellPopulation<DIM>*>(&rCellPopulation))
    {
        PottsBasedCellPopulation<DIM>* pCellPopulation = static_cast<PottsBasedCellPopulation<DIM>*>(&(rCellPopulation));

        for (typename PottsBasedCellPopulation<DIM>::Iterator cell_iter = pCellPopulation->Begin();
                cell_iter != pCellPopulation->End();
                ++cell_iter)
        {
            // Get the location index corresponding to this cell
            unsigned index = pCellPopulation->GetLocationIndexUsingCell(*cell_iter);
            double boundary = pCellPopulation->rGetMesh().GetSurfaceAreaOfElement(index);
            cell_iter->GetCellData()->SetItem("boundary", boundary);
        }
    }
    else if (dynamic_cast<VertexBasedCellPopulation<DIM>*>(&rCellPopulation))
    {
        VertexBasedCellPopulation<DIM>* pCellPopulation = static_cast<VertexBasedCellPopulation<DIM>*>(&(rCellPopulation));
        for (typename VertexBasedCellPopulation<DIM>::Iterator cell_iter = pCellPopulation->Begin();
                cell_iter != pCellPopulation->End();
                ++cell_iter)
        {
            // Get the location index corresponding to this cell
            unsigned index = pCellPopulation->GetLocationIndexUsingCell(*cell_iter);
            double boundary = pCellPopulation->rGetMesh().GetSurfaceAreaOfElement(index);
            cell_iter->GetCellData()->SetItem("boundary", boundary);
        }
    }
    else if (dynamic_cast<NodeBasedCellPopulation<DIM>*>(&rCellPopulation))
    {

        // Not implemented or tested in 1D
        assert(DIM==2 ||DIM==3);

        // Get node index corresponding to this cell
        unsigned node_index = this->GetLocationIndexUsingCell(pCell);
        Node<DIM>* p_node = this->GetNode(node_index);

        // Get cell radius
        double cell_radius = p_node->GetRadius();

        // Begin code to approximate cell volume
        double averaged_cell_radius = 0.0;
        unsigned num_cells = 0;

        // Get the location of this node
        c_vector<double, DIM> node_i_location = GetNode(node_index)->rGetLocation();

        // Get the set of node indices corresponding to this cell's neighbours
        std::set<unsigned> neighbouring_node_indices = GetNeighbouringNodeIndices(node_index);

        // THe number of neighbours in equilibrium configuration, from sphere packing problem
        unsigned num_neighbours_equil;
        if (DIM==2)
        {
            num_neighbours_equil = 6;
        }
        else
        {
            assert(DIM==3);
            num_neighbours_equil = 12;
        }

        // Loop over this set
        for (std::set<unsigned>::iterator iter = neighbouring_node_indices.begin();
                iter != neighbouring_node_indices.end();
                ++iter)
        {
            Node<DIM>* p_node_j = this->GetNode(*iter);

            // Get the location of the neighbouring node
            c_vector<double, DIM> node_j_location = p_node_j->rGetLocation();

            double neighbouring_cell_radius = p_node_j->GetRadius();

            // If this throws then you may not be considering all cell interactions use a larger cut off length
            assert(cell_radius+neighbouring_cell_radius<mpNodesOnlyMesh->GetMaximumInteractionDistance());

            // Calculate the distance between the two nodes and add to cell radius
            double separation = norm_2(mpNodesOnlyMesh->GetVectorFromAtoB(node_j_location,node_i_location));

            if (separation < cell_radius+neighbouring_cell_radius)
            {
                // The effective radius is the mid point of the overlap
                averaged_cell_radius = averaged_cell_radius + cell_radius - (cell_radius+neighbouring_cell_radius-separation)/2.0;
                num_cells++;
            }
        }
        if (num_cells < num_neighbours_equil)
        {
            averaged_cell_radius += (num_neighbours_equil-num_cells)*cell_radius;

            averaged_cell_radius /= num_neighbours_equil;
        }
        else
        {
            averaged_cell_radius /= num_cells;
        }
        assert(averaged_cell_radius < mpNodesOnlyMesh->GetMaximumInteractionDistance()/2.0);

        cell_radius = averaged_cell_radius;

        // End code to approximate cell volume

        // Calculate cell volume from radius of cell
        double cell_volume = 0.0;
        if (DIM == 2)
        {
            cell_iter->GetCellData()->SetItem("boundary", 2*M_PI*cell_radius);
        }
        else if (DIM == 3)
        {
            cell_iter->GetCellData()->SetItem("boundary", 4*M_PI*cell_radius*cell_radius);
        }
    }
}

    template<unsigned DIM>
void BoundaryTrackingModifier<DIM>::OutputSimulationModifierParameters(out_stream& rParamsFile)
{
    // No parameters to output, so just call method on direct parent class
    AbstractCellBasedSimulationModifier<DIM>::OutputSimulationModifierParameters(rParamsFile);
}

// Explicit instantiation
template class BoundaryTrackingModifier<1>;
template class BoundaryTrackingModifier<2>;
template class BoundaryTrackingModifier<3>;

// Serialization for Boost >= 1.36
#include "SerializationExportWrapperForCpp.hpp"
EXPORT_TEMPLATE_CLASS_SAME_DIMS(BoundaryTrackingModifier)
