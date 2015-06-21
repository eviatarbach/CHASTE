#ifndef CONTACTINHIBITIONTARGETAREAMODIFIER_HPP_
#define CONTACTINHIBITIONTARGETAREAMODIFIER_HPP_

#include "ChasteSerialization.hpp"
#include "AbstractTargetAreaModifier.hpp"

/**
 * A modifier class in which the target area property of each cell is updated.
 * It is used to implement growth in vertex-based simulations.
 *
 * \todo Improve class documentation by describing precisely how the target
 * area depends on the cell's status
 */
template<unsigned DIM>
class ContactInhibitionTargetAreaModifier : public AbstractTargetAreaModifier<DIM>
{
    double mScalingParameter;

    /** Needed for serialization. */
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
        archive & boost::serialization::base_object<AbstractTargetAreaModifier<DIM> >(*this);
    }

public:

    /**
     * Default constructor.
     */
    ContactInhibitionTargetAreaModifier();

    /**
     * Destructor.
     */
    virtual ~ContactInhibitionTargetAreaModifier();

    void SetScalingParameter(double scalingParameter);

    /**
     * Overridden UpdateTargetAreaOfCell() method.
     *
     * @param pCell pointer to the cell
     */
    virtual void UpdateTargetAreaOfCell(const CellPtr pCell);

    /**
     * Overridden OutputSimulationModifierParameters() method.
     * Output any simulation modifier parameters to file.
     *
     * @param rParamsFile the file stream to which the parameters are output
     */
    void OutputSimulationModifierParameters(out_stream& rParamsFile);
};

#include "SerializationExportWrapper.hpp"
EXPORT_TEMPLATE_CLASS_SAME_DIMS(ContactInhibitionTargetAreaModifier)

#endif /*CONTACTINHIBITIONTARGETAREAMODIFIER_HPP_*/
