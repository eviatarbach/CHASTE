#ifndef CONTACTDEPENDENTFORCE_HPP_
#define CONTACTDEPENDENTFORCE_HPP_

#include "ChasteSerialization.hpp"
#include <boost/serialization/base_object.hpp>

#include "AbstractForce.hpp"

/**
 * A chemotactic force class.
 */
template<unsigned DIM>
class ContactDependentForce  : public AbstractForce<DIM>
{
friend class TestForces;

private:
    c_vector<double,DIM> mForce;

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
    }

public:

    /**
     * Constructor.
     */
    ContactDependentForce();

    /**
     * Destructor.
     */
    ~ContactDependentForce();

    void SetForce(c_vector<double,DIM> force);

    /**
     * Overridden AddForceContribution() method.
     *
     * @param rCellPopulation reference to the cell population
     *
     * Fc = chi(C,|gradC|) gradC/|gradC|  (if |gradC|>0, else Fc = 0)
     *
     */
    void AddForceContribution(AbstractCellPopulation<DIM,DIM>& rCellPopulation);

    /**
     * Overridden OutputForceParameters() method.
     *
     * @param rParamsFile the file stream to which the parameters are output
     */
    void OutputForceParameters(out_stream& rParamsFile);
};

#include "SerializationExportWrapper.hpp"
EXPORT_TEMPLATE_CLASS_SAME_DIMS(ContactDependentForce)

#endif /*CONTACTDEPENDENTFORCE_HPP_*/
