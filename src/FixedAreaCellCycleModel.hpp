#ifndef FIXEDAREACELLCYCLEMODEL_HPP_
#define FIXEDAREACELLCYCLEMODEL_HPP_

#include "AbstractSimpleCellCycleModel.hpp"

class FixedAreaCellCycleModel : public AbstractSimpleCellCycleModel
{
private:

    friend class boost::serialization::access;

    /**
     * Boost Serialization method for archiving/checkpointing
     * @param archive  The boost archive.
     * @param version  The current version of this class.
     */
    template<class Archive>
    void serialize(Archive & archive, const unsigned int version)
    {
        archive & boost::serialization::base_object<AbstractSimpleCellCycleModel>(*this);
        archive & mDivisionVolume;
    }

    double mDivisionVolume;

protected:
    void SetG1Duration();
public:

    /**
     * Constructor.
     */
    FixedAreaCellCycleModel();

    /**
     * Overridden UpdateCellCyclePhase() method.
     */
    void UpdateCellCyclePhase();

    /**
     * Overridden builder method to create new instances of
     * the cell-cycle model.
     *
     * @return new cell-cycle model
     *
     */
    AbstractCellCycleModel* CreateCellCycleModel();

    /**
     * @param divisionVolume
     */
    void SetDivisionVolume(double divisionVolume);

    // TODO: add getter for division volume

    /**
     * Outputs cell cycle model parameters to file.
     *
     * @param rParamsFile the file stream to which the parameters are output
     */
    virtual void OutputCellCycleModelParameters(out_stream& rParamsFile);
};

// Declare identifier for the serializer
#include "SerializationExportWrapper.hpp"
CHASTE_CLASS_EXPORT(FixedAreaCellCycleModel)

#endif // FIXEDAREACELLCYCLEMODEL_HPP_
