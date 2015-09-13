#ifndef TESTTISSUEINVASION_HPP_
#define TESTTISSUEINVASION_HPP_

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
#include "CylindricalHoneycombVertexMeshGenerator.hpp"
#include "VertexBasedCellPopulation.hpp"

#include "NagaiHondaForce.hpp"
#include "DiffusionForce.hpp"

#include "ContactInhibitionTargetAreaModifier.hpp"
#include "SimpleTargetAreaModifier.hpp"
#include "AncestorTrackingModifier.hpp"
#include "BoundaryTrackingModifier.hpp"
#include "ContactBoundaryTrackingModifier.hpp"
#include "GenerationTrackingModifier.hpp"
#include "PressureTrackingModifier.hpp"
#include "VolumeTrackingModifier.hpp"

#include "WildTypeCellMutationState.hpp"
#include "PlaneBoundaryCondition.hpp"

#include "CellType.hpp"
#include "CellLabel.hpp"

#include "PlaneBasedCellKiller.hpp"

#include "XMLCellWriter.hpp"
#include "MutableVertexMesh.hpp"
#include "AbstractMesh.hpp"

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

#include "NagaiHondaMultipleDifferentialAdhesionForce.hpp"
#include "FixedAreaCellCycleModel.hpp"
#include "ContactDependentWhaForce.hpp"
#include "CellBasedConfigTestSuite.hpp"
#include "RandomNumberGenerator.hpp"

#include <sstream>

#define SSTR( x ) dynamic_cast< std::ostringstream & >( \
                ( std::ostringstream() << std::dec << x ) ).str()

class TestTissueInvasion : public CellBasedConfigTestSuite
{
    public:
        void TestMonolayerSimple()
        {
            CylindricalHoneycombVertexMeshGenerator generator((int) config["cells_across"], (int) config["cells_up"]);    // Parameters are: cells across, cells up
            Cylindrical2dVertexMesh* p_mesh = generator.GetCylindricalMesh();
            //HoneycombVertexMeshGenerator generator(20, 10);
            //MutableVertexMesh<2,2>* p_mesh = generator.GetMesh();
            RandomNumberGenerator* p_gen = RandomNumberGenerator::Instance();

            std::vector<CellPtr> cells;
            MAKE_PTR(WildTypeCellMutationState, p_state);
            MAKE_PTR(TransitCellProliferativeType, p_transit_type);

            boost::shared_ptr<AbstractCellProperty> type_1(new CellType(1, 5, config["area_deformation_parameter"], config["perimeter_deformation_parameter"]));
            boost::shared_ptr<AbstractCellProperty> type_2(new CellType(2, 5, config["area_deformation_parameter"], config["perimeter_deformation_parameter"]));
            boost::shared_ptr<AbstractCellProperty> label_1(new CellLabel(3));
            boost::shared_ptr<AbstractCellProperty> label_2(new CellLabel(1));
            for (unsigned i=0; i<p_mesh->GetNumElements(); i++)
            {
                FixedAreaCellCycleModel* p_cycle_model = new FixedAreaCellCycleModel();
                p_cycle_model->SetDimension(2);
                p_cycle_model->SetBirthTime(0.0); // So all out of M phase
                p_cycle_model->SetDivisionVolume(config["division_volume"]);

                CellPtr p_cell(new Cell(p_state, p_cycle_model));
                p_cell->SetCellProliferativeType(p_transit_type);
                p_cell->InitialiseCellCycleModel();

                cells.push_back(p_cell);
            }

            //CellsGenerator<StochasticDurationCellCycleModel, 2> cells_generator;
            //cells_generator.GenerateBasicRandom(cells, p_mesh->GetNumElements(), p_transit_type);

            VertexBasedCellPopulation<2> cell_population(*p_mesh, cells);

            cell_population.AddCellWriter<XMLCellWriter>();

            for (AbstractCellPopulation<2>::Iterator cell_iter = cell_population.Begin();
                    cell_iter != cell_population.End();
                    ++cell_iter)
            {
                if (p_gen->ranf() < config["type1_fraction"])
                {
                    cell_iter->AddCellProperty(type_1);
                    cell_iter->AddCellProperty(label_1);
                }
                else
                {
                    cell_iter->AddCellProperty(type_2);
                    cell_iter->AddCellProperty(label_2);
                }
            }

            OffLatticeSimulation<2> simulator(cell_population);
            if (simulation_id == UNSIGNED_UNSET)
            {
                simulator.SetOutputDirectory("TestTissueInvasion");
            }
            else
            {
                simulator.SetOutputDirectory(SSTR("TestTissueInvasion_" << simulation_id));
            }
            simulator.SetEndTime(config["end_time"]);
            simulator.SetDt(config["dt"]);

            simulator.SetSamplingTimestepMultiple((int) config["sampling_rate"]);

            MAKE_PTR(NagaiHondaMultipleDifferentialAdhesionForce<2>, p_force);
            std::vector<std::vector<double> > adhesion_matrix(4, std::vector<double>(4, 0.0));
            adhesion_matrix[1][1] = config["adhesion11"];
            adhesion_matrix[0][1] = adhesion_matrix[1][0] = config["adhesion01"];
            adhesion_matrix[1][2] = adhesion_matrix[2][1] = config["adhesion12"];
            adhesion_matrix[0][2] = adhesion_matrix[2][0] = config["adhesion02"];
            adhesion_matrix[2][2] = config["adhesion22"];

            p_force->SetNagaiHondaAdhesionMatrix(adhesion_matrix);
            p_force->SetNagaiHondaDeformationEnergyParameter(config["area_deformation_parameter"]);
            p_force->SetNagaiHondaMembraneSurfaceEnergyParameter(config["perimeter_deformation_parameter"]);
            simulator.AddForce(p_force);

            MAKE_PTR(BoundaryTrackingModifier<2>, p_boundary_modifier);
            simulator.AddSimulationModifier(p_boundary_modifier);

            MAKE_PTR(ContactBoundaryTrackingModifier<2>, p_contact_boundary_modifier);
            simulator.AddSimulationModifier(p_contact_boundary_modifier);

            MAKE_PTR(VolumeTrackingModifier<2>, p_volume_modifier);
            simulator.AddSimulationModifier(p_volume_modifier);

            MAKE_PTR(AncestorTrackingModifier<2>, p_ancestor_modifier);
            simulator.AddSimulationModifier(p_ancestor_modifier);

            MAKE_PTR_ARGS(GenerationTrackingModifier<2>, p_generation_modifier, (1));
            simulator.AddSimulationModifier(p_generation_modifier);

            MAKE_PTR(ContactDependentWhaForce<2>, a_force);
            c_vector<double,2> force;
            force[0] = 0.0;
            force[1] = config["wha_force"];
            a_force->SetForce(force);
            simulator.AddForce(a_force);

            MAKE_PTR(DiffusionForce<2>, p_dforce);
            p_dforce->SetAbsoluteTemperature(config["diffusion_temperature"]);
            simulator.AddForce(p_dforce);

            // Has to be added after volume modifier!
            MAKE_PTR(ContactInhibitionTargetAreaModifier<2>, p_growth_modifier);
            p_growth_modifier->SetScalingParameter(config["growth_scaling_parameter"]);
            simulator.AddSimulationModifier(p_growth_modifier);

            MAKE_PTR(PressureTrackingModifier<2>, p_pressure_modifier);
            p_pressure_modifier->SetDeformationParameter(1.0);
            simulator.AddSimulationModifier(p_pressure_modifier);

            c_vector<double,2> point = zero_vector<double>(2);
            c_vector<double,2> normal = zero_vector<double>(2);
            normal(1) = -1.0;
            MAKE_PTR_ARGS(PlaneBoundaryCondition<2>, p_bc, (&cell_population, point, normal));
            simulator.AddCellPopulationBoundaryCondition(p_bc);
/*
            c_vector<double,2> point2 = zero_vector<double>(2);
            c_vector<double,2> normal2 = zero_vector<double>(2);
            normal2(0) = -1.0;
            MAKE_PTR_ARGS(PlaneBoundaryCondition<2>, p_bc2, (&cell_population, point2, normal2));
            simulator.AddCellPopulationBoundaryCondition(p_bc2);

            c_vector<double,2> point3 = zero_vector<double>(2);
            c_vector<double,2> normal3 = zero_vector<double>(2);
            point3(0) = 20.5;
            normal3(0) = 1.0;
            MAKE_PTR_ARGS(PlaneBoundaryCondition<2>, p_bc3, (&cell_population, point3, normal3));
            simulator.AddCellPopulationBoundaryCondition(p_bc3);
*/
            simulator.Solve();
        }
};

#endif /* TESTTISSUEINVASION_HPP_ */
