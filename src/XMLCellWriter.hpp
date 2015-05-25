#ifndef ABSTRACTXMLCELLBASEDWRITER_HPP_
#define ABSTRACTXMLCELLBASEDWRITER_HPP_

#include "ChasteSerialization.hpp"
#include "AbstractCellWriter.hpp"

template<unsigned ELEMENT_DIM, unsigned SPACE_DIM>
class XMLCellWriter : public AbstractCellWriter<ELEMENT_DIM, SPACE_DIM>
{
public:
    XMLCellWriter();

    void VisitCell(CellPtr pCell, AbstractCellPopulation<ELEMENT_DIM, SPACE_DIM>* pCellPopulation);

    void WriteTimeStamp();

    void WriteNewline();

    double GetCellDataForVtkOutput(CellPtr pCell, AbstractCellPopulation<ELEMENT_DIM, SPACE_DIM>* pCellPopulation);

};

#endif
