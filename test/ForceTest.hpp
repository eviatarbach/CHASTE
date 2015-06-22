#ifndef TESTRUNNINGVERTEXBASEDSIMULATIONSTUTORIAL_HPP_
#define TESTRUNNINGVERTEXBASEDSIMULATIONSTUTORIAL_HPP_

#include <cxxtest/TestSuite.h>
#include "CheckpointArchiveTypes.hpp"
#include "AbstractCellBasedTestSuite.hpp"

#include "CellsGenerator.hpp"
#include "OffLatticeSimulation.hpp"
#include "DifferentiatedCellProliferativeType.hpp"
#include "SmartPointers.hpp"

#include "StochasticDurationCellCycleModel.hpp"
#include "ContactInhibitionCellCycleModel.hpp"

#include "HoneycombVertexMeshGenerator.hpp"

#include "VertexBasedCellPopulation.hpp"

#include "NagaiHondaForce.hpp"
#include "AppliedForce.hpp"

#include "SimpleTargetAreaModifier.hpp"
#include "AncestorTrackingModifier.hpp"
#include "BoundaryTrackingModifier.hpp"
#include "ContactBoundaryTrackingModifier.hpp"
#include "GenerationTrackingModifier.hpp"

#include "WildTypeCellMutationState.hpp"
#include "PlaneBoundaryCondition.hpp"

#include "CellType.hpp"

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

#include "NagaiHondaMultipleDifferentialAdhesionForce.hpp"

#include "PetscSetupAndFinalize.hpp"

class TestForceSimulations : public AbstractCellBasedTestSuite
{
    public:
        void TestAppliedForce()
        {
            HoneycombVertexMeshGenerator generator(8, 8);
            MutableVertexMesh<2,2>* p_mesh = generator.GetMesh();
            p_mesh->SetCellRearrangementThreshold(0.1);

            MAKE_PTR(DifferentiatedCellProliferativeType, p_diff_type);
            std::vector<CellPtr> cells;

            CellsGenerator<StochasticDurationCellCycleModel, 2> cells_generator;
            cells_generator.GenerateBasic(cells, p_mesh->GetNumElements(), std::vector<unsigned>(), p_diff_type);

            VertexBasedCellPopulation<2> cell_population(*p_mesh, cells);

            boost::shared_ptr<AbstractCellProperty> type_1(new CellType(1, 5));
            boost::shared_ptr<AbstractCellProperty> type_2(new CellType(2, 5));
            boost::shared_ptr<AbstractCellProperty> label(new CellLabel(1));

            unsigned index = 0;
            for (AbstractCellPopulation<2>::Iterator cell_iter = cell_population.Begin();
                    cell_iter != cell_population.End();
                    ++cell_iter, index++)
            {
                if (index == 18)
                {
                    cell_iter->AddCellProperty(type_2);
                    //cell_iter->AddCellProperty(label);
                }
                else
                {
                    cell_iter->AddCellProperty(type_1);
                }
            }

            OffLatticeSimulation<2> simulator(cell_population);
            simulator.SetOutputDirectory("ForceTest");
            simulator.SetSamplingTimestepMultiple(10);
            simulator.SetEndTime(20.0);

            MAKE_PTR(AppliedForce<2>, a_force);
            std::map <unsigned, c_vector<double,2> > force_map;
            force_map[18][0] = 0.0;
            force_map[18][1] = 5.0;
            a_force->SetForceMap(force_map);
            simulator.AddForce(a_force);

            MAKE_PTR(NagaiHondaMultipleDifferentialAdhesionForce<2>, p_force);

            std::vector<std::vector<double> > adhesion_matrix(4, std::vector<double>(4, 0.0));

            adhesion_matrix[0][1] = adhesion_matrix[1][0] = 1.0;
            adhesion_matrix[1][1] = 2.0;
            adhesion_matrix[2][0] = adhesion_matrix[0][2] = 1.0;
            adhesion_matrix[2][2] = 2.0;
            adhesion_matrix[2][1] = adhesion_matrix[1][2] = 2.0;

            p_force->SetNagaiHondaAdhesionMatrix(adhesion_matrix);
            p_force->SetNagaiHondaDeformationEnergyParameter(10.0);
            p_force->SetNagaiHondaMembraneSurfaceEnergyParameter(10.0);
            simulator.AddForce(p_force);

            MAKE_PTR(SimpleTargetAreaModifier<2>, p_growth_modifier);
            simulator.AddSimulationModifier(p_growth_modifier);

            simulator.Solve();
        }
};

#endif /* TESTRUNNINGVERTEXBASEDSIMULATIONSTUTORIAL_HPP_ */
