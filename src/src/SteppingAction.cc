#include "SteppingAction.hh"
#include "DetectorConstruction.hh"
#include "Run.hh"
#include "EventAction.hh"
#include "G4RunManager.hh"
#include "G4SystemOfUnits.hh"
#include "G4UnitsTable.hh"



// ....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

SteppingAction::SteppingAction(DetectorConstruction *det, EventAction *event)
    : G4UserSteppingAction(), fDetector(det), fEventAction(event) {}

// ....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

SteppingAction::~SteppingAction() {}

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

    if (std::abs(aStep->GetPreStepPoint()->GetPosition().x()) > Settings::CR_SAMPLING_XY_HALF_SIZE * km)
        {
            aStep->GetTrack()->SetTrackStatus(fStopAndKill);
            return;
        }

    if (std::abs(aStep->GetPreStepPoint()->GetPosition().z()) > Settings::CR_SAMPLING_XY_HALF_SIZE * km)
        {
            aStep->GetTrack()->SetTrackStatus(fStopAndKill);
            return;
        }

    if (std::abs(aStep->GetPreStepPoint()->GetPosition().x()) > Settings::EFIELD_XY_HALF_SIZE * km)
        {
            return;
        }

    if (std::abs(aStep->GetPreStepPoint()->GetPosition().z()) > Settings::EFIELD_XY_HALF_SIZE * km)
        {
            return;
        }

    ///////////////////////
    const G4int PDG = aStep->GetTrack()->GetParticleDefinition()->GetPDGEncoding();
    ///////////////////////

    // cleaning low energy particles to improve performance
    if (aStep->GetPreStepPoint())
        {
            // avoid removing positron below threshold to make sure we still have annihilation
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
    // the event execution time can become very long if a particle is stuck.
    // these lines will kill the stuck particle but not terminate the event
    if (Settings::USE_WALL_TIME_LIMIT_FOR_EVENT)
        {
            part_ID = aStep->GetTrack()->GetTrackID();

            if (part_ID == previous_part_ID)
                {
                    double WT = get_wall_time();

                    if (abs(WT - Settings::wall_T_begin_event) > computation_length_for_event_limit)
                        {
                            aStep->GetTrack()->SetTrackStatus(fStopAndKill);
                            Settings::wall_T_begin_event = WT;
                            return;
                        }
                }

            previous_part_ID = part_ID;
        }

    ////////////////////////////

    if (!(PDG == PDG_elec || PDG == PDG_phot || PDG == PDG_posi))
        {
            return;
        }

    if (Settings::USE_STACKING_ACTION)
        {
            if (aStep->GetPreStepPoint())
                {
                    if ((Settings::current_efield_status != Settings::OFF) &&
                        (is_inside_eField_region(aStep->GetPreStepPoint()->GetPosition().y(),
                                                 aStep->GetPreStepPoint()->GetPosition().x(),
                                                 aStep->GetPreStepPoint()->GetPosition().z()))
                       )
                        {
                            if (aStep->GetTrack()->GetGlobalTime() > Settings::VARIABLE_TIME_LIMIT)
                                {
                                    aStep->GetTrack()->SetTrackStatus(fSuspend);
                                }
                        }
                }
        }

    ////////////////// PART THAT WAS BEFORE PART OF Sens Det

    const G4int PDG_nb = aStep->GetTrack()->GetParticleDefinition()->GetPDGEncoding();

    //    if (!(PDG_nb == PDG_phot || PDG_nb == PDG_posi || PDG_nb == PDG_elec)) return;

    thePrePoint = aStep->GetPreStepPoint();

    const G4double pre_y = aStep->GetPreStepPoint()->GetPosition().y() / km;
    const G4double post_y = aStep->GetPostStepPoint()->GetPosition().y() / km;

    for (uint i_alt = 0; i_alt < Settings::RECORD_ALTITUDES.size(); ++i_alt)
        {
            G4double rec_alt = Settings::RECORD_ALTITUDES[i_alt];

            if ((pre_y >= rec_alt && post_y < rec_alt) || (pre_y <= rec_alt && post_y > rec_alt)) // particle is crossing 20 km layer
                {
                    //    if (thePrePoint->GetStepStatus() == fGeomBoundary) // if the particle has just entered the volume ; should not be necessary, but won't hurt
                    //        {
                    const G4double ener = thePrePoint->GetKineticEnergy();
                    //                    const G4int ID_part = aStep->GetTrack()->GetTrackID();
                    const G4double momy = thePrePoint->GetMomentumDirection().y();

                    // WARNING : PARTICLES ARE ALLOWED TO BE RECORED SEVERAL TIMES

                    if (ener > Settings::ENERGY_MIN && ener < Settings::ENERGY_MAX)
                        //            if (aStep->GetTrack()->GetKineticEnergy() > Settings::ENERGY_MIN)
                        {

                            if (PDG_nb == 22)
                                {
                                    analysis->fill_histogram_ener(0, i_alt, ener);
                                    analysis->fill_histogram_momX(0, i_alt, thePrePoint->GetMomentumDirection().x());
                                    analysis->fill_histogram_momY(0, i_alt, thePrePoint->GetMomentumDirection().y());
                                    analysis->fill_histogram_momZ(0, i_alt, thePrePoint->GetMomentumDirection().z());
                                }

                            if (PDG_nb == 11)
                                {
                                    analysis->fill_histogram_ener(1, i_alt, ener);
                                    analysis->fill_histogram_momX(1, i_alt, thePrePoint->GetMomentumDirection().x());
                                    analysis->fill_histogram_momY(1, i_alt, thePrePoint->GetMomentumDirection().y());
                                    analysis->fill_histogram_momZ(1, i_alt, thePrePoint->GetMomentumDirection().z());
                                }

                            if (PDG_nb == -11)
                                {
                                    analysis->fill_histogram_ener(2, i_alt, ener);
                                    analysis->fill_histogram_momX(2, i_alt, thePrePoint->GetMomentumDirection().x());
                                    analysis->fill_histogram_momY(2, i_alt, thePrePoint->GetMomentumDirection().y());
                                    analysis->fill_histogram_momZ(2, i_alt, thePrePoint->GetMomentumDirection().z());
                                }

                            if (momy > 0.0)
                                {
                                    if (PDG_nb == 22)
                                        {
                                            analysis->photon_counter_up[i_alt]++;
                                        }
                                    else if (PDG_nb == 11)
                                        {
                                            analysis->electron_counter_up[i_alt]++;
                                        }
                                    else if (PDG_nb == -11)
                                        {
                                            analysis->positron_counter_up[i_alt]++;
                                        }
                                }

                            if (momy < 0.0)
                                {
                                    if (PDG_nb == 22)
                                        {
                                            analysis->photon_counter_down[i_alt]++;
                                        }
                                    else if (PDG_nb == 11)
                                        {
                                            analysis->electron_counter_down[i_alt]++;
                                        }
                                    else if (PDG_nb == -11)
                                        {
                                            analysis->positron_counter_down[i_alt]++;
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
    if (alt > alt_min && alt < alt_max
        && abs(xx) < Settings::EFIELD_XY_HALF_SIZE
        && abs(zz) < Settings::EFIELD_XY_HALF_SIZE)
        {
            return true;
        }
    else
        {
            return false;
        }
}


// ------------------------------------------------------------------------

double SteppingAction::get_wall_time()
// returns time in seconds
{
    struct timeval tv;

    gettimeofday(&tv, NULL);
    return tv.tv_sec + (tv.tv_usec / 1000000.0);
}