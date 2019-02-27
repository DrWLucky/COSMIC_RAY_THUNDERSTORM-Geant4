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


#pragma once

#include "globals.hh"
#include <vector>
#include "G4ThreeVector.hh"
#include "G4Track.hh"
#include "g4csv.hh"
#include <array>
#include "Settings.hh"
#include <algorithm>

// #include "g4root.hh"
// #include "g4xml.hh"

// following singleton pattern

// ....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

class AnalysisManager
{
private:

    AnalysisManager();

    static AnalysisManager *instance;

private:
    ////////////////////////////////////////////////////
    //        static const uint NB_ENER_BINS = 256;
    //        static const uint NB_MOM_BINS = 128;

public:

    ~AnalysisManager();

    static AnalysisManager *getInstance();

    void write_output_file_endOf_program();

    G4int NB_OUTPUT() const;

    void check_if_should_use_stacking_action();

    void add_NB_OUTPUT();

    void fill_histogram_E(const G4int idx_part, const G4int idx_alt, const G4double &value);
    void fill_histogram_mX(const G4int idx_part, const G4int idx_alt, const G4double &value);
    void fill_histogram_mY(const G4int idx_part, const G4int idx_alt, const G4double &value);
    void fill_histogram_mZ(const G4int idx_part, const G4int idx_alt, const G4double &value);

    static const uint nbp = 2;
    static const uint nbalt_max = 6;
    static const uint ngride = 256;
    static const uint ngridm = 128;
    uint PART_SPEC[nbp][nbalt_max][ngride];
    uint PART_MOM_X[nbp][nbalt_max][ngridm];
    uint PART_MOM_Y[nbp][nbalt_max][ngridm];
    uint PART_MOM_Z[nbp][nbalt_max][ngridm];
    uint counter_up[nbp][nbalt_max];
    uint counter_down[nbp][nbalt_max];

private:

    Settings *settings = Settings::getInstance();

    G4String asciiFileName1;

    G4int NB_OUTPUT_ = 0;

    std::vector<G4String> RECORDED_OUTPUT_STRINGS;

    std::ofstream asciiFile_analysis;

    //    bool not_contains(const G4int &x, const std::vector<G4int> &v);

    G4double get_scale(const G4double alt);

    G4double interpolate(std::vector<G4double> &xData, std::vector<G4double> &yData, G4double x, bool extrapolate);

    ////////////////////////////////////////////////////

    std::vector<double> ENER_GRID;
    std::vector<double> MOM_GRID;


    std::vector<double> get_ener_grid();

    std::vector<double> get_MOM_grid();


};
