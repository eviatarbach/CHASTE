#ifndef NAGAIHONDAMULTIPLEDIFFERENTIALADHESIONFORCE_HPP_
#define NAGAIHONDAMULTIPLEDIFFERENTIALADHESIONFORCE_HPP_

#include "ChasteSerialization.hpp"
#include <boost/serialization/base_object.hpp>

#include "NagaiHondaContractilityForce.hpp"

#include <iostream>

/**
 * A force class for use in vertex-based simulations, based on a model
 * model proposed by T. Nagai and H. Honda ("A dynamic cell model for
 * the formation of epithelial tissues", Philosophical Magazine Part B
 * 81:699-719) to include differential adhesion between normal and
 * labelled cells. To include differential adhesion we override the
 * GetAdhesionParameter() method.
 *
 * Each of the model parameter member variables are rescaled such that
 * mDampingConstantNormal takes the default value 1, whereas Nagai and
 * Honda (who denote the parameter by nu) take the value 0.01.
 */
template<unsigned DIM>
class NagaiHondaMultipleDifferentialAdhesionForce  : public NagaiHondaContractilityForce<DIM>
{
private:

    /**
     * Cell-cell adhesion energy parameters for two cells. 0 represents the boundary.
     * Has units of kg (cell size at equilibrium rest length)^2 s^-2.
     */
    std::vector<std::vector<double> > mAdhesionMatrix;

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
        archive & boost::serialization::base_object<NagaiHondaContractilityForce<DIM> >(*this);
        archive & mAdhesionMatrix;
    }

public:

    /**
     * Constructor.
     */
    NagaiHondaMultipleDifferentialAdhesionForce();

    /**
     * Destructor.
     */
    virtual ~NagaiHondaMultipleDifferentialAdhesionForce();

    /**
     * Overridden GetAdhesionParameter() method.
     *
     * Get the adhesion parameter for the edge between two given nodes. Depends
     * on the type of cells attached to the elements.
     *
     * @param pNodeA one node
     * @param pNodeB the other node
     * @param rVertexCellPopulation reference to the cell population
     *
     * @return the adhesion parameter for this edge.
     */
    virtual double GetAdhesionParameter(Node<DIM>* pNodeA, Node<DIM>* pNodeB, VertexBasedCellPopulation<DIM>& rVertexCellPopulation);

    void SetNagaiHondaAdhesionMatrix(std::vector<std::vector<double> > adhesionMatrix);

    /**
     * Overridden OutputForceParameters() method.
     *
     * @param rParamsFile the file stream to which the parameters are output
     */
    void OutputForceParameters(out_stream& rParamsFile);
};

#include "SerializationExportWrapper.hpp"
EXPORT_TEMPLATE_CLASS_SAME_DIMS(NagaiHondaMultipleDifferentialAdhesionForce)

#endif /*NAGAIHONDAMULTIPLEDIFFERENTIALADHESIONFORCE_HPP_*/
