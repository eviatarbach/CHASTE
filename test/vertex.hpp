#ifndef TESTRUNNINGVERTEXBASEDSIMULATIONSTUTORIAL_HPP_
#define TESTRUNNINGVERTEXBASEDSIMULATIONSTUTORIAL_HPP_

/*
 * = Examples showing how to create, run and visualize vertex-based simulations =
 *
 * EMPTYLINE
 *
 * == Introduction ==
 *
 * EMPTYLINE
 *
 * In this tutorial we show how Chaste can be used to create, run and visualize vertex-based simulations.
 * Full details of the mechanical model proposed by T. Nagai and H. Honda ("A dynamic cell model for
 * the formation of epithelial tissues", Philosophical Magazine Part B 81:699-719).
 *
 * EMPTYLINE
 *
 * == The test ==
 *
 * EMPTYLINE
 *
 * As in previous cell-based Chaste tutorials, we begin by including the necessary header files.
 */
#include <cxxtest/TestSuite.h>
#include "CheckpointArchiveTypes.hpp"
#include "AbstractCellBasedTestSuite.hpp"

/* The remaining header files define classes that will be used in the cell-based
 * simulation. We have encountered some of these header files in previous cell-based
 * Chaste tutorials. */
#include "CellsGenerator.hpp"
#include "OffLatticeSimulation.hpp"
#include "TransitCellProliferativeType.hpp"
#include "SmartPointers.hpp"
/* The next header file defines the cell cycle model. */
#include "StochasticDurationCellCycleModel.hpp"
/* The next two header files define a helper class for generating suitable meshes: one planar and one periodic. */
#include "HoneycombVertexMeshGenerator.hpp"
/* The next header file defines a vertex-based {{{CellPopulation}}} class.*/
#include "VertexBasedCellPopulation.hpp"
/* The next header file defines a force law for describing the mechanical interactions
 * between neighbouring cells in the cell population, subject to each vertex.
 */
#include "NagaiHondaForce.hpp"
/* This force law assumes that cells possess a "target area" property which determines the size of each
 * cell in the simulation. In order to assign target areas to cells and update them in each time step, we need
 * the next header file.
 */
#include "SimpleTargetAreaModifier.hpp"
#include "AncestorTrackingModifier.hpp"
#include "BoundaryTrackingModifier.hpp"
#include "ContactBoundaryTrackingModifier.hpp"

/* The next header file defines a boundary condition for the cells.*/
#include "PlaneBoundaryCondition.hpp"
/* The next header file defines a cell killer, which specifies how cells are removed from the simulation.*/
#include "PlaneBasedCellKiller.hpp"

#include "XMLCellWriter.hpp"

#include "PetscSetupAndFinalize.hpp"

/* Next, we define the test class, which inherits from {{{AbstractCellBasedTestSuite}}}
 * and defines some test methods.
 */
class TestRunningVertexBasedSimulationsTutorial : public AbstractCellBasedTestSuite
{
    public:
        /* EMPTYLINE
         *
         * == Test 1 - a basic vertex-based simulation ==
         *
         * EMPTYLINE
         *
         * In the first test, we run a simple vertex-based simulation, in which we create a monolayer
         * of cells, using a mutable vertex mesh. Each cell is assigned a stochastic cell-cycle model.
         */
        void TestMonolayer() throw(Exception)
        {
            /* First, we generate a vertex mesh. To create a {{{MutableVertexMesh}}}, we can use
             * the {{{HoneycombVertexMeshGenerator}}}. This generates a honeycomb-shaped mesh,
             * in which all nodes are equidistant. Here the first and second arguments
             * define the size of the mesh - we have chosen a mesh that is 2 elements (i.e.
             * cells) wide, and 2 elements high.
             */
            HoneycombVertexMeshGenerator generator(1, 1);    // Parameters are: cells across, cells up
            MutableVertexMesh<2,2>* p_mesh = generator.GetMesh();

            /* Having created a mesh, we now create a {{{std::vector}}} of {{{CellPtr}}}s.
             * To do this, we use the `CellsGenerator` helper class, which is templated over the type
             * of cell model required (here {{{StochasticDurationCellCycleModel}}})
             * and the dimension. We create an empty vector of cells and pass this into the
             * method along with the mesh. The second argument represents the size of that the vector
             * {{{cells}}} should become - one cell for each element, the third argument specifies
             * the proliferative type of the cell. */
            std::vector<CellPtr> cells;
            MAKE_PTR(TransitCellProliferativeType, p_transit_type);
            CellsGenerator<StochasticDurationCellCycleModel, 2> cells_generator;
            cells_generator.GenerateBasicRandom(cells, p_mesh->GetNumElements(), p_transit_type);

            /* Now we have a mesh and a set of cells to go with it, we can create a {{{CellPopulation}}}.
             * In general, this class associates a collection of cells with a mesh.
             * For this test, because we have a {{{MutableVertexMesh}}}, we use a particular type of
             * cell population called a {{{VertexBasedCellPopulation}}}.
             */
            VertexBasedCellPopulation<2> cell_population(*p_mesh, cells);

            cell_population.AddCellWriter<XMLCellWriter>();

            /* We then pass the cell population into an {{{OffLatticeSimulation}}},
             * and set the output directory and end time. */
            OffLatticeSimulation<2> simulator(cell_population);
            simulator.SetOutputDirectory("VertexBasedMonolayer");
            simulator.SetEndTime(50.0);
            /*
             * For longer simulations, we may not want to output the results
             * every time step. In this case we can use the following method,
             * to print results every 50 time steps instead. As the default time step
             * used by the simulator (for vertex based simulations), is 0.02 hours, this method will cause the
             * simulator to print results every 6 minutes (i.e. 0.1 hours).
             */
            simulator.SetSamplingTimestepMultiple(50);

            /* We must now create one or more force laws, which determine the mechanics of the vertices
             * of each cell in a cell population. For this test, we use one force law, based on the
             * Nagai-Honda mechanics, and pass it to the {{{OffLatticeSimulation}}}.
             * For a list of possible forces see subclasses of {{{AbstractForce}}}.
             * These can be found in the inheritance diagram, here, [class:AbstractForce AbstractForce].
             * Note that some of these forces are not compatible with vertex-based simulations see the specific class documentation for details,
             * if you try to use an incompatible class then you will receive a warning.
             */
            MAKE_PTR(NagaiHondaForce<2>, p_force);
            simulator.AddForce(p_force);

            /* A {{{NagaiHondaForce}}} assumes that each cell has a target area. The target areas of cells are used to determine pressure
             * forces on each vertex and eventually determine the size of each cell in the simulation. In order to assign target areas to cells
             * and update them in each time step we add a {{{SimpleTargetAreaModifier}}} to the simulation, which inherits from
             *  {{{AbstractTargetAreaModifier}}}.
             */
            MAKE_PTR(SimpleTargetAreaModifier<2>, p_growth_modifier);
            simulator.AddSimulationModifier(p_growth_modifier);

            MAKE_PTR(AncestorTrackingModifier<2>, p_ancestor_modifier);
            simulator.AddSimulationModifier(p_ancestor_modifier);

            MAKE_PTR(BoundaryTrackingModifier<2>, p_boundary_modifier);
            simulator.AddSimulationModifier(p_boundary_modifier);

            MAKE_PTR(ContactBoundaryTrackingModifier<2>, p_contact_boundary_modifier);
            simulator.AddSimulationModifier(p_contact_boundary_modifier);

            /* To run the simulation, we call {{{Solve()}}}. */
            simulator.Solve();

            /* The next two lines are for test purposes only and are not part of this tutorial. If different simulation input parameters are being explored
             * the lines should be removed.*/
            TS_ASSERT_EQUALS(cell_population.GetNumRealCells(), 4u);
            TS_ASSERT_DELTA(SimulationTime::Instance()->GetTime(), 1.0, 1e-10);
        }

        /*
         * EMPTYLINE
         *
         * To visualize the results, open a new terminal, {{{cd}}} to the Chaste directory,
         * then {{{cd}}} to {{{anim}}}. Then do: {{{java Visualize2dVertexCells /tmp/$USER/testoutput/VertexBasedMonolayer/results_from_time_0}}}.
         * We may have to do: {{{javac Visualize2dVertexCells.java}}} beforehand to create the
         * java executable.
         *
         * EMPTYLINE
         */
};

#endif /* TESTRUNNINGVERTEXBASEDSIMULATIONSTUTORIAL_HPP_ */
