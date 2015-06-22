#include <boost/regex.hpp>
#include "XMLCellWriter.hpp"
#include "AbstractCellWriter.hpp"
#include "AbstractCellPopulation.hpp"
#include "VertexBasedCellPopulation.hpp"
#include "SimulationTime.hpp"
#include "CellType.hpp"
#include "MutableVertexMesh.hpp"
#include "VertexMesh.hpp"

    template<unsigned ELEMENT_DIM, unsigned SPACE_DIM>
XMLCellWriter<ELEMENT_DIM, SPACE_DIM>::XMLCellWriter()
    : AbstractCellWriter<ELEMENT_DIM, SPACE_DIM>("cell_data.xml")
{
};

    template<unsigned ELEMENT_DIM, unsigned SPACE_DIM>
void XMLCellWriter<ELEMENT_DIM, SPACE_DIM>::SetSimulationInfo(std::string simulationType="", std::string typeList="",
        std::string axisDivision="", std::string cellCycleModel="", std::string extraSimInfo="")
{
    mSimulationType = simulationType;
    mTypeList = typeList;
    mAxisDivision = axisDivision;
    mCellCycleModel = cellCycleModel;
    mExtraSimInfo = extraSimInfo;
}

    template<unsigned ELEMENT_DIM, unsigned SPACE_DIM>
void XMLCellWriter<ELEMENT_DIM, SPACE_DIM>::OpenOutputFile(OutputFileHandler& rOutputFileHandler)
{
    AbstractCellWriter<ELEMENT_DIM, SPACE_DIM>::OpenOutputFile(rOutputFileHandler);
    *this->mpOutStream << "<simulation simulation_type=\"" << mSimulationType << "\" ";
    *this->mpOutStream << "type_list=\"" << mTypeList << "\" ";
    *this->mpOutStream << "axis_division=\"" << mAxisDivision << "\" ";
    *this->mpOutStream << "cell_cycle_model=\"" << mCellCycleModel << "\" ";
    *this->mpOutStream << "time_step=\"" << SimulationTime::Instance()->GetTimeStep() << "\" ";
    *this->mpOutStream << "extra_sim_info=\"" << mExtraSimInfo << "\"";
    *this->mpOutStream << ">\n";
}

    template<unsigned ELEMENT_DIM, unsigned SPACE_DIM>
void XMLCellWriter<ELEMENT_DIM, SPACE_DIM>::WriteNewline()
{
    *this->mpOutStream << "</time>\n";
}

    template<unsigned ELEMENT_DIM, unsigned SPACE_DIM>
void XMLCellWriter<ELEMENT_DIM, SPACE_DIM>::WriteTimeStamp()
{
    *this->mpOutStream << "<time t=\"" << SimulationTime::Instance()->GetTime() << "\" tau=\"" << SimulationTime::Instance()->GetTimeStepsElapsed() << "\">\n";
}

    template<unsigned ELEMENT_DIM, unsigned SPACE_DIM>
void XMLCellWriter<ELEMENT_DIM, SPACE_DIM>::VisitCell(CellPtr pCell, AbstractCellPopulation<ELEMENT_DIM, SPACE_DIM>* pCellPopulation)
{
    *this->mpOutStream << "<cell ";

    unsigned cell_id = pCell->GetCellId();
    c_vector<double, SPACE_DIM> centre_location = pCellPopulation->GetLocationOfCellCentre(pCell);
    double volume = pCell->GetCellData()->GetItem("volume");

    unsigned location_index = pCellPopulation->GetLocationIndexUsingCell(pCell);

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

    unsigned generation = pCell->GetCellData()->GetItem("generation");

    *this->mpOutStream << "generation=\"" << generation << "\" ";

    double boundary = pCell->GetCellData()->GetItem("boundary");

    *this->mpOutStream << "perimeter=\"" << boundary << "\" ";

    double contact_boundary = pCell->GetCellData()->GetItem("contact_boundary");

    *this->mpOutStream << "contact_perimeter=\"" << boundary - contact_boundary << "\" ";

    *this->mpOutStream << "neighbours=\"";

    std::set<unsigned> neighbours = pCellPopulation->GetNeighbouringLocationIndices(pCell);

    for (std::set<unsigned>::iterator neighbour_iter = neighbours.begin();
            neighbour_iter != neighbours.end();
            ++neighbour_iter)
    {
        *this->mpOutStream << " " << *neighbour_iter;
    }

    *this->mpOutStream << "\" ";

    if (pCell->HasCellProperty<CellType>())
    {
        CellPropertyCollection collection = pCell->rGetCellPropertyCollection().GetProperties<CellType>();
        boost::shared_ptr<CellType> p_type = boost::static_pointer_cast<CellType>(collection.GetProperty());
        unsigned type = p_type->GetType();
        *this->mpOutStream << "type=\"" << type << "\" ";
    }

    if (dynamic_cast<VertexBasedCellPopulation<ELEMENT_DIM>*>(pCellPopulation))
    {
        double elongation = dynamic_cast<VertexBasedCellPopulation<ELEMENT_DIM>*>(pCellPopulation)->rGetMesh().GetElongationShapeFactorOfElement(location_index);
        *this->mpOutStream << "elongation=\"" << elongation << "\" ";

        c_vector<double, SPACE_DIM> short_axis = dynamic_cast<VertexBasedCellPopulation<ELEMENT_DIM>*>(pCellPopulation)->rGetMesh().GetShortAxisOfElement(location_index);
        *this->mpOutStream << "minor_axis=\"" << short_axis[0] << " " << short_axis[1] << "\" ";
    }

    try
    {
        double ext_force_x = pCell->GetCellData()->GetItem("ext_force_x");
        double ext_force_y = pCell->GetCellData()->GetItem("ext_force_y");
        *this->mpOutStream << "ext_force\"" << ext_force_x << " " << ext_force_y << "\" ";
    }
    catch (Exception& e)
    {
    }

    try
    {
        double polarity_x = pCell->GetCellData()->GetItem("polarity_x");
        double polarity_y = pCell->GetCellData()->GetItem("polarity_y");
        *this->mpOutStream << "polarity\"" << polarity_x << " " << polarity_y << "\" ";
    }
    catch (Exception& e)
    {
    }

    try
    {
        double contractility = pCell->GetCellData()->GetItem("contractility");
        *this->mpOutStream << "contractility\"" << contractility << "\" ";
    }
    catch (Exception& e)
    {
    }

    double pressure = pCell->GetCellData()->GetItem("pressure");
    *this->mpOutStream << "pressure\"" << pressure << "\" ";

    double target_area = pCell->GetCellData()->GetItem("target area");
    *this->mpOutStream << "target_area\"" << target_area << "\" ";

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
