//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//

// Added to mimic time dependent simulation, and to count the number of energetic electron every 5 microseconds
// and turn OFF the electric field if there is more than 100000 (-> RREA)

// can be a bad idea to set it on, because it can use a lot of memory compared to default G4 behaviour

#pragma once

#include "G4SystemOfUnits.hh"
#include "G4ThreeVector.hh"
#include "G4UserStackingAction.hh"
#include "Settings.hh"
#include "globals.hh"
#include <algorithm>
#include <vector>

class G4Track;

class BaseStackingActionMessenger;

class BaseStackingAction : public G4UserStackingAction
{
public:
    BaseStackingAction();

    ~BaseStackingAction() override;

public:
    G4ClassificationOfNewTrack ClassifyNewTrack(const G4Track *aTrack) override;

    void NewStage() override;

    void PrepareNewEvent() override;

private:

    Settings *settings = Settings::getInstance();

    bool does_not_contain(const G4int ID, const std::vector<G4int> &LIST_IDS);

    void print_status();

    bool is_inside_eField_region(const G4double &alt, const G4double &xx, const G4double &zz);

    void check_PART_NB_LIMIT();

protected:
    G4StackManager *stackManager;
    //        G4String part_name;
    std::vector<G4int> LIST_ENERGETIC_PART_IDS;
    //        std::vector<G4double> LIST_ENERGIES;
    G4double VARIABLE_TIME_LIMIT;
    G4double TIME_STEP = -77.88 * microsecond; // just for initialization, should correspond to a fraction of a RREA avalanche length (time)
    const G4double ENER_THRES = 800.0 * keV;
    const uint ENERGETIC_PART_NB_LIMIT = 10000;
    uint EVENT_NB = 0;

    G4double alt_min = settings->EFIELD_REGION_ALT_CENTER - settings->EFIELD_REGION_LEN / 2.0; // km
    G4double alt_max = settings->EFIELD_REGION_ALT_CENTER + settings->EFIELD_REGION_LEN / 2.0; // km
};
