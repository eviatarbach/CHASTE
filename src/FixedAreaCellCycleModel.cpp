#include "FixedAreaCellCycleModel.hpp"
#include "CellLabel.hpp"
#include "DifferentiatedCellProliferativeType.hpp"
#include "RandomNumberGenerator.hpp"
#include <time.h>

FixedAreaCellCycleModel::FixedAreaCellCycleModel()
    : AbstractSimpleCellCycleModel(),
      mDivisionVolume(DOUBLE_UNSET)
{
    double dt = 0.002;//SimulationTime::Instance()->GetTimeStep();
    mG1Duration = 5*dt;
    mG2Duration = dt;
    mSDuration = dt;
    mMDuration = dt;
}

void FixedAreaCellCycleModel::UpdateCellCyclePhase()
{
    if (mDivisionVolume == DOUBLE_UNSET)
    {
        EXCEPTION("The member variable mDivisionVolume has not yet been set.");
    }

    // Get cell volume
    double cell_volume = mpCell->GetCellData()->GetItem("volume");

    // Removes the cell label
    //mpCell->RemoveCellProperty<CellLabel>();

    RandomNumberGenerator* p_gen = RandomNumberGenerator::Instance();
    p_gen->Reseed(time(NULL));

    double dt = SimulationTime::Instance()->GetTimeStep();
    if (mCurrentCellCyclePhase == G_ONE_PHASE)
    {
        // Update G1 duration based on cell volume

        if (cell_volume < mDivisionVolume)
        {
            mG1Duration += dt;// + p_gen->NormalRandomDeviate(0.0, 0.0005)/30;
        }
    }

    double time_since_birth = GetAge();
    assert(time_since_birth >= 0);


    if (mpCell->GetCellProliferativeType()->IsType<DifferentiatedCellProliferativeType>())
    {
        mCurrentCellCyclePhase = G_ZERO_PHASE;
    }
    else if ( time_since_birth < GetMDuration() )
    {
        mCurrentCellCyclePhase = M_PHASE;
    }
    else if ( time_since_birth < GetMDuration() + mG1Duration)
    {
        mCurrentCellCyclePhase = G_ONE_PHASE;
    }
    else if ( time_since_birth < GetMDuration() + mG1Duration + GetSDuration())
    {
        mCurrentCellCyclePhase = S_PHASE;
    }
    else if ( time_since_birth < GetMDuration() + mG1Duration + GetSDuration() + GetG2Duration())
    {
        mCurrentCellCyclePhase = G_TWO_PHASE;
    }
}

AbstractCellCycleModel* FixedAreaCellCycleModel::CreateCellCycleModel()
{
    // Create a new cell-cycle model
    FixedAreaCellCycleModel* p_model = new FixedAreaCellCycleModel();

    /*
     * Set each member variable of the new cell-cycle model that inherits
     * its value from the parent.
     *
     * Note 1: some of the new cell-cycle model's member variables (namely
     * mBirthTime, mCurrentCellCyclePhase, mReadyToDivide, mTimeSpentInG1Phase,
     * mCurrentHypoxicDuration, mCurrentHypoxiaOnsetTime) will already have been
     * correctly initialized in its constructor.
     *
     * Note 2: one or more of the new cell-cycle model's member variables
     * may be set/overwritten as soon as InitialiseDaughterCell() is called on
     * the new cell-cycle model.
     */
    p_model->SetBirthTime(mBirthTime);
    p_model->SetDimension(mDimension);
    p_model->SetMinimumGapDuration(mMinimumGapDuration);
    p_model->SetStemCellG1Duration(mStemCellG1Duration);
    p_model->SetTransitCellG1Duration(mTransitCellG1Duration);
    p_model->SetSDuration(mSDuration);
    p_model->SetG2Duration(mG2Duration);
    p_model->SetMDuration(mMDuration);
    p_model->SetDivisionVolume(mDivisionVolume);

    return p_model;
}

void FixedAreaCellCycleModel::SetG1Duration()
{
}

void FixedAreaCellCycleModel::SetDivisionVolume(double divisionVolume)
{
    mDivisionVolume = divisionVolume;
}

bool FixedAreaCellCycleModel::ReadyToDivide()
{
    assert(mpCell != NULL);

    double volume = mpCell->GetCellData()->GetItem("volume");

    if (!mReadyToDivide)
    {
        if (volume >= mDivisionVolume)
        {
            mReadyToDivide = true;
        }
    }
    return mReadyToDivide;
}

void FixedAreaCellCycleModel::OutputCellCycleModelParameters(out_stream& rParamsFile)
{
    // Call method on direct parent class
    AbstractSimpleCellCycleModel::OutputCellCycleModelParameters(rParamsFile);
}

// Serialization for Boost >= 1.36
#include "SerializationExportWrapperForCpp.hpp"
CHASTE_CLASS_EXPORT(FixedAreaCellCycleModel)
