#include <boost/regex.hpp>
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
    *this->mpOutStream << "<time t=\"" << SimulationTime::Instance()->GetTime() << "\">\n";
}

    template<unsigned ELEMENT_DIM, unsigned SPACE_DIM>
void XMLCellWriter<ELEMENT_DIM, SPACE_DIM>::VisitCell(CellPtr pCell, AbstractCellPopulation<ELEMENT_DIM, SPACE_DIM>* pCellPopulation)
{
    *this->mpOutStream << "<cell ";

    //unsigned location_index = pCellPopulation->GetLocationIndexUsingCell(pCell);

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
    *this->mpOutStream << "birth_time=\"" << pCell->GetCellCycleModel()->GetBirthTime() << "\" ";

    unsigned ancestor_index = pCell->GetCellData()->GetItem("direct_ancestor");

    *this->mpOutStream << "ancestor=\"" << ancestor_index << "\" ";

    double boundary = pCell->GetCellData()->GetItem("boundary");

    *this->mpOutStream << "perimeter=\"" << boundary << "\" ";

    double contact_boundary = pCell->GetCellData()->GetItem("contact_boundary");

    *this->mpOutStream << "contact_perimeter=\"" << contact_boundary << "\" ";

    *this->mpOutStream << "neighbours=\"";

    std::set<unsigned> neighbours = pCellPopulation->GetNeighbouringLocationIndices(pCell);

    for (std::set<unsigned>::iterator neighbour_iter = neighbours.begin();
            neighbour_iter != neighbours.end();
            ++neighbour_iter)
    {
        *this->mpOutStream << " " << *neighbour_iter;
    }

    *this->mpOutStream << "\" ";

    std::string state = pCell->GetCellProliferativeType()->GetIdentifier();

//    boost::regex state_regex("([\\w]+)CellProliferativeType");
//    boost::smatch match;

//    if (boost::regex_search(state, match, state_regex))
//    {
//        state = match[1];
//    }

    *this->mpOutStream << "state=\"" << state << "\" ";

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

#include "SerializationExportWrapperForCpp.hpp"
// Declare identifier for the serializer
EXPORT_TEMPLATE_CLASS_ALL_DIMS(XMLCellWriter)
