#include "CellType.hpp"

CellType::CellType(unsigned type, unsigned colour, double areaDeformationParameter,
        double perimeterDeformationParameter)
    : AbstractCellProperty(),
    mType(type),
    mColour(colour),
    mAreaDeformationParameter(areaDeformationParameter),
    mPerimeterDeformationParameter(perimeterDeformationParameter)
{
    // 0 is reserved for the boundary
    assert(type != 0);
}

CellType::~CellType() {
}

unsigned CellType::GetType() const {
    return mType;
}

unsigned CellType::GetColour() const {
    return mColour;
}

double CellType::GetAreaDeformationParameter() const {
    return mAreaDeformationParameter;
}

double CellType::GetPerimeterDeformationParameter() const {
    return mPerimeterDeformationParameter;
}

#include "SerializationExportWrapperForCpp.hpp"
// Declare identifier for the serializer
CHASTE_CLASS_EXPORT(CellType)
