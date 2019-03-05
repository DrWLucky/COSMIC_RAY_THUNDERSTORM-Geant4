//
// ********************************************************************
#include "StackingAction.hh"
#include "G4RunManager.hh"

// Added to mimic time dependent simulation, and to count the number of
// energetic electron every 5 microseconds
// and turn OFF the electric field if there is more than 100000 (-> RREA)

// can be a bad idea to set it on, because it can use a lot of memory compared
// to default G4 behaviour

BaseStackingAction::BaseStackingAction()
{
    TIME_STEP = settings->DELTA_T;
}

BaseStackingAction::~BaseStackingAction()
{
}

void BaseStackingAction::PrepareNewEvent()
{
    LIST_ENERGETIC_PART_IDS.clear();

    VARIABLE_TIME_LIMIT = 11.0 * millisecond; // propagation time of light from 50 km to about 15 km
    EVENT_NB++;

    settings->current_efield_status = settings->initial_efield_status;

    settings->VARIABLE_TIME_LIMIT = VARIABLE_TIME_LIMIT;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

G4ClassificationOfNewTrack BaseStackingAction::ClassifyNewTrack(const G4Track *aTrack)
{
    if (settings->current_efield_status == settings->OFF)
    {
        return fUrgent;
    }

    if (!is_inside_eField_region(aTrack->GetPosition().y() / km, aTrack->GetPosition().x() / km, aTrack->GetPosition().z() / km))
    {
        return fUrgent; // ignore if we are not in the E-field region
    }

    // Warning : dot not use "aTrack->GetStep()->GetPreStepPoint()", this is not
    // the stepping action, step may not exist

    const G4int ID = aTrack->GetTrackID();

    // if energy > 500 keV and if the ID is not already saved
    if (does_not_contain(ID, LIST_ENERGETIC_PART_IDS))
    {
        if (aTrack->GetKineticEnergy() > ENER_THRES)
        {
            LIST_ENERGETIC_PART_IDS.push_back(ID); // save the ID

            check_PART_NB_LIMIT();
        }
    }

    if (aTrack->GetGlobalTime() > VARIABLE_TIME_LIMIT)
    {
        return fWaiting;
    }

    // default classification
    return fUrgent;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

void BaseStackingAction::print_status()
{
    G4cout << "current max time : " << VARIABLE_TIME_LIMIT / microsecond << " microsecond" << G4endl;
    G4cout << ">0.1 MeV electron count : " << LIST_ENERGETIC_PART_IDS.size() << G4endl;
    G4cout << "Event nb : " << EVENT_NB << G4endl;
    //    G4cout << part_name << G4endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

void BaseStackingAction::NewStage() // called when the "Urgent stack" is empty
// and the particles in the "waiting stack" are transfered to the "Urgent stack"
{
    //    check_PART_NB_LIMIT();

    if (settings->current_efield_status == settings->OFF)
    {
        return;
    }

    LIST_ENERGETIC_PART_IDS.clear();

    VARIABLE_TIME_LIMIT += TIME_STEP;
    //    LIST_ENERGETIC_PART_IDS.clear();
    //    print_status();
    settings->VARIABLE_TIME_LIMIT = VARIABLE_TIME_LIMIT;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

void BaseStackingAction::check_PART_NB_LIMIT()
{
    if (LIST_ENERGETIC_PART_IDS.size() > ENERGETIC_PART_NB_LIMIT)
    {
        settings->current_efield_status = settings->OFF;
        LIST_ENERGETIC_PART_IDS.clear();
        settings->RREA_PART_NB_LIMIT_HAS_BEEN_REACHED = 1;
    }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

bool BaseStackingAction::does_not_contain(const G4int ID, const std::vector<G4int> &LIST_IDS)
{
    if (LIST_IDS.empty())
    {
        return true;
    }

    return !(std::find(LIST_IDS.begin(), LIST_IDS.end(), ID) != LIST_IDS.end());
}

bool BaseStackingAction::is_inside_eField_region(const G4double &alt, const G4double &xx, const G4double &zz)
// alt assumed in km
{
    return alt > alt_min && alt < alt_max && std::fabs(xx) < settings->EFIELD_XY_HALF_SIZE && std::fabs(zz) < settings->EFIELD_XY_HALF_SIZE;
}
