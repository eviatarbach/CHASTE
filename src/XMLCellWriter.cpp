#include "XMLCellWriter.hpp"
#include "AbstractCellWriter.hpp"
#include "AbstractCellPopulation.hpp"
#include "SimulationTime.hpp"

    template<unsigned ELEMENT_DIM, unsigned SPACE_DIM>
XMLCellWriter<ELEMENT_DIM, SPACE_DIM>::XMLCellWriter()
    : AbstractCellWriter<ELEMENT_DIM, SPACE_DIM>("cell_data.xml")
{
};

    template<unsigned ELEMENT_DIM, unsigned SPACE_DIM>
void XMLCellWriter<ELEMENT_DIM, SPACE_DIM>::WriteNewline()
{
    *this->mpOutStream << "</time>\n";
}

    template<unsigned ELEMENT_DIM, unsigned SPACE_DIM>
void XMLCellWriter<ELEMENT_DIM, SPACE_DIM>::WriteTimeStamp()
{
    *this->mpOutStream << "<time t=\"" << SimulationTime::Instance()->GetTime() << "\"> ";
}

    template<unsigned ELEMENT_DIM, unsigned SPACE_DIM>
void XMLCellWriter<ELEMENT_DIM, SPACE_DIM>::VisitCell(CellPtr pCell, AbstractCellPopulation<ELEMENT_DIM, SPACE_DIM>* pCellPopulation)
{
    *this->mpOutStream << "<cell ";

    unsigned cell_id = pCell->GetCellId();
    c_vector<double, SPACE_DIM> centre_location = pCellPopulation->GetLocationOfCellCentre(pCell);
    double volume = pCellPopulation->GetVolumeOfCell(pCell);

    *this->mpOutStream << "cell_id=\"" << cell_id << "\" ";

    *this->mpOutStream << "x=\"" << centre_location[0] << "\" ";
    *this->mpOutStream << "y=\"" << centre_location[1] << "\" ";

    if (volume < DBL_MAX)   // Only write cells with finite volume (avoids a case for boundary cells in MeshBasedCellPopulation)
    {
        *this->mpOutStream << "area=\"" << volume << "\" ";
    }

    // Write birth time
    *this->mpOutStream << "birth_time=\"" << SimulationTime::Instance()->GetTime() - pCell->GetAge() << "\" ";

    pCellPopulation->SetCellAncestorsToLocationIndices();
    double ancestor_index = (pCell->GetAncestor() == UNSIGNED_UNSET) ? (-1.0) : (double)pCell->GetAncestor();
    *this->mpOutStream << "ancestor=\"" << ancestor_index << "\" ";

    *this->mpOutStream << "/>\n";
}

// Dummy implementation because it is required
    template<unsigned ELEMENT_DIM, unsigned SPACE_DIM>
double XMLCellWriter<ELEMENT_DIM, SPACE_DIM>::GetCellDataForVtkOutput(CellPtr pCell, AbstractCellPopulation<ELEMENT_DIM, SPACE_DIM>* pCellPopulation)
{
    return 0.0;
}


// Explicit instantiation
template class XMLCellWriter<1,1>;
template class XMLCellWriter<1,2>;
template class XMLCellWriter<2,2>;
template class XMLCellWriter<1,3>;
template class XMLCellWriter<2,3>;
template class XMLCellWriter<3,3>;
