#include "SteppingAction.hh"
#include "DetectorConstruction.hh"
#include "EventAction.hh"
#include "G4RunManager.hh"
#include "Run.hh"

// ....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

SteppingAction::SteppingAction(DetectorConstruction *det, EventAction *event) : G4UserSteppingAction(), fDetector(det), fEventAction(event)
{
}

// ....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

SteppingAction::~SteppingAction() = default;

// ....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void SteppingAction::UserSteppingAction(const G4Step *aStep)
{
    //////// AVOID PUTTING THIS LINE, it will produce incorrect energy record
    //        if (aStep->GetTrack()->GetTrackStatus() != fAlive) return;
    ////////////////////////////////////////////

    if (aStep->GetPreStepPoint()->GetGlobalTime() > 6.0 * second)
    {
        aStep->GetTrack()->SetTrackStatus(fStopAndKill);
        return;
    }

    if (aStep->GetPreStepPoint()->GetPosition().y() <= 0.0 * km)
    {
        aStep->GetTrack()->SetTrackStatus(fStopAndKill);
        return;
    }

    if (aStep->GetPreStepPoint()->GetPosition().y() > 120.0 * km)
    {
        aStep->GetTrack()->SetTrackStatus(fStopAndKill);
        return;
    }

    if (std::abs(aStep->GetPreStepPoint()->GetPosition().x()) > settings->CR_SAMPLING_XY_HALF_SIZE * km)
    {
        aStep->GetTrack()->SetTrackStatus(fStopAndKill);
        return;
    }

    if (std::abs(aStep->GetPreStepPoint()->GetPosition().z()) > settings->CR_SAMPLING_XY_HALF_SIZE * km)
    {
        aStep->GetTrack()->SetTrackStatus(fStopAndKill);
        return;
    }

    ////////////////////////////
    // turn off electric field if event is too long (i.e. requires too much
    // computation time)

    if (settings->USE_WALL_TIME_LIMIT_FOR_EVENT)
    {
        nb_skip++;

        if (nb_skip > 1600000)
        {
            double WT = get_wall_time();

            if (abs(WT - settings->wall_T_begin_event) > computation_time_length_for_event_limit)
            {
                settings->current_efield_status = settings->OFF;
                settings->RREA_PART_NB_LIMIT_HAS_BEEN_REACHED = 1;
            }

            nb_skip = 0;
#ifndef NDEBUG // debug mode
            G4cout << "nb skip reset" << G4endl;
#endif
        }
    }

    ////////////////////////////
    if (std::abs(aStep->GetPreStepPoint()->GetPosition().x()) > settings->EFIELD_XY_HALF_SIZE * km)
    {
        return;
    }

    if (std::abs(aStep->GetPreStepPoint()->GetPosition().z()) > settings->EFIELD_XY_HALF_SIZE * km)
    {
        return;
    }

    ///////////////////////
    const G4int PDG = aStep->GetTrack()->GetParticleDefinition()->GetPDGEncoding();
    ///////////////////////

    // cleaning low energy particles to improve performance
    if (aStep->GetPreStepPoint())
    {
        // avoid removing positron below threshold to make sure we still have
        // annihilation
        if (PDG != PDG_posi)
        {
            if (aStep->GetPreStepPoint()->GetKineticEnergy() < 8.0 * keV)
            {
                aStep->GetTrack()->SetTrackStatus(fStopAndKill);
                return;
            }
        }
    }

    ////////////////////////////

    if (!(PDG == PDG_elec || PDG == PDG_phot || PDG == PDG_posi))
    {
        return;
    }

    if (settings->USE_STACKING_ACTION)
    {
        if (aStep->GetPreStepPoint())
        {
            if ((settings->current_efield_status != settings->OFF) && (is_inside_eField_region(aStep->GetPreStepPoint()->GetPosition().y(), aStep->GetPreStepPoint()->GetPosition().x(), aStep->GetPreStepPoint()->GetPosition().z())))
            {
                if (aStep->GetTrack()->GetGlobalTime() > settings->VARIABLE_TIME_LIMIT)
                {
                    aStep->GetTrack()->SetTrackStatus(fSuspend);
                }
            }
        }
    }

    ////////////////// PART THAT WAS BEFORE PART OF Sens Det

    const G4int PDG_nb = aStep->GetTrack()->GetParticleDefinition()->GetPDGEncoding();

    //    if (!(PDG_nb == PDG_phot || PDG_nb == PDG_posi || PDG_nb == PDG_elec))
    //    return;

    thePrePoint = aStep->GetPreStepPoint();

    const G4double pre_y = aStep->GetPreStepPoint()->GetPosition().y() / km;
    const G4double post_y = aStep->GetPostStepPoint()->GetPosition().y() / km;

    for (uint i_alt = 0; i_alt < settings->RECORD_ALTITUDES.size(); ++i_alt)
    {
        G4double rec_alt = settings->RECORD_ALTITUDES[i_alt];

        if ((pre_y >= rec_alt && post_y < rec_alt) || (pre_y <= rec_alt && post_y > rec_alt)) // particle is crossing 20 km layer
        {
            //    if (thePrePoint->GetStepStatus() == fGeomBoundary) // if the
            //    particle has just entered the volume ; should not be necessary, but
            //    won't hurt
            //        {
            const G4double ener = thePrePoint->GetKineticEnergy();
            //                    const G4int ID_part =
            //                    aStep->GetTrack()->GetTrackID();
            const G4double momy = thePrePoint->GetMomentumDirection().y();

            // WARNING : PARTICLES ARE ALLOWED TO BE RECORED SEVERAL TIMES

            if (ener > settings->ENERGY_MIN_RECORD && ener < settings->ENERGY_MAX_RECORD)
                //            if (aStep->GetTrack()->GetKineticEnergy() >
                //            settings->ENERGY_MIN_RECORD)
            {
                if (PDG_nb == -13)
                {
                    analysis->fill_histogram_E(0, i_alt, ener);
                    analysis->fill_histogram_mX(0, i_alt, thePrePoint->GetMomentumDirection().x());
                    analysis->fill_histogram_mY(0, i_alt, thePrePoint->GetMomentumDirection().y());
                    analysis->fill_histogram_mZ(0, i_alt, thePrePoint->GetMomentumDirection().z());
                }

                if (PDG_nb == 13)
                {
                    analysis->fill_histogram_E(1, i_alt, ener);
                    analysis->fill_histogram_mX(1, i_alt, thePrePoint->GetMomentumDirection().x());
                    analysis->fill_histogram_mY(1, i_alt, thePrePoint->GetMomentumDirection().y());
                    analysis->fill_histogram_mZ(1, i_alt, thePrePoint->GetMomentumDirection().z());
                }


                if (momy > 0.0)
                {
                    if (PDG_nb == -13)
                    {
                        analysis->counter_up[i_alt][0]++;
                    }
                    else if (PDG_nb == 13)
                    {
                        analysis->counter_up[i_alt][1]++;
                    }
                }

                if (momy < 0.0)
                {
                    if (PDG_nb == -13)
                    {
                        analysis->counter_down[i_alt][0]++;
                    }
                    else if (PDG_nb == 13)
                    {
                        analysis->counter_down[i_alt][1]++;
                    }
                }

                analysis->add_NB_OUTPUT();
            }
        }
    }

} // SteppingAction::UserSteppingAction

bool SteppingAction::is_inside_eField_region(const G4double &alt, const G4double &xx, const G4double &zz)
// alt assumed in km
{
    return alt > alt_min && alt < alt_max && std::fabs(xx) < settings->EFIELD_XY_HALF_SIZE && std::fabs(zz) < settings->EFIELD_XY_HALF_SIZE;
}

// ------------------------------------------------------------------------

double SteppingAction::get_wall_time()
// returns time in seconds
{
    struct timeval tv{};

    gettimeofday(&tv, nullptr);
    return tv.tv_sec + (tv.tv_usec / 1000000.0);
}
