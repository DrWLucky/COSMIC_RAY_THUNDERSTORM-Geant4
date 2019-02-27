#pragma once

#include "AnalysisManager.hh"
#include "G4ThreeVector.hh"
#include "G4UserSteppingAction.hh"
#include "RegionInformation.hh"
#include "globals.hh"
#include <vector>

#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"
#include "G4UnitsTable.hh"
#include <CLHEP/Units/SystemOfUnits.h>

#include "Settings.hh"

#include <sys/time.h>
#include <time.h>

class DetectorConstruction;

class EventAction;

// ....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class SteppingAction : public G4UserSteppingAction
{
public:
    SteppingAction(DetectorConstruction *, EventAction *);

    ~SteppingAction() override;

    void UserSteppingAction(const G4Step *aStep) override;

private:

    Settings *settings = Settings::getInstance();

    AnalysisManager *analysis = AnalysisManager::getInstance();
    DetectorConstruction *fDetector;
    EventAction *fEventAction;
    G4StepPoint *thePrePoint = nullptr;

    const G4int PDG_phot = 22;
    const G4int PDG_elec = 11;
    const G4int PDG_posi = -11;

    double get_wall_time();

    double computation_time_length_for_event_limit = 900.; // 15 minutes

    //    G4int part_ID;
    //    G4int previous_part_ID;

    bool is_inside_eField_region(const G4double &alt, const G4double &xx, const G4double &zz);

    G4double alt_min = settings->EFIELD_REGION_ALT_CENTER - settings->EFIELD_REGION_LEN / 2.0; // km
    G4double alt_max = settings->EFIELD_REGION_ALT_CENTER + settings->EFIELD_REGION_LEN / 2.0; // km

    uint nb_skip = 0;
};

// ....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
