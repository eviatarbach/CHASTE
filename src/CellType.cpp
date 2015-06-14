#include "CellType.hpp"

CellType::CellType(unsigned type, unsigned colour)
    : AbstractCellProperty(),
    mType(type),
    mColour(colour) {
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

#include "SerializationExportWrapperForCpp.hpp"
// Declare identifier for the serializer
CHASTE_CLASS_EXPORT(CellType)
