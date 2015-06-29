#ifndef CELLTYPE_HPP_
#define CELLTYPE_HPP_

#include <boost/shared_ptr.hpp>
#include "AbstractCellProperty.hpp"
#include "ChasteSerialization.hpp"
#include <boost/serialization/base_object.hpp>

/**
 * Cell label class.
 *
 * Each Cell owns a CellPropertyCollection, which may include a shared pointer
 * to an object of this type. When a Cell that is labelled divides, the daughter
 * cells are both labelled.
 *
 * The CellType object keeps track of the number of cells that have the label, as well
 * as what colour should be used by the visualizer to display cells with the label.
 */
class CellType : public AbstractCellProperty
{
protected:

    unsigned mType;

    /**
     * Colour for use by visualizer.
     */
    unsigned mColour;

    double mAreaDeformationParameter;

    double mPerimeterDeformationParameter;

private:

    /** Needed for serialization. */
    friend class boost::serialization::access;
    /**
     * Archive the member variables.
     *
     * @param archive the archive
     * @param version the current version of this class
     */
    template<class Archive>
    void serialize(Archive & archive, const unsigned int version)
    {
        archive & boost::serialization::base_object<AbstractCellProperty>(*this);
        archive & mType;
        archive & mColour;
    }

public:

    /**
     * Constructor.
     *
     * @param colour  what colour cells with this label should be in the visualizer (defaults to 5)
     */
    CellType(unsigned type=1, unsigned colour=5, double areaDeformationParameter=100.0,
            double perimeterDeformationParameter=10.0);

    /**
     * Destructor.
     */
    virtual ~CellType();

    unsigned GetType() const;

    /**
     * @return #mColour.
     */
    unsigned GetColour() const;

    double GetAreaDeformationParameter() const;

    double GetPerimeterDeformationParameter() const;
};

#include "SerializationExportWrapper.hpp"
// Declare identifier for the serializer
CHASTE_CLASS_EXPORT(CellType)

#endif /* CELLTYPE_HPP_ */
