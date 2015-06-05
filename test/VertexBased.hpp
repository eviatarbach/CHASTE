#ifndef TESTRUNNINGVERTEXBASEDSIMULATIONSTUTORIAL_HPP_
#define TESTRUNNINGVERTEXBASEDSIMULATIONSTUTORIAL_HPP_

#include <cxxtest/TestSuite.h>
#include "CheckpointArchiveTypes.hpp"
#include "AbstractCellBasedTestSuite.hpp"

#include "CellsGenerator.hpp"
#include "OffLatticeSimulation.hpp"
#include "TransitCellProliferativeType.hpp"
#include "SmartPointers.hpp"

#include "StochasticDurationCellCycleModel.hpp"
#include "ContactInhibitionCellCycleModel.hpp"

#include "HoneycombVertexMeshGenerator.hpp"

#include "VertexBasedCellPopulation.hpp"

#include "NagaiHondaForce.hpp"

#include "SimpleTargetAreaModifier.hpp"
#include "AncestorTrackingModifier.hpp"
#include "BoundaryTrackingModifier.hpp"
#include "ContactBoundaryTrackingModifier.hpp"
#include "GenerationTrackingModifier.hpp"

#include "WildTypeCellMutationState.hpp"
#include "PlaneBoundaryCondition.hpp"

#include "CellLabel.hpp"

#include "PlaneBasedCellKiller.hpp"

#include "XMLCellWriter.hpp"
#include "MutableVertexMesh.hpp"

#include "MeshBasedCellPopulation.hpp"
#include "CellMutationStatesCountWriter.hpp"
#include "WildTypeCellMutationState.hpp"
#include "HoneycombMeshGenerator.hpp"
#include "OutputFileHandler.hpp"
#include "GeneralisedLinearSpringForce.hpp"
#include "SimulationTime.hpp"
#include "MutableMesh.hpp"
#include "MutableVertexMesh.hpp"
#include "PlaneBoundaryCondition.hpp"
#include "VolumeTrackingModifier.hpp"

#include "PetscSetupAndFinalize.hpp"

class TestRunningVertexBasedSimulations : public AbstractCellBasedTestSuite
{
    public:
        void TestMonolayerSimple()
        {

            HoneycombVertexMeshGenerator generator(2, 2);    // Parameters are: cells across, cells up
            MutableVertexMesh<2,2>* p_mesh = generator.GetMesh();

            std::vector<CellPtr> cells;
            MAKE_PTR(TransitCellProliferativeType, p_transit_type);

            CellsGenerator<StochasticDurationCellCycleModel, 2> cells_generator;
            cells_generator.GenerateBasicRandom(cells, p_mesh->GetNumElements(), p_transit_type);

            VertexBasedCellPopulation<2> cell_population(*p_mesh, cells);

            cell_population.AddCellWriter<XMLCellWriter>();

            OffLatticeSimulation<2> simulator(cell_population);
            simulator.SetOutputDirectory("VertexBasedMonolayerSimple");
            simulator.SetEndTime(10.0);

            simulator.SetSamplingTimestepMultiple(50);

            MAKE_PTR(NagaiHondaForce<2>, p_force);
            simulator.AddForce(p_force);

            MAKE_PTR(SimpleTargetAreaModifier<2>, p_growth_modifier);
            simulator.AddSimulationModifier(p_growth_modifier);

            MAKE_PTR(AncestorTrackingModifier<2>, p_ancestor_modifier);
            simulator.AddSimulationModifier(p_ancestor_modifier);

            MAKE_PTR(BoundaryTrackingModifier<2>, p_boundary_modifier);
            simulator.AddSimulationModifier(p_boundary_modifier);

            MAKE_PTR_ARGS(GenerationTrackingModifier<2>, p_generation_modifier, (50));
            simulator.AddSimulationModifier(p_generation_modifier);

            MAKE_PTR(ContactBoundaryTrackingModifier<2>, p_contact_boundary_modifier);
            simulator.AddSimulationModifier(p_contact_boundary_modifier);

            simulator.Solve();
        }
        void TestMonolayerWithContactInhibition()
        {
            HoneycombVertexMeshGenerator generator(2, 2);
            MutableVertexMesh<2,2>* p_mesh = generator.GetMesh();

            MAKE_PTR(WildTypeCellMutationState, p_state);
            MAKE_PTR(TransitCellProliferativeType, p_transit_type);
            std::vector<CellPtr> cells;
            for (unsigned i=0; i<p_mesh->GetNumElements(); i++)
            {
                ContactInhibitionCellCycleModel* p_cycle_model = new ContactInhibitionCellCycleModel();
                p_cycle_model->SetDimension(2);
                p_cycle_model->SetBirthTime(-(double)i - 2.0); // So all out of M phase
                p_cycle_model->SetQuiescentVolumeFraction(0.9);
                p_cycle_model->SetEquilibriumVolume(1.0);

                CellPtr p_cell(new Cell(p_state, p_cycle_model));
                p_cell->SetCellProliferativeType(p_transit_type);
                p_cell->InitialiseCellCycleModel();
                cells.push_back(p_cell);
            }

            VertexBasedCellPopulation<2> cell_population(*p_mesh, cells);
            cell_population.AddCellPopulationCountWriter<CellMutationStatesCountWriter>();
            cell_population.AddCellWriter<XMLCellWriter>();

            OffLatticeSimulation<2> simulator(cell_population);
            simulator.SetOutputDirectory("VertexBasedMonolayerWithContactInhibition");
            simulator.SetSamplingTimestepMultiple(50);
            simulator.SetEndTime(10.0);

            MAKE_PTR(VolumeTrackingModifier<2>, p_modifier);
            simulator.AddSimulationModifier(p_modifier);

            MAKE_PTR(NagaiHondaForce<2>, p_nagai_honda_force);
            simulator.AddForce(p_nagai_honda_force);

            MAKE_PTR(SimpleTargetAreaModifier<2>, p_growth_modifier);
            simulator.AddSimulationModifier(p_growth_modifier);

            MAKE_PTR(AncestorTrackingModifier<2>, p_ancestor_modifier);
            simulator.AddSimulationModifier(p_ancestor_modifier);

            MAKE_PTR(BoundaryTrackingModifier<2>, p_boundary_modifier);
            simulator.AddSimulationModifier(p_boundary_modifier);

            MAKE_PTR_ARGS(GenerationTrackingModifier<2>, p_generation_modifier, (50));
            simulator.AddSimulationModifier(p_generation_modifier);

            MAKE_PTR(ContactBoundaryTrackingModifier<2>, p_contact_boundary_modifier);
            simulator.AddSimulationModifier(p_contact_boundary_modifier);

            simulator.Solve();
        }

};

#endif /* TESTRUNNINGVERTEXBASEDSIMULATIONSTUTORIAL_HPP_ */
