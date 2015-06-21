#ifndef NAGAIHONDACONTRACTILITYFORCE_HPP_
#define NAGAIHONDACONTRACTILITYFORCE_HPP_

#include "ChasteSerialization.hpp"
#include <boost/serialization/base_object.hpp>
#include "Exception.hpp"

#include "AbstractForce.hpp"
#include "VertexBasedCellPopulation.hpp"

#include <iostream>

/**
 * A force class for use in vertex-based simulations, based on a mechanical
 * model proposed by T. Nagai and H. Honda ("A dynamic cell model for the formation
 * of epithelial tissues", Philosophical Magazine Part B 81:699-719). In contrast to the force proposed
 * by Nagai and Honda this force has an additional force term implemented that scales with the perimeter
 * of a cell to simulate the surface membrane energy. This particular perimeter force term in turn differs from the one
 * proposed by Farhadifar et al (2007) in the sense that it employs a target perimeter.
 *
 * Each of the model parameter member variables are rescaled such that mDampingConstantNormal
 * takes the default value 1, whereas Nagai and Honda (who denote the parameter by
 * nu) take the value 0.01.
 */
template<unsigned DIM>
class NagaiHondaContractilityForce  : public AbstractForce<DIM>
{
friend class TestForces;

private:

    friend class boost::serialization::access;
    /**
     * Boost Serialization method for archiving/checkpointing.
     * Archives the object and its member variables.
     *
     * @param archive  The boost archive.
     * @param version  The current version of this class.
     */
    template<class Archive>
    void serialize(Archive & archive, const unsigned int version)
    {
        archive & boost::serialization::base_object<AbstractForce<DIM> >(*this);
        archive & mNagaiHondaDeformationEnergyParameter;
        archive & mNagaiHondaMembraneSurfaceEnergyParameter;
        archive & mNagaiHondaCellCellAdhesionEnergyParameter;
        archive & mNagaiHondaCellBoundaryAdhesionEnergyParameter;
    }

protected:

    /**
     * Cell deformation energy parameter. Has units of kg s^-2 (cell size at equilibrium rest length)^-1.
     */
    double mNagaiHondaDeformationEnergyParameter;

    /**
     * Cell membrane energy parameter. Has units of kg (cell size at equilibrium rest length) s^-2.
     */
    double mNagaiHondaMembraneSurfaceEnergyParameter;

    /**
     * Cell-cell adhesion energy parameter. Has has units of kg (cell size at equilibrium rest length)^2 s^-2.
     * This parameter corresponds to 1/2 of the Lambda parameter in forces proposed by Farhadifar et al (2007).
     * This slight difference comes from the fact that when we apply the forces to a particular node, each
     * edge is visited twice - and hence the force originating from that edge is applied twice.
     */
    double mNagaiHondaCellCellAdhesionEnergyParameter;

    /**
     * Cell-boundary adhesion energy parameter. Has units of kg (cell size at equilibrium rest length)^2 s^-2.
     */
    double mNagaiHondaCellBoundaryAdhesionEnergyParameter;


public:

    /**
     * Constructor.
     */
    NagaiHondaContractilityForce();

    /**
     * Destructor.
     */
    virtual ~NagaiHondaContractilityForce();

    /**
     * Overridden AddForceContribution() method.
     *
     * Calculates the force on each node in the vertex-based cell population based on the
     * Nagai Honda model.
     *
     * @param rCellPopulation reference to the cell population
     */
    virtual void AddForceContribution(AbstractCellPopulation<DIM>& rCellPopulation);

    /**
     * Get the adhesion parameter for the edge between two given nodes.
     *
     * @param pNodeA one node
     * @param pNodeB the other node
     * @param rVertexCellPopulation reference to the cell population
     *
     * @return the adhesion parameter for this edge.
     */
    virtual double GetAdhesionParameter(Node<DIM>* pNodeA, Node<DIM>* pNodeB, VertexBasedCellPopulation<DIM>& rVertexCellPopulation);

    /**
     * @return mNagaiHondaContractilityDeformationEnergyParameter
     */
    double GetNagaiHondaDeformationEnergyParameter();

    /**
     * @return mNagaiHondaContractilityMembraneSurfaceEnergyParameter
     */
    double GetNagaiHondaMembraneSurfaceEnergyParameter();

    /**
     * @return mCellCellAdhesionEnergyParameter
     */
    double GetNagaiHondaCellCellAdhesionEnergyParameter();

    /**
     * @return mNagaiHondaContractilityCellBoundaryAdhesionEnergyParameter
     */
    double GetNagaiHondaCellBoundaryAdhesionEnergyParameter();

    /**
     * Set mNagaiHondaContractilityDeformationEnergyParameter.
     *
     * @param NagaiHondaContractilityDeformationEnergyParameter the new value of mNagaiHondaContractilityDeformationEnergyParameter
     */
    void SetNagaiHondaDeformationEnergyParameter(double NagaiHondaDeformationEnergyParameter);

    /**
     * Set mNagaiHondaContractilityMembraneSurfaceEnergyParameter.
     *
     * @param NagaiHondaContractilityMembraneSurfaceEnergyParameter the new value of mNagaiHondaContractilityMembraneSurfaceEnergyParameter
     */
    void SetNagaiHondaMembraneSurfaceEnergyParameter(double NagaiHondaMembraneSurfaceEnergyParameter);

    /**
     * Set mNagaiHondaContractilityCellCellAdhesionEnergyParameter. This parameter corresponds to 1/2 of the Lambda parameter in the forces by
     * Farhadifar et al (2007).
     *
     * @param NagaiHondaContractilityCellCellAdhesionEnergyEnergyParameter the new value of mNagaiHondaContractilityCellCellAdhesionEnergyParameter
     */
    void SetNagaiHondaCellCellAdhesionEnergyParameter(double NagaiHondaCellCellAdhesionEnergyEnergyParameter);

    /**
     * Set mNagaiHondaContractilityCellBoundaryAdhesionEnergyParameter.
     *
     * @param NagaiHondaContractilityCellBoundaryAdhesionEnergyParameter the new value of mNagaiHondaContractilityCellBoundaryAdhesionEnergyParameter
     */
    void SetNagaiHondaCellBoundaryAdhesionEnergyParameter(double NagaiHondaCellBoundaryAdhesionEnergyParameter);


    /**
     * Overridden OutputForceParameters() method.
     *
     * @param rParamsFile the file stream to which the parameters are output
     */
    void OutputForceParameters(out_stream& rParamsFile);
};

#include "SerializationExportWrapper.hpp"
EXPORT_TEMPLATE_CLASS_SAME_DIMS(NagaiHondaContractilityForce)

#endif /*NAGAIHONDACONTRACTILITYFORCE_HPP_*/
